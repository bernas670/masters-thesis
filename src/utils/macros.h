#ifndef MY_MACROS_H
#define MY_MACROS_H

#define PROG_FILE "./resources/bench_test.txt"
#define FLAG_FILE "./resources/flags_test.txt"

#define TEMP_FILE "/home/bernas/thesis/resources/temps.txt"
#define RES_FILE  "/home/bernas/thesis/results/res_06.csv"
#define LOG_FILE  "/home/bernas/thesis/logs/log_06.txt"


// number of times each program is executed
#define NUM_RUNS 10

// number of cpus in the system
#define NUM_CPUS 1

// time for system to cooldown (seconds)
#define COOLDOWN 2
// number of consecutive times to consider cooldown
#define NUM_COOLDOWN 3
// temperature range
#define TEMP_RANGE 1.0f

// number of samples to calculate temperature average
#define NUM_BASELINE 10
// interval to collect samples for baseline temperature
#define INT_BASELINE 5

#endif