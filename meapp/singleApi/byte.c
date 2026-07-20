#include "byte.h"
#include <stddef.h>

#define BYTE_LENGTH       (8u)

const uint8_t RANG_MASK_1_L[] = {0x01, 0x03, 0x07, 0x0F,
                                 0x1F, 0x3F, 0x7F, 0xFF};
const uint8_t RANG_MASK_0_L[] = {0xFE, 0XFC, 0xF8, 0xF0,
                                 0xE0, 0xC0, 0x80, 0x00};
                                 
static int32_t min(int32_t value1, int32_t value2)
{
    if(value1 > value2)
    {
        return value2;
    }
    return value1;
}

uint8_t Byte_get_byte(const int32_t start_pos, const int32_t length, const uint8_t* init_byte)  
{
    if (start_pos > BYTE_LENGTH - 1 || start_pos < 0 || length < 1 || init_byte == NULL) 
    {
        return 0x00;
    }

    int32_t end_pos = min(start_pos + length - 1, BYTE_LENGTH - 1);
    int32_t real_len = end_pos + 1 - start_pos;
    uint8_t result = (*init_byte) >> start_pos;
    result &= RANG_MASK_1_L[real_len - 1];

    return result;
}

void Byte_set_value(const uint8_t value, const int32_t start_pos,
                     const int32_t length, uint8_t* targat_byte) 
{
    if (start_pos > BYTE_LENGTH - 1 || start_pos < 0 || length < 1 || targat_byte == NULL) 
    {
        return;
    }

    int32_t end_pos = min(start_pos + length - 1, BYTE_LENGTH - 1);
    int32_t real_len = end_pos + 1 - start_pos;
    uint8_t current_value_low = 0x00;

    if (start_pos > 0) 
    {
        current_value_low = *targat_byte & RANG_MASK_1_L[start_pos - 1];
    }

    uint8_t current_value_high = *targat_byte & RANG_MASK_0_L[end_pos];
    uint8_t middle_value = value & RANG_MASK_1_L[real_len - 1];
    middle_value = (uint8_t)(middle_value << start_pos);
    *targat_byte = (uint8_t)(current_value_high + middle_value + current_value_low);
}