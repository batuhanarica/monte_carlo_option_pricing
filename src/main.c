#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "include/rng.h"
#include "include/monte_carlo.h"


//
// Monte Carlo Option Pricing - Main Entry Point
//
// This program prices European options using Monte Carlo simulation.
// It simulates thousands of possible stock price paths and averages
// the payoffs to estimate the option's fair value.

/**
 * Main entry point for the Monte Carlo option pricer.
 *
 * Typical workflow:
 *   1. Set up option parameters (strike, maturity, etc.)
 *   2. Seed the random number generator
 *   3. Run N simulations of stock price using GBM
 *   4. Calculate payoff for each simulation
 *   5. Average payoffs and discount to present value
 *   6. Report the estimated option price
 *
 * @return  0 on success, non-zero on error
 */
int main(void) {
    rng_seed(123456u);  // Fixed seed for reproducibility

    // Example parameters for a European call option
    double S0 = 100.0;      // Initial stock price
    double K = 100.0;       // Strike price
    double r = 0.05;        // Risk-free interest rate (5%)
    double sigma = 0.2;     // Volatility (20%)
    double T = 1.0;         // Time to maturity (1 year)
    uint32_t n_sim = 1000000; // Number of Monte Carlo simulations

    // Price using Monte Carlo simulation
    double mc_price = price_european_call_mc(S0, K, r, sigma, T, n_sim);
    
    // Price using analytical Black-Scholes formula
    double bs_price = price_european_call_bs(S0, K, r, sigma, T);
    
    // Calculate the error
    double error = mc_price - bs_price;
    double error_pct = (error / bs_price) * 100.0;

    // Print comparison
    printf("=== European Call Option Pricing ===\n");
    printf("Parameters:\n");
    printf("  S0 (Initial Price): $%.2f\n", S0);
    printf("  K  (Strike Price):  $%.2f\n", K);
    printf("  r  (Risk-free Rate): %.2f%%\n", r * 100);
    printf("  σ  (Volatility):     %.2f%%\n", sigma * 100);
    printf("  T  (Time to Expiry): %.2f years\n", T);
    printf("  Simulations:         %u\n\n", n_sim);
    
    printf("Results:\n");
    printf("  Monte Carlo Price:   $%.4f\n", mc_price);
    printf("  Black-Scholes Price: $%.4f\n", bs_price);
    printf("  Error:               $%.4f (%.2f%%)\n", error, error_pct);
    
    // Check if results are reasonable (within 1% is good for 1M simulations)
    if (fabs(error_pct) < 1.0) {
        printf("\nMonte Carlo result is within 1%% of Black-Scholes\n");
    } else {
        printf("\nWARNING: Large discrepancy detected! Check for bugs.\n");
    }

    return 0;
}
