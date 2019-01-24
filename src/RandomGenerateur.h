#ifndef RANDOM_GENERATEUR_H
#define RANDOM_GENERATEUR_H

#include <random>

namespace PM3D
{
    class RandomGenerateur {
        std::random_device rd;
        std::mt19937 prng;

        RandomGenerateur()
            : prng{rd()}
        {}

    public:
        RandomGenerateur(const RandomGenerateur&) = delete;
        RandomGenerateur& operator=(const RandomGenerateur&) = delete;

        static RandomGenerateur& get() {
            static RandomGenerateur singleton;
            return singleton;
        }

        int random(int minInclu, int maxInclu) {
            std::uniform_int_distribution<int> d{ minInclu, maxInclu };
            return d(prng);
        }
        float random(float min, float max) {
            std::uniform_real_distribution<float> d{ min, max };
            return d(prng);
        }
    };
}
#endif // !RANDOM_GENERATEUR_H