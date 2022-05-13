#include <stdlib.h>

typedef struct str_arr
{
    char** data;
    size_t size;
} str_arr;

/**
 * @brief read file contents to memory.
 * 
 * @param filename path to file
 * @return str_arr resulting array
 */
str_arr file2arr(const char* filename);

int file_append(const char* filename, const char* data);