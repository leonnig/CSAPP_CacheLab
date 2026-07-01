#define _GNU_SOURCE //Avoid the restriction with stdc=99 from makefile.

#include "cachelab.h"
#include <unistd.h>  /* getopt */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int valid;
    unsigned long long tag;
    int lru_counter;
}CacheLine;

int main(int argc, char **argv)
{
    int opt, s = 0, E = 0 ,b = 0;
    char filename[50] = {0};
    while ((opt = getopt(argc, argv, "hvs:E:b:t:"))!=-1) {
        switch (opt) {
            case 'h':
                printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
                break;
            case 'v':
                printf("vvv\n");
                break;
            case 's':
                s = atoi(optarg);
                printf("There is %d sets\n",s);
                break;
            case 'E':
                E = atoi(optarg);
                printf("There is %d lines per set\n",E);
                break;
            case 'b':
                b = atoi(optarg);
                printf("There is %d block\n",b);
                break;
            case 't':
                strncpy(filename, optarg, sizeof(filename));
                printf("The file is %s\n",filename);
                break;
            default:
                printf("wrong argument\n");
                break;
        } 
    }
    
    int S = 1U << s;
    CacheLine **cache = (CacheLine **)malloc(S * sizeof(CacheLine *));
    for (int i = 0; i < S; i++) {
        cache[i] = (CacheLine *)malloc(E * sizeof(CacheLine));
        for(int j = 0; j < E; j++) {
            cache[i][j].lru_counter = 0;
            cache[i][j].tag = 0;
            cache[i][j].valid = 0;
        }
    }
    
    
    for(int i = 0; i < S; i++){
            free(cache[i]);  
    }
    free(cache);
    //printSummary(0, 0, 0);
    return 0;
}
