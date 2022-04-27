#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "rapl-read.h"
#include "../utils/files.h"

// 1st - command to run
// 2nd - program name
// 3rd - compilation flags
int main(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    char *cmd = argv[1];
    char *prog = argv[2];
    char *flag = argv[3];

    pkg_arr rapl = rapl_read(cmd);

    // TODO: process flag string



    // get timestamp
    time_t clk = time(NULL);
    char* timestamp = strtok(ctime(&clk), "\n");

    char* results[2048];
    // timestamp, program, flag, execution time, pkg energy, pp0, pp1, dram, psys, ...
    sprintf(results, "%s,%s,%s,%f", timestamp, prog, flag, rapl.time);

    // iterate through packages
    for (int i = 0; i < rapl.size; i++) {
        rapl_st *pkg = rapl.data[i];
        // energy, pp0, pp1, dram, psys
        sprintf(results, "%s,%f,%f,%f,%f,%f", results, pkg->pkg_energy, pkg->pp0, pkg->pp1, pkg->dram, pkg->psys);
    }

    sprintf(results, "%s\n", results);

    if (file_append("/tera-linux/rapi/thesis/results.csv", results)) {
        printf("==> Error writing to file\n");
    }

    // TODO: give the machine time to cooldown
    // sleep(120);

    return 0;
}