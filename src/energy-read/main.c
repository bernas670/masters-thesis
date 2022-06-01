#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#include "rapl-read.h"
#include "../utils/macros.h"
#include "../utils/files.h"
#include "../utils/cpu.h"

// 1st - command to run
// 2nd - program name
// 3rd - compilation flags
int main(int argc, char *argv[]) {

    char *cmd = argv[1];
    char *prog = argv[2];
    char *flag = strrchr(argv[3], ' ') + 1;

    // TODO: generalize this
    // read temps from file
    char* line = file_readline(TEMP_FILE);
    // double temps[2];
    // sscanf(line, "%lf %lf", &temps[0], &temps[1]);
    double temps[1];
    sscanf(line, "%lf", &temps[0]);
    free(line);

    // give the machine time to cooldown
    int cooldown_time = await_cooldown(temps, TEMP_RANGE, COOLDOWN);

    // log compilation
    char *log_str[512];          
    sprintf(log_str, "COOLDOWN ==> %d seconds\n", cooldown_time);
    file_append(LOG_FILE, log_str);

    pkg_arr rapl = rapl_read(cmd);

    // get timestamp
    time_t clk = time(NULL);
    char* timestamp = strtok(ctime(&clk), "\n");

    char* results[2048];
    // timestamp, program, flag, execution time
    sprintf(results, "%s,%s,%s,%f", timestamp, prog, flag, rapl.time);

    // iterate through packages
    for (int i = 0; i < rapl.size; i++) {
        rapl_st *pkg = rapl.data[i];
        // energy, pp0, pp1, dram, psys
        sprintf(results, "%s,%f,%f,%f,%f,%f", results, pkg->pkg_energy, pkg->pp0, pkg->pp1, pkg->dram, pkg->psys);
    }

    sprintf(results, "%s\n", results);

    if (file_append(RES_FILE, results)) {
        printf("==> Error writing to file\n");
    }

    return 0;
}