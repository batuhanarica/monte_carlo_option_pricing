//
// Geometric Brownian Motion (GBM) simulation for stock prices
// This is the heart of Monte Carlo option pricing - it models how stock prices
// move randomly over time according to the Black-Scholes assumptions.
//

#include "include/stock.h"
#include "include/rng.h"
#include <math.h>

/**
 * Simulate a stock price at maturity using Geometric Brownian Motion (GBM).
 *
 * GBM models stock prices as: dS = μS dt + σS dW
 * where dW is a Wiener process (random walk).
 *
 * The closed-form solution gives us the price at time T:
 *   S(T) = S0 * exp((r - σ²/2)*T + σ*√T*Z)
 *
 * Why (r - σ²/2) instead of just r?
 *   - This is the "drift correction" or Itô's lemma adjustment
 *   - Without it, the expected stock price would be wrong
 *   - The -σ²/2 term corrects for the fact that log-returns are used
 *
 * Reference: https://en.wikipedia.org/wiki/Geometric_Brownian_motion
 *
 * @param S0     Initial stock price (e.g., $100)
 * @param r      Risk-free interest rate (e.g., 0.05 for 5%)
 * @param sigma  Volatility (standard deviation of returns, e.g., 0.2 for 20%)
 * @param T      Time to maturity in years (e.g., 1.0 for one year)
 * @return       Simulated stock price at maturity
 */
double simulate_gbm(double S0, double r, double sigma, double T) {
    // Generate a standard normal random variable Z ~ N(0,1)
    // This represents the random "shock" to the stock price
    double Z = normal_random();

    // Apply the GBM formula:
    // - (r - 0.5*σ²)*T is the deterministic drift (adjusted for log-normal distribution)
    // - σ*√T*Z is the random component (volatility scaled by time and randomness)
    // - exp() converts from log-space back to price space
    return S0 * exp((r - 0.5 * sigma * sigma) * T + sigma * sqrt(T) * Z);
}

