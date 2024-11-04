#ifndef RANDOM_NUMBER_GENERATOR_H
#define RANDOM_NUMBER_GENERATOR_H

#include <random>

class RandomNumberGenerator
{
    public:
        // Constructor
        RandomNumberGenerator() : gen(rd())
        {
        }

        // Generates a random number between 0 and the max
        int generate(int maximum)
        {
            std::uniform_int_distribution<> dis(0, maximum);
            return dis(gen);
        }

    private:
        std::random_device rd;          // Seed
        std::mt19937 gen;               // Mersenne Twister engine
};

#endif // RANDOM_NUMBER_GENERATOR_H