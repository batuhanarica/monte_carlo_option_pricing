//
// Option payoff functions for European options
// These calculate how much an option is worth at expiration
//

#include "include/option.h"

/**
 * Calculate the payoff of a European call option at expiration.
 *
 * A call option gives you the right to BUY a stock at the strike price K.
 * It's only worth something if the stock price S ends up ABOVE K.
 *
 * Payoff = max(S - K, 0)
 *
 * Example: Strike K = 100, Stock ends at S = 115
 *          Payoff = 115 - 100 = $15 (you buy at 100, sell at 115)
 *
 * @param S  Final stock price at expiration
 * @param K  Strike price (the agreed purchase price)
 * @return   Payoff amount (always >= 0)
 */
double call_payoff(double S, double K)
{
    // If stock price > strike: profit is the difference
    // Otherwise: option expires worthless (you wouldn't exercise)
    return (S > K) ? (S - K) : 0.0;
}

/**
 * Calculate the payoff of a European put option at expiration.
 *
 * A put option gives you the right to SELL a stock at the strike price K.
 * It's only worth something if the stock price S ends up BELOW K.
 *
 * Payoff = max(K - S, 0)
 *
 * Example: Strike K = 100, Stock ends at S = 85
 *          Payoff = 100 - 85 = $15 (you buy at 85, sell at 100)
 *
 * @param S  Final stock price at expiration
 * @param K  Strike price (the agreed sale price)
 * @return   Payoff amount (always >= 0)
 */
double put_payoff(double S, double K)
{
    // If strike > stock price: profit is the difference
    // Otherwise: option expires worthless (you wouldn't exercise)
    return (K > S) ? (K - S) : 0.0;
}
