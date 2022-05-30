#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Compiles program with specified flag and runs it the specified times
int compile(char *path, char *flag, int runs)
{
    char *root = getcwd(NULL, 0);

    // change to benchmark directory
    chdir("./nofib");
    if (chdir(path) != 0) {
        printf("==> Could not change directory to: %s\n==> PWD: %s\n", path, getcwd(NULL, 0));
        return 1;
    }

    // compile benchmark
    char make_cmd[1024];
    printf("==> compiling %s with flag: %s\n", path, flag);
    // sprintf(make_cmd, "sudo make NoFibRuns=%d EXTRA_HC_OPTS=\"-package-db /home/rapi/.ghc/x86_64-linux-8.6.5/package.conf.d -O0 %s\" >null 2>null", runs, flag);
    sprintf(make_cmd, "sudo make NoFibRuns=%d EXTRA_HC_OPTS=\"-package-db /home/rapi/.ghc/x86_64-linux-8.6.5/package.conf.d -O0 %s\"", runs, flag);

    int ret;
    if ((ret = system(make_cmd)) == 0) {
        printf("==> %s successfuly compiled with flag: %s\n", path, flag);
    } else {
        printf("==> Not able to compile %s with flag: %s\n", path, flag);
    }

    // clean benchmark directory
    system("make clean >null 2>null");
    printf("==> cleaned %s\n\n", path);

    // go back to root directory
    chdir(root);
    free(root);

    return ret;
}