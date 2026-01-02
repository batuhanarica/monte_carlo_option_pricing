//
// Created by b2 on 12/28/25.
//

#ifndef MONTE_CARLO_OPTION_PRICING_STOCK_H
#define MONTE_CARLO_OPTION_PRICING_STOCK_H

typedef struct {
    double initial_price; //initial stock price: S0
    double interest_rate; //risk free interest rate: r
    double volatility;//sigma
    double maturity;//T
}stock_params;

#endif //MONTE_CARLO_OPTION_PRICING_STOCK_H