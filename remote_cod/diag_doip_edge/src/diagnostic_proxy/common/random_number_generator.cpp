// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#include "common/random_number_generator.h"

namespace asf
{
namespace diag
{
namespace common
{

RandomNumberGenerator::RandomNumberGenerator(int lowerBound, int upperBound)
    : seed_{}
    , engine_{seed_()}
    , distribution_{lowerBound, upperBound}
{
}

int RandomNumberGenerator::get()
{
    return distribution_(engine_);
}

}  // namespace common
}  // namespace diag
}  // namespace asf
