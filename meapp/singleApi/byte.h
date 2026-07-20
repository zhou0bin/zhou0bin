#pragma once

#include <stdint.h>

void Byte_set_value(const uint8_t value, const int32_t start_pos,
                     const int32_t length, uint8_t* targat_byte);

uint8_t Byte_get_byte(const int32_t start_pos, const int32_t length,
                      const uint8_t* init_byte);

