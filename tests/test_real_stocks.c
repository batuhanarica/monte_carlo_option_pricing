//
// Real Stock Option Pricing Tests
// Tests Monte Carlo pricing against real market option data
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "include/rng.h"
#include "include/monte_carlo.h"

// Global seed - can be fixed (reproducible) or time-based (random)
static uint32_t g_seed = 42u;
static int g_use_random_seed = 0;

#define MAX_LINE_LENGTH 256
#define MAX_TICKER_LENGTH 10

/**
 * Structure to hold option test data from CSV
 */
typedef struct {
    char ticker[MAX_TICKER_LENGTH];
    double S0;              // Current stock price
    double K;               // Strike price
    double r;               // Risk-free rate
    double sigma;           // Volatility (annualized)
    int days_to_expiry;     // Days until expiration
    double market_price;    // Actual market price (if available)
} OptionData;

/**
 * Parse a single line of CSV data into OptionData struct
 * Returns 1 on success, 0 on failure (comment or invalid line)
 */
int parse_csv_line(const char *line, OptionData *opt) {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') {
        return 0;
    }
    
    // Parse CSV: ticker,S0,K,r,sigma,days,market_price
    int parsed = sscanf(line, "%9[^,],%lf,%lf,%lf,%lf,%d,%lf",
                        opt->ticker, &opt->S0, &opt->K, &opt->r,
                        &opt->sigma, &opt->days_to_expiry, &opt->market_price);
    
    return (parsed == 7) ? 1 : 0;
}

/**
 * Convert days to expiry to years (trading days = 252 per year)
 */
double days_to_years(int days) {
    return (double)days / 365.0;  // Calendar days
}

/**
 * Run pricing test for a single option
 * Returns: error percentage (MC vs BS)
 */
double test_option(const OptionData *opt, uint32_t n_sim, int test_num) {
    double T = days_to_years(opt->days_to_expiry);
    
    // Use different seed per test for independence
    // But deterministic if using fixed base seed
    rng_seed(g_seed + test_num);
    
    // Price using Monte Carlo
    double mc_price = price_european_call_mc(opt->S0, opt->K, opt->r, opt->sigma, T, n_sim);
    
    // Price using Black-Scholes
    double bs_price = price_european_call_bs(opt->S0, opt->K, opt->r, opt->sigma, T);
    
    // Calculate errors
    double mc_bs_error = ((mc_price - bs_price) / bs_price) * 100.0;
    
    // Compare to market price if available
    char market_comparison[50] = "N/A";
    if (opt->market_price > 0.01) {
        double market_error = ((mc_price - opt->market_price) / opt->market_price) * 100.0;
        snprintf(market_comparison, sizeof(market_comparison), "$%.2f (%+.1f%%)", 
                 opt->market_price, market_error);
    }
    
    // Determine option type (ITM/ATM/OTM)
    const char *moneyness;
    double ratio = opt->S0 / opt->K;
    if (ratio > 1.02) moneyness = "ITM";
    else if (ratio < 0.98) moneyness = "OTM";
    else moneyness = "ATM";
    
    printf("| %-5s | %3s | $%7.2f | $%7.2f | %5.1f%% | %3dd | $%7.2f | $%7.2f | %+6.2f%% | %-18s |\n",
           opt->ticker, moneyness, opt->S0, opt->K, opt->sigma * 100,
           opt->days_to_expiry, mc_price, bs_price, mc_bs_error, market_comparison);
    
    return mc_bs_error;
}

/**
 * Print table header
 */
void print_header(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                              REAL STOCK OPTION PRICING TEST                                                       ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("| %-5s | %-3s | %8s | %8s | %6s | %4s | %8s | %8s | %7s | %-18s |\n",
           "Stock", "M", "Price", "Strike", "Vol", "Exp", "MC", "BS", "MC-BS", "Market (error)");
    printf("|-------|-----|----------|----------|--------|------|----------|----------|---------|--------------------|\n");
}

/**
 * Print table footer with summary
 */
void print_footer(int total, int within_1pct, double avg_error) {
    printf("╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ SUMMARY: %d options tested | MC within 1%% of BS: %d/%d (%.1f%%) | Avg MC-BS error: %.2f%%                           ║\n",
           total, within_1pct, total, (double)within_1pct/total*100, avg_error);
    printf("╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}

int main(int argc, char *argv[]) {
    // Default CSV file path
    const char *csv_file = "tests/real_stocks.csv";
    uint32_t n_sim = 500000;  // Simulations per option
    int positional_arg = 0;   // Track which positional argument we're on
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--random") == 0 || strcmp(argv[i], "-r") == 0) {
            g_use_random_seed = 1;
            g_seed = (uint32_t)time(NULL);
        } else if (strcmp(argv[i], "--seed") == 0 || strcmp(argv[i], "-s") == 0) {
            if (i + 1 < argc) {
                g_seed = (uint32_t)atoi(argv[++i]);
            }
        } else if (argv[i][0] != '-') {
            // Positional arguments: csv_file, then n_sim
            if (positional_arg == 0) {
                csv_file = argv[i];
                positional_arg++;
            } else if (positional_arg == 1) {
                n_sim = (uint32_t)atoi(argv[i]);
                positional_arg++;
            }
        }
    }
    
    // Open CSV file
    FILE *fp = fopen(csv_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", csv_file);
        fprintf(stderr, "Usage: %s [csv_file] [n_simulations] [--random|-r] [--seed|-s N]\n", argv[0]);
        fprintf(stderr, "  --random, -r    Use time-based random seed (different results each run)\n");
        fprintf(stderr, "  --seed N, -s N  Use specific seed N\n");
        return 1;
    }
    
    printf("Loading options from: %s\n", csv_file);
    printf("Simulations per option: %u\n", n_sim);
    printf("Seed: %u%s\n", g_seed, g_use_random_seed ? " (random)" : " (fixed)");
    
    print_header();
    
    char line[MAX_LINE_LENGTH];
    OptionData opt;
    int total = 0;
    int within_1pct = 0;
    double total_error = 0.0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (parse_csv_line(line, &opt)) {
            double err = test_option(&opt, n_sim, total);
            
            total++;
            if (fabs(err) < 1.0) within_1pct++;
            total_error += fabs(err);
        }
    }
    
    fclose(fp);
    
    if (total > 0) {
        print_footer(total, within_1pct, total_error / total);
    } else {
        printf("No valid option data found in file.\n");
    }
    
    return 0;
}
