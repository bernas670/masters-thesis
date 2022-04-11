
typedef struct corediff_st
{
    int insertions;
    int deletions;
} corediff_sim;

corediff_sim corediff(char* base, char* opt);