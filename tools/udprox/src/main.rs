use std::collections::HashMap;
use std::env;
use std::net::{SocketAddr, UdpSocket};
use std::str::{pattern, FromStr};
use std::sync::{Arc, Mutex};
use std::thread;

use colored::Colorize;

const MAX_BUF_SIZE: usize = 4096;

fn hex_to_bytes(hex: &str) -> Vec<u8> {
    hex::decode(hex).expect("Failed to decode hex string")
}

fn hex_dump(data: &[u8], pattern: &[u8]) {
    for (i, chunk) in data.chunks(16).enumerate() {
        print!("{:04x}:", i * 16);
        for byte in chunk {
            if pattern.contains(byte) {
                // Color the matched bytes
                print!("{}", format!("{:02x} ", byte).red());
            } else {
                print!("{:02x} ", byte);
            }
        }
        println!();
    }
}

fn match_and_replace(buf: &mut [u8], pattern: &[u8], replacement: &[u8]) -> bool {
    if let Some(pos) = buf
        .windows(pattern.len())
        .position(|window| window == pattern)
    {
        println!("Pattern found at position {}", pos);
        buf[pos..pos + replacement.len()].copy_from_slice(replacement);
        return true;
    }

    false
}

#[derive(Clone)]
struct Connection {
    client_addr: SocketAddr,
    server_socket: UdpSocket,
}

fn forward(socket: &UdpSocket, to_addr: &SocketAddr, from_buf: &[u8]) {
    match socket.send_to(from_buf, to_addr) {
        Ok(n) => println!("Forwarded packet: bytes={} to={}", n, to_addr),
        Err(e) => eprint!("Error forwarding packet: {}", e),
    }
}

fn new_connection(server_addr: &SocketAddr, client_addr: &SocketAddr) -> Connection {
    let server_socket = UdpSocket::bind("0.0.0.0:0").expect("Failed to bind to UDP socket");
    server_socket
        .connect(server_addr)
        .expect("Failed to connect to server");

    Connection {
        client_addr: client_addr.clone(),
        server_socket,
    }
}

fn run_connection(
    conn: Connection,
    proxy_socket: Arc<UdpSocket>,
    pattern: Vec<u8>,
    replacement: Vec<u8>,
) {
    let mut buf = [0; MAX_BUF_SIZE];
    loop {
        match conn.server_socket.recv(&mut buf) {
            Ok(n) => {
                if match_and_replace(&mut buf[..n], &pattern, &replacement) {
                    println!("Hex dump of matched packet:");
                    hex_dump(&buf[..n], &pattern);
                }
                proxy_socket
                    .send_to(&buf[..n], &conn.client_addr)
                    .unwrap_or_else(|e| eprintln!("Failed to replay to client {}", e));
            }
            Err(e) => eprintln!("Error reading from server: {}", e),
        }
    }
}

fn proxy(
    proxy_socket: Arc<UdpSocket>,
    server_addr: SocketAddr,
    client_dict: Arc<Mutex<HashMap<String, Connection>>>,
    pattern: Vec<u8>,
    replacement: Vec<u8>,
) {
    let mut buf = [0; MAX_BUF_SIZE];
    loop {
        match proxy_socket.recv_from(&mut buf) {
            Ok((n, from_addr)) => {
                let from_addr_str = from_addr.to_string();
                let mut client_dict_guard = client_dict.lock().unwrap();
                let conn = client_dict_guard
                    .entry(from_addr_str.clone())
                    .or_insert_with(|| new_connection(&server_addr, &from_addr));

                let client_conn = conn.clone();
                let proxy_socket_clone = Arc::clone(&proxy_socket);

                if !client_dict_guard.contains_key(&from_addr_str) {
                    let pattern_clone = pattern.clone();
                    let replacement_clone = replacement.clone();
                    thread::spawn(move || {
                        run_connection(
                            client_conn,
                            proxy_socket_clone,
                            pattern_clone,
                            replacement_clone,
                        );
                    });
                }

                conn.server_socket
                    .send(&buf[..n])
                    .unwrap_or_else(|e| eprintln!("Error forwarding to server: {}", e));
            }
            Err(e) => eprintln!("Error receiving from client: {}", e),
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 5 {
        eprintln!(
            "Usage: {} <server_addr> <proxy_addr> <match_pattern> <replace_pattern>",
            args[0]
        );
        std::process::exit(1);
    }

    let server_addr = SocketAddr::from_str(&args[1]).expect("Invalid server address");
    let proxy_addr = SocketAddr::from_str(&args[2]).expect("Invalid proxy address");

    // Convert match and replace patterns from hex strings to byte arrays
    let match_pattern = hex_to_bytes(&args[3]);
    let replace_pattern = hex_to_bytes(&args[4]);

    let proxy_socket = Arc::new(UdpSocket::bind(proxy_addr).expect("Failed to bind proxy socket"));
    let client_dict: Arc<Mutex<HashMap<String, Connection>>> = Arc::new(Mutex::new(HashMap::new()));

    println!(
        "Listening on {} and proxying to {}",
        proxy_addr, server_addr
    );

    proxy(
        proxy_socket,
        server_addr,
        client_dict,
        match_pattern,
        replace_pattern,
    );
}
