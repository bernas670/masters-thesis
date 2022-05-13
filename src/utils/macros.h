#ifndef MY_MACROS_H
#define MY_MACROS_H


// const char* PROG_FILE = "./resources/bench_test.txt";
// const char* FLAG_FILE = "./resources/flags_test.txt";

#define PROG_FILE "./resources/bench_test.txt"
#define FLAG_FILE "./resources/flags_test.txt"


// const char* RES_FILE = "/tera-linux/rapi/thesis/results/res_02.csv";
// const char* LOG_FILE = "/tera-linux/rapi/thesis/logs/log_02.txt";

#define RES_FILE  "/tera-linux/rapi/thesis/results/res_04.csv"
#define LOG_FILE  "/tera-linux/rapi/thesis/logs/log_04.txt"


// number of runs
// const int NUM_RUNS = 1;
#define NUM_RUNS 3

// number of cpus in the system
// const int NUM_CPUS = 2;
#define NUM_CPUS 2

// base system temperatures
// const double BASE_TEMPS[2] = { 40.0f, 40.0f };
static double* BASE_TEMPS;
// #define BASE_TEMPS (double[]){40.0f, 40.0f}

// temperature range
// const double TEMP_RANGE = 2.0f;
#define TEMP_RANGE 2.0f
// time for system to cooldown (seconds)
// const int COOLDOWN = 5;
#define COOLDOWN 5

#endif