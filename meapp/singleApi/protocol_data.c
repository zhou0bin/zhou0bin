
#include "protocol_data.h"

float ProtocolData_BoundedValue(float lower, float upper, float val) 
{
    if (lower > upper) {
        return val;
    }
    if (val < lower) {
        return lower;
    }
    if (val > upper) {
        return upper;
    }

    return val;
}

uint8_t ProtocolData_CalculateCheckSum(const uint8_t *input, const uint32_t length) 
{
    return 0xFF;
}
