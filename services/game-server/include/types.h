#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint32_t player_id_t;

typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_ACTIVE,
    PLAYER_STATE_DISCONNECTED,
} player_state_t;

typedef struct {
    player_id_t id;
    char name[50];
    player_state_t state;
    uint32_t hp;
    
} player_t;

typedef struct {
    player_t players[6]; // list of players in a lobby / match
    uint32_t session_id; // lobby / match code
} game_session_t;

// Game logic types

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} vector3_t;

// encoding and decoding functions

void encode_int32(int32_t value, uint8_t *buffer);
uint32_t decode_uint32(const uint8_t *buffer);

void encode_float32(float value, uint8_t *buffer);
float decode_float32(const uint8_t *buffer);

#endif