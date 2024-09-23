FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    cmake \
    gcc \
    make

WORKDIR /usr/src/dvfps-server

COPY . .

RUN mkdir build && cd build && cmake ..

RUN cd build && make

EXPOSE 12345/udp

CMD ["./build/game_server"]