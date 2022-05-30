#ifndef MY_MACROS_H
#define MY_MACROS_H

#define PROG_FILE "./resources/bench_test.txt"
#define FLAG_FILE "./resources/flags_test.txt"

#define TEMP_FILE "/tera-linux/rapi/thesis/resources/temps.txt"
#define RES_FILE  "/tera-linux/rapi/thesis/results/res_05.csv"
#define LOG_FILE  "/tera-linux/rapi/thesis/logs/log_05.txt"


// number of runs
// const int NUM_RUNS = 1;
#define NUM_RUNS 10

// number of cpus in the system
// const int NUM_CPUS = 2;
#define NUM_CPUS 2

// temperature range
// const double TEMP_RANGE = 2.0f;
#define TEMP_RANGE 0.0f
// time for system to cooldown (seconds)
// const int COOLDOWN = 5;
#define COOLDOWN 5

// number of samples to calculate temperature average
#define NUM_BASELINE 10

#endif