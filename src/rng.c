//
// Random Number Generator (RNG) module
// Provides uniform and normal random numbers for Monte Carlo simulations.
// Uses Xorshift32 for speed and Box-Muller for normal distribution.
//
// Created by b2 on 12/28/25.
//

#include <stdint.h>
#include <math.h>
#include "include/rng.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global RNG state - this single number holds the "position" in our random sequence
// Static means it's private to this file (encapsulation)
static uint32_t rng_state;

/**
 * Initialize the random number generator with a seed.
 *
 * The seed determines the entire sequence of random numbers.
 * Same seed = same sequence (useful for reproducible simulations).
 * Different seeds = different sequences.
 *
 * @param seed  Starting value for RNG (use time(NULL) for "random" seed)
 */
void rng_seed(uint32_t seed) {
    // Xorshift doesn't work with zero (would output zeros forever)
    // So we force any zero seed to become 1
    if (seed == 0) {
        seed = 1;
    }
    rng_state = seed;
}

/**
 * Xorshift32 - a fast, simple pseudo-random number generator.
 *
 * How it works:
 *   1. XOR the state with itself shifted left 13 bits
 *   2. XOR with itself shifted right 17 bits  
 *   3. XOR with itself shifted left 5 bits
 *
 * These specific shift amounts (13, 17, 5) were chosen by George Marsaglia
 * to maximize the period (how long until the sequence repeats).
 * Period = 2^32 - 1 (about 4 billion numbers before repeating)
 *
 * Why Xorshift?
 *   - Blazing fast (just 3 XORs and 3 shifts)
 *   - Good statistical properties for Monte Carlo
 *   - No division or multiplication needed
 *
 * Reference: https://en.wikipedia.org/wiki/Xorshift
 *
 * @param state  Pointer to the RNG state (modified in place)
 * @return       Next pseudo-random 32-bit unsigned integer
 */
static inline uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;  // First transformation
    x ^= x >> 17;  // Second transformation
    x ^= x << 5;   // Third transformation
    *state = x;    // Save new state for next call
    return x;
}

/**
 * Generate a uniform random double in [0.0, 1.0).
 *
 * Takes a 32-bit integer from xorshift32 and scales it to [0, 1).
 * This is the building block for all other random distributions.
 *
 * @return  Random double uniformly distributed in [0.0, 1.0)
 */
double random_double() {
    // Divide by UINT32_MAX to scale from [0, 2^32-1] to [0, 1)
    return (double)xorshift32(&rng_state) / (double)UINT32_MAX;
}

/**
 * Generate a standard normal random variable using Box-Muller transform.
 *
 * The Box-Muller transform converts two uniform random numbers into
 * a normally distributed random number with mean=0 and std=1.
 *
 * How it works (geometrically):
 *   1. Take two uniform randoms U1, U2 in (0,1)
 *   2. sqrt(-2*ln(U1)) gives us a radius (Rayleigh distributed)
 *   3. 2*π*U2 gives us an angle (uniform on circle)
 *   4. Together they form a 2D Gaussian; we use the x-coordinate
 *
 * Formula: Z = sqrt(-2*ln(U1)) * cos(2*π*U2)
 *
 * Why we need this:
 *   - Stock prices in GBM require normal random shocks
 *   - We only have uniform randoms, so we must transform them
 *
 * Reference: https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
 *
 * @return  Random double from standard normal distribution N(0,1)
 */
double normal_random(void) {
    double u1 = random_double();  // First uniform random
    double u2 = random_double();  // Second uniform random

    // Box-Muller formula:
    // - sqrt(-2*ln(u1)) = radius (distance from origin)
    // - cos(2*π*u2) = x-coordinate on unit circle
    // - Product gives us one standard normal sample
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}