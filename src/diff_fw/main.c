#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <time.h>

#include "../utils/files.h"
#include "ghc.h"
#include "corediff.h"

// 1st arg - file with program paths
// 2nd arg - file with optimization flags
int main(int argc, char *argv[])
{

    // TODO: handle arguments

    // get flags
    str_arr flags;
    if ((flags = file2arr(argv[2])).size == -1)
    {
        printf("Could not read flags file: %s", argv[1]);
        return 1;
    }

    FILE *programs_fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((programs_fp = fopen(argv[1], "r")) == NULL)
    {
        return 1;
    }

    // for each program
    while ((read = getline(&line, &len, programs_fp)) != -1)
    {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = 0;
        
        char* base_path = compile(line, "-O0");

        for (size_t i = 0; i < flags.size; i++)
        {
            char* opt_path = compile(line, flags.data[i]);
            
            corediff_sim sim = corediff(base_path, opt_path);
            
            // save results
            time_t clk = time(NULL);
            char* timestamp = strtok(ctime(&clk), "\n");

            char* results[1024];
            sprintf(results, "%s,%s,%s,%d,%d\n", timestamp, line, flags.data[i], sim.insertions, sim.deletions);
            file_append("../tmp/results.csv", results);

            free(opt_path);
        }

        free(base_path);
    }

    if (line)
        free(line);

    fclose(programs_fp);

    // clean memory used to store flags
    for (int i = 0; i < flags.size; i++)
        free(flags.data[i]);
    free(flags.data);

    return 0;
}