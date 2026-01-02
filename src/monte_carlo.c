//
// Monte Carlo Option Pricing Engine
// This is where the magic happens - we simulate thousands of possible futures
// and average the results to estimate what an option is worth today.
//

#include <math.h>
#include "include/stock.h"
#include "include/option.h"
#include "include/monte_carlo.h"
#include "include/gbm.h"
#include "include/normal.h"


/**
 * Price a European call option using Monte Carlo simulation.
 *
 * The idea is simple but powerful:
 *   1. Simulate many possible stock price paths
 *   2. Calculate the payoff at expiration for each path
 *   3. Average all the payoffs
 *   4. Discount back to today's value
 *
 * Why does this work?
 *   - By the law of large numbers, the average of many simulations
 *     converges to the true expected value
 *   - Risk-neutral pricing tells us the fair price is the discounted
 *     expected payoff under the risk-neutral measure
 *
 * Accuracy: Error decreases as 1/√n_sim (need 4x simulations for 2x accuracy)
 *
 * @param S0      Initial stock price (e.g., $100)
 * @param K       Strike price (the price at which you can buy)
 * @param r       Risk-free interest rate (e.g., 0.05 for 5%)
 * @param sigma   Volatility (e.g., 0.2 for 20% annual volatility)
 * @param T       Time to maturity in years (e.g., 1.0 for one year)
 * @param n_sim   Number of simulations (more = more accurate, but slower)
 * @return        Estimated fair price of the call option
 */
double price_european_call_mc(
    double S0,
    double K,
    double r,
    double sigma,
    double T,
    uint32_t n_sim
){
    double payoff_sum = 0.0;

    // Run n_sim independent simulations
    for (uint32_t i = 0; i < n_sim; i++) {
        // Step 1: Simulate where the stock price ends up at maturity
        // Each call gives us a different random outcome
        double ST = simulate_gbm(S0, r, sigma, T);

        // Step 2: Calculate how much money we'd make with this outcome
        // For a call: max(ST - K, 0) - we profit if stock > strike
        double payoff = call_payoff(ST, K);

        // Step 3: Add to our running total
        payoff_sum += payoff;
    }

    // Step 4: Calculate the average payoff across all simulations
    // This estimates E[payoff] under the risk-neutral measure
    double avg_payoff = payoff_sum / n_sim;

    // Step 5: Discount back to present value
    // A dollar in the future is worth less than a dollar today
    // exp(-r*T) is the discount factor
    double discounted_price = avg_payoff * exp(-r * T);

    return discounted_price;
}

/**
 * Price a European call option using the analytical Black-Scholes formula.
 *
 * The Black-Scholes formula gives the exact theoretical price:
 *   C = S0 * N(d1) - K * e^(-rT) * N(d2)
 *
 * where:
 *   d1 = [ln(S0/K) + (r + σ²/2)T] / (σ√T)
 *   d2 = d1 - σ√T
 *   N(x) = standard normal CDF
 *
 * This is the "gold standard" to compare Monte Carlo results against.
 * If MC and BS differ significantly, there's likely a bug somewhere.
 *
 * @param S0      Initial stock price
 * @param K       Strike price
 * @param r       Risk-free interest rate
 * @param sigma   Volatility
 * @param T       Time to maturity in years
 * @return        Exact Black-Scholes price for European call
 */
double price_european_call_bs(
    double S0,
    double K,
    double r,
    double sigma,
    double T
) {
    // Calculate d1 and d2 (the standardized distances)
    double sqrt_T = sqrt(T);
    double d1 = (log(S0 / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt_T);
    double d2 = d1 - sigma * sqrt_T;

    // Black-Scholes formula:
    // Call = S0 * N(d1) - K * e^(-rT) * N(d2)
    double call_price = S0 * normal_cdf(d1) - K * exp(-r * T) * normal_cdf(d2);

    return call_price;
}