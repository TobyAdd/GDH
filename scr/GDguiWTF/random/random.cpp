#pragma once
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <random>
#include "random.h"


namespace Random {
    void random_all() {
        srand(time(NULL));
    }

    float random_float(float a, float b) {
        float random = ((float)rand()) / (float)RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }

    int random_int(int a, int b) {
        float random = ((float)rand()) / (float)RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return (int)(a + r);
    }

    bool random_chance(int percentage) {
        return random_int(1, 100) <= percentage;
    }
}
