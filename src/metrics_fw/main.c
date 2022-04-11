#include <stdio.h>
#include <stdlib.h>

// 1st - command to run
// 2nd - program name
// 3rd - compilation flags
int main(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    system("pwd");
    system(argv[1]);
    system("pwd");

    return 0;
}