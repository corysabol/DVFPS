#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

void encode_int32(int32_t value, uint8_t *buffer) {
    buffer[0] = (value >> 24) & 0xFF;
    buffer[1] = (value >> 16) & 0xFF;
    buffer[2] = (value >> 8) & 0xFF;
    buffer[3] = value & 0xFF;
}

uint32_t decode_uint32(const uint8_t *buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

/// @brief Encodes a float32 to a buffer of bytes in little-endian order suitable for Godot.
/// @param value 
/// @param buffer 
void encode_float32(float value, uint8_t *buffer) {}

float decode_float32(const uint8_t *buffer) {}

void encode_vector3(const vector3_t *vector, uint8_t *buffer) {
    memcpy(buffer, &vector->x, sizeof(float));
    memcpy(buffer + 4, &vector->y, sizeof(float));
    memcpy(buffer + 8, &vector->y, sizeof(float));
}

void decode_vector3(const uint8_t *buffer, vector3_t *vector) {
    memcpy(&vector->x, buffer, sizeof(float));
    memcpy(&vector->y, buffer + 4, sizeof(float));
    memcpy(&vector->z, buffer + 8, sizeof(float));
}