#pragma once
#include <stdint.h>

float ProtocolData_BoundedValue(float lower, float upper, float val);
uint8_t ProtocolData_CalculateCheckSum(const uint8_t *input, const uint32_t length);

