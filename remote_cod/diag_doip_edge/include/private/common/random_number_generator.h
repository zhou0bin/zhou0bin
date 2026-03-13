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

/// @file
/// @brief A simple Random Value Generator based on <random>

#ifndef ASF_DIAG_PROXY_INCLUDE_COMMON_RANDOM_NUMBER_GENERATOR_H_
#define ASF_DIAG_PROXY_INCLUDE_COMMON_RANDOM_NUMBER_GENERATOR_H_

#include <random>

namespace asf
{
namespace diag
{
namespace common
{
/// @brief A simple Random Number Generator based on the c++ random header
class RandomNumberGenerator
{
private:
    /// @brief The seed for the random engine
    std::random_device seed_;

    /// @brief The random engine used by the distribution
    std::default_random_engine engine_;

    /// @brief The distribution (uniform for now) of random values
    std::uniform_int_distribution<int> distribution_;

public:
    /// @brief Instantiates a random number generator that generates int numbers from [lowerBound,upperBound].
    /// It uses uniform distribution and the default random engine of the c++ random header
    ///
    /// @param lowerBound the lower bound of the closed interval from which random numbers shall be generated
    /// @param upperBound the upper bound of the closed interval from which random numbers shall be generated
    RandomNumberGenerator(int lowerBound, int upperBound);

    /// @brief Get the next random number from the generator
    /// @returns the next random number from the generator
    int get();
};

}  // namespace common
}  // namespace diag
}  // namespace asf

#endif  // ASF_DIAG_PROXY_INCLUDE_COMMON_RANDOM_NUMBER_GENERATOR_H_
