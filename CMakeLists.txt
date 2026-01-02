cmake_minimum_required(VERSION 4.1)
project(monte_carlo_option_pricing C)

set(CMAKE_C_STANDARD 99)

add_executable(monte_carlo_option_pricing main.c
        include/stock.h
        src/main.c
        include/rng.h
        src/rng.c
        include/normal.h
        src/normal.c)
