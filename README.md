# Monte Carlo Option Pricing

A fast, educational implementation of Monte Carlo simulation for pricing European options, written in C.

## What is this?

This project prices European call options using two methods:

1. **Monte Carlo Simulation** - Simulates thousands of possible stock price paths and averages the payoffs
2. **Black-Scholes Formula** - The analytical solution for comparison

The Monte Carlo approach is particularly powerful because it can be extended to price exotic options where no closed-form solution exists.

## How it works

### The Big Picture

Imagine you want to know what a call option is worth today. The problem is: you don't know where the stock price will be at expiration. It could go up, down, or sideways.

Monte Carlo solves this by:
1. Simulating many possible futures (e.g., 1 million random stock price paths)
2. Calculating the payoff for each scenario
3. Averaging all the payoffs
4. Discounting back to today's value

By the law of large numbers, this average converges to the true expected value.

### The Math

Stock prices are modeled using **Geometric Brownian Motion (GBM)**:

```
S(T) = S₀ × exp((r - σ²/2)T + σ√T × Z)
```

Where:
- `S₀` = Initial stock price
- `r` = Risk-free interest rate
- `σ` = Volatility (annualized)
- `T` = Time to expiration (years)
- `Z` = Standard normal random variable

The option price is then:

```
Price = e^(-rT) × Average(max(S(T) - K, 0))
```

## Project Structure

```
monte_carlo_option_pricing/
├── src/
│   ├── main.c           # Entry point and example usage
│   ├── monte_carlo.c    # MC simulation & Black-Scholes pricing
│   ├── gbm.c            # Geometric Brownian Motion simulation
│   ├── rng.c            # Random number generation (Xorshift + Box-Muller)
│   ├── option.c         # Payoff functions (call/put)
│   └── normal.c         # Normal distribution CDF
├── include/
│   ├── monte_carlo.h
│   ├── gbm.h
│   ├── rng.h
│   ├── option.h
│   ├── normal.h
│   └── stock.h
├── tests/
│   ├── test_real_stocks.c   # Test suite with real stock data
│   └── real_stocks.csv      # Sample option data (AAPL, TSLA, etc.)
├── Makefile
└── README.md
```

## Building

### Requirements
- GCC (or any C11-compatible compiler)
- Make
- Linux/macOS (should work on Windows with MinGW)

### Compile

```bash
make            # Build the main program
make test       # Build and run tests
make clean      # Remove build artifacts
```

### Run

```bash
./monte_carlo_option_pricing
```

Example output:
```
=== European Call Option Pricing ===
Parameters:
  S0 (Initial Price): $100.00
  K  (Strike Price):  $100.00
  r  (Risk-free Rate): 5.00%
  σ  (Volatility):     20.00%
  T  (Time to Expiry): 1.00 years
  Simulations:         1000000

Results:
  Monte Carlo Price:   $10.4942
  Black-Scholes Price: $10.4847
  Error:               $0.0095 (0.09%)
```

## Testing with Real Stocks

The project includes a test suite that prices options on real stocks:

```bash
make test           # Run with 500k simulations (fixed seed)
make test-random    # Run with random seed (different results each time)
make test-fast      # Run with 100k simulations (faster)
make test-accurate  # Run with 2M simulations (more precise)
```

Example test output:
```
╔═══════════════════════════════════════════════════════════════════════════════════════╗
║                              REAL STOCK OPTION PRICING TEST                           ║
╠═══════════════════════════════════════════════════════════════════════════════════════╣
| Stock | M   |    Price |   Strike |    Vol |  Exp |       MC |       BS |   MC-BS    |
|-------|-----|----------|----------|--------|------|----------|----------|------------|
| AAPL  | ATM | $ 185.50 | $ 185.00 |  22.0% |  30d | $   5.27 | $   5.27 |    +0.07%  |
| TSLA  | ATM | $ 248.50 | $ 250.00 |  55.0% |  30d | $  15.36 | $  15.35 |    +0.07%  |
| NVDA  | ATM | $ 480.50 | $ 480.00 |  45.0% |  30d | $  25.83 | $  25.81 |    +0.07%  |
╠═══════════════════════════════════════════════════════════════════════════════════════╣
║ SUMMARY: 28 options tested | MC within 1% of BS: 28/28 (100.0%)                       ║
╚═══════════════════════════════════════════════════════════════════════════════════════╝
```

### Adding Your Own Test Data

Edit `tests/real_stocks.csv`:
```csv
# ticker,stock_price,strike,risk_free_rate,volatility,days_to_expiry,market_price
AAPL,185.50,185.00,0.045,0.22,30,6.50
TSLA,248.50,250.00,0.045,0.55,30,18.90
```

## Key Components

### Random Number Generation (`rng.c`)

Uses **Xorshift32** for fast uniform random numbers, then **Box-Muller transform** to convert to normal distribution:

```c
// Box-Muller: converts uniform [0,1) to standard normal N(0,1)
Z = sqrt(-2 × ln(U₁)) × cos(2π × U₂)
```

### Why Xorshift?
- Blazing fast (just XOR and bit shifts)
- Period of 2³² - 1 (4 billion numbers before repeating)
- Good enough for Monte Carlo (not cryptographic)

### Black-Scholes (`monte_carlo.c`)

The analytical solution for European options:

```c
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
Call = S₀ × N(d₁) - K × e^(-rT) × N(d₂)
```

Where `N(x)` is the standard normal CDF.

## Performance

With 1 million simulations:
- **Accuracy**: ~0.1% error vs Black-Scholes
- **Speed**: ~50ms on modern hardware
- **Memory**: O(1) - no paths stored, just running sum

Accuracy scales as `1/√n` — need 4× more simulations for 2× better accuracy.

## Limitations

- **European options only** - No early exercise (American options would need different approach)
- **Constant volatility** - Real markets have volatility smile/skew
- **No dividends** - Current implementation assumes no dividend payments
- **Single asset** - No correlation modeling for multi-asset options


## References

- [Geometric Brownian Motion](https://en.wikipedia.org/wiki/Geometric_Brownian_motion)
- [Black-Scholes Model](https://en.wikipedia.org/wiki/Black%E2%80%93Scholes_model)
- [Box-Muller Transform](https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform)
- [Xorshift RNG](https://en.wikipedia.org/wiki/Xorshift)

## License

MIT License - feel free to use this for learning or as a starting point for your own projects.

---

*Built as a learning project to understand quantitative finance and Monte Carlo methods.*
