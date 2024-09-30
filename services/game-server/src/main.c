#include <stdio.h>
#include "networking.h"
#include "auth.h"
#include "game_logic.h"

int main(int argc, char *argv[]) {
    printf("Starting DVFPS multiplayer server...\n");

    if (!initialize_network()) {
        printf("Failed to initialize network.\n");
        return 1;
    }

    // Server loop
    while (1) {
        handle_player_connections();
        run_game_logic();
    }

    return 0;
} 
