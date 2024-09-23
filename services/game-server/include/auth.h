#ifndef AUTH_H
#define AUTH_H

// Function to add a player to the server
// Params:
//   username - The username of the player
//   token - The session token issued by the external REST API
void add_player(char* username, char* token);

// Function to validate the player's token by making an HTTP request to the external REST API
// Params:
//   token - The session token to validate
// Returns:
//   1 if the token is valid, 0 if invalid
int validate_player_token(const char *token);

// Function to check if a player has a valid token
// Params:
//   playerId - The ID of the player in the players array
// Returns:
//   1 if the player's token is valid, 0 if invalid
int check_player_auth(int playerId);

#endif