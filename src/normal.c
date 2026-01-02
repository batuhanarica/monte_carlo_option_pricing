//
// Normal Distribution Utilities
// Provides the cumulative distribution function (CDF) for the standard normal
// distribution, needed for analytical Black-Scholes pricing.
//
// Created by b2 on 12/28/25.
//

#include <math.h>
#include "include/normal.h"

/**
 * Approximate the standard normal cumulative distribution function (CDF).
 *
 * Uses the error function (erf) which is available in C99's math.h.
 * This is the most accurate and simplest approach.
 *
 * N(x) = (1 + erf(x/√2)) / 2
 *
 * @param x  The value to evaluate the CDF at
 * @return   Probability that a standard normal variable is ≤ x
 */
double normal_cdf(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}