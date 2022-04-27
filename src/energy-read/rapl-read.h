#include <stdlib.h>

typedef struct rapl_st {
    double pkg_energy;
    double pp0, pp1;
    double dram;
    double psys;
} rapl_st;

typedef struct pkg_arr_st {
    double time;
    size_t size;
    rapl_st** data;
} pkg_arr;

pkg_arr rapl_read(char* cmd);