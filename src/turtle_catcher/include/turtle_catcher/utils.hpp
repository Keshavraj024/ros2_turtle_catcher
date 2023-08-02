#ifndef TURTLE_CATCHER__UTILS_HPP_
#define TURTLE_CATCHER__UTILS_HPP_
#include <random>

namespace turtle_catcher
{
    template <typename T>
    T GenerateRandomNumber(T min, T max)
    {
        // Seed the random number generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // Define the distribution for generating numbers between min and max
        std::uniform_real_distribution<T> distribution(min, max);

        // Generate a random number between min and max
        T randomNum = distribution(gen);

        return randomNum;
    }

}

#endif /* TURTLE_CATCHER__UTILS_HPP_ */