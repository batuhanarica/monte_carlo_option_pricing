//
// Monte Carlo Option Pricing Header
//

#ifndef MONTE_CARLO_OPTION_PRICING_MC_H
#define MONTE_CARLO_OPTION_PRICING_MC_H

#include <stdint.h>

// Monte Carlo pricing for European call option
double price_european_call_mc(
    double S0,
    double K,
    double r,
    double sigma,
    double T,
    uint32_t n_sim
);

// Analytical Black-Scholes price for European call option
double price_european_call_bs(
    double S0,
    double K,
    double r,
    double sigma,
    double T
);

#endif //MONTE_CARLO_OPTION_PRICING_MC_H