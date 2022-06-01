#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "../utils/macros.h"
#include "../utils/files.h"
#include "../utils/cpu.h"
#include "ghc.h"


int main(int argc, char *argv[])
{
    // get flags
    str_arr flags;
    if ((flags = file2arr(FLAG_FILE)).size == -1)
    {
        printf("Could not read flags file: %s", FLAG_FILE);
        return 1;
    }

    FILE *programs_fp;
    char *program = NULL;
    size_t len = 0;
    ssize_t read;

    if ((programs_fp = fopen(PROG_FILE, "r")) == NULL)
    {
        return 1;
    }

    // calculate temperature baseline
    double *base_temps = get_baseline(NUM_BASELINE, COOLDOWN);
    char *log_str[512];

    // write temps to file
    // sprintf(log_str, "%f %f", base_temps[0], base_temps[1]);
    sprintf(log_str, "%f", base_temps[0]);
    file_write(TEMP_FILE, log_str);

    // log temperatures   
    // TODO: generalize this
    // sprintf(log_str, "TEMPS ==> CPU1: %f CPU2: %f\n", base_temps[0], base_temps[1]);
    sprintf(log_str, "TEMPS ==> CPU: %f\n", base_temps[0]);
    file_append(LOG_FILE, log_str);

    // for each program
    while ((read = getline(&program, &len, programs_fp)) != -1)
    {
        if (program[strlen(program) - 1] == '\n')
            program[strlen(program) - 1] = 0;

        for (size_t i = 0; i < flags.size; i++)
        {
            int compile_st = compile(program, flags.data[i], NUM_RUNS);

            // log compilation
            sprintf(log_str, "%s ==> program: %s | flag: %s\n", compile_st == 0 ? "OK" : "ERROR", program, flags.data[i]);
            file_append(LOG_FILE, log_str);
        }
    }

    if (program) free(program);
    fclose(programs_fp);

    // clean memory used to store flags
    for (int i = 0; i < flags.size; i++)
        free(flags.data[i]);
    free(flags.data);

    return 0;
}