#include "corediff.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>


corediff_sim corediff(char* base, char* opt) {
    char* cmd[2048], filename[254];
    FILE *cbor_fp;

    char* root = getecwd(NULL, 0);
    chdir(base);

    if ((cbor_fp = popen("find *.cbor", "r")) == NULL) {
        printf("Error looking for .cbor files.");
        exit(1);
    }

    chdir(root);
    free(root);

    int deletions = 0, insertions = 0;

    // iterate through cbor files
    while(fgets(filename, sizeof(filename), cbor_fp) != NULL) {

        sprintf(cmd, 
            "./../bin/corediff diff %s/%s %s/%s | ansi2txt | awk \"/^- / { ++r1 } /^\\+ / { ++r2 }\"' END { print \"del:\",r1 \"\\nins:\",r2 }'", 
            base, filename, opt, filename);

        FILE *cmd_fp;
        if ((cmd_fp = popen(cmd, "r")) == NULL) {
            printf("Error diffing files.\n");
            exit(1);
        }

        char* sim[254];
        if (fgets(sim, sizeof(sim), cmd_fp) == NULL) {
            printf("Error calculating similarity.\n");
            exit(1);
        }

        pclose(cmd_fp);

        int del, ins;
        sscanf(sim, "del: %d ins: %d", &del, &ins);

        deletions += del;
        insertions += ins;
    }
    
    pclose(cbor_fp);

    corediff_sim sim = { .insertions = insertions, .deletions = deletions};
    return sim;
}
