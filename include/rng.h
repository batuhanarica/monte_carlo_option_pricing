//
// Created by b2 on 12/28/25.
//
//Random Number Generator

#include <stdint.h>

#ifndef MONTE_CARLO_OPTION_PRICING_RNG_H
#define MONTE_CARLO_OPTION_PRICING_RNG_H

void rng_seed(uint32_t seed);
double rng_lcg(void);
double normal_random(void);

#endif //MONTE_CARLO_OPTION_PRICING_RNG_H