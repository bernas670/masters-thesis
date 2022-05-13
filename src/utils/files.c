#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "files.h"


const size_t BUFF_INC = 20;


str_arr file2arr(const char* filename) {
    FILE *fp;
    str_arr arr = { .data = NULL, .size = -1 };

    if ((fp = fopen(filename, "r")) == NULL) {
        return arr;
    }

    if ((arr.data = calloc(BUFF_INC, sizeof(char*))) == NULL) {
        return arr;
    }
    arr.size = BUFF_INC;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;   
    int num_lines = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (arr.size == num_lines + 1) {
            if ((arr.data = reallocarray(arr.data, arr.size + BUFF_INC, sizeof(char*))) == NULL) {
                // TODO: handle errors
                return arr;
            }
            arr.size += BUFF_INC;
        }


        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = 0;
        }

        arr.data[num_lines] = malloc(strlen(line) + 1);
        strcpy(arr.data[num_lines], line);

        num_lines++;
    }

    if (line) {
        free(line);
    }


    arr.size = num_lines;
    if ((arr.data = reallocarray(arr.data, arr.size, sizeof(char*))) == NULL) {
        // TODO: handle errors
        return arr;
    }

    fclose(fp);
    return arr;
}

int file_append(const char* filename, const char* data) {
    FILE* fp;

    if ((fp = fopen(filename, "a")) == NULL) {
        return 1;
    }

    fprintf(fp, "%s", data);
    fclose(fp);

    return 0;
}