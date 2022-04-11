#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int mkpath(char *path)
{
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", path);

    return system(cmd);
}

// Builds program path strings (i.e. "parallel/blackscholes/-O0")
// WARNING: memory needs to be freed after use
char *prog_path(char *program, char *flag)
{
    char *path = malloc(strlen(program) + strlen(flag) + 2 + 1);
    sprintf(path, "%s/%s", program, flag);
    return path;
}

// Compiles program with specified flag.
// Copies last .cbor files and converts them to .txt
// Returns char* with path to files
// WARNING: memory needs to be freed after use
char* compile(char *path, char *flag)
{
    // create results directory
    char res[1024];
    sprintf(res, "../tmp/%s/%s", path, flag);
    mkpath(res);

    char *root = getcwd(NULL, 0);

    // change to benchmark directory
    chdir("../nofib");
    if (chdir(path) != 0) {
        // FIXME: abort
        printf("==> Could not change directory to: %s\n==> PWD: %s\n", path, getcwd(NULL, 0));
        return NULL;
    }

    // compile benchmark
    char make_cmd[1024];
    printf("==> compiling %s with flag: %s\n", path, flag);
    printf("pwd: %s\n", getcwd(NULL, 0));
    // sprintf(make_cmd, "make NoFibRuns=0 EXTRA_HC_OPTS=\"-O0 -fplugin GhcDump.Plugin %s\"", flag);
    sprintf(make_cmd, "make NoFibRuns=0 EXTRA_HC_OPTS=\"-O0 -fplugin GhcDump.Plugin %s\" >null 2>null", flag);
    // FIXME: check compilation status
    if (system(make_cmd) == 0) {
        printf("==> %s successfuly compiled with flag: %s\n", path, flag);
    } else {
        // FIXME: abort compilation
        printf("==> Not able to compile %s with flag: %s\n", path, flag);
        return NULL;
    }

    FILE *hs_fp, *cbor_fp;
    char filename[1024], buffer[1024], cmd[2048];

    // get .hs files in folder
    hs_fp = popen("find *.hs *.lhs 2> /dev/null", "r");
    if (hs_fp == NULL)
    {
        printf("Error looking for .hs files");
        exit(1);
    }

    while (fgets(filename, sizeof(filename), hs_fp) != NULL)
    {
        // remove extension (.hs)
        filename[strlen(filename) - 4] = 0;

        sprintf(buffer, "find %s.pass-*.cbor | tail -1", filename);
        cbor_fp = popen(buffer, "r");
        fgets(buffer, sizeof(buffer), cbor_fp);
        pclose(cbor_fp);

        // remove "\n" from the end of buffer
        buffer[strlen(buffer) - 1] = 0;

        // FIXME: this was not tested yet
        sprintf(cmd, "cp %s %s/%s/%s.cbor", buffer, root, res, filename);
        system(cmd);

        sprintf(cmd, "%s/../bin/cbor-txt %s %s/%s/%s.txt", root, buffer, root, res, filename);
        system(cmd);
    }

    pclose(hs_fp);

    // clean benchmark directory
    system("make clean >null 2>null");
    system("rm *.cbor");
    printf("==> cleaned %s\n\n", path);

    // go back to root directory
    chdir(root);
    free(root);

    return prog_path(path, flag);
}
