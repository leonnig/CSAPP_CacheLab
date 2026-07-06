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

int hit_count = 0, miss_count = 0,  eviction_count=0, current_time = 0;

void access_cache(unsigned long long set_index, unsigned long long tag, int E, CacheLine **cache) {
    //Check if valid == 1 and tag == 1
    int pass1 = 0, pass2 = 0;
    for (int i = 0; i < E; i++) {
        if(cache[set_index][i].valid == 1 && cache[set_index][i].tag == tag) {
            hit_count++;
            current_time++;
            cache[set_index][i].lru_counter = current_time;
            pass1 = 1;
            break;
        }
    }   
    //if miss but have empty room 
    if (pass1 == 0) {
        miss_count++;
        for (int i = 0; i < E; i++) {
            if (cache[set_index][i].valid == 0) {
                cache[set_index][i].valid = 1;
                cache[set_index][i].tag = tag;
                current_time++;
                cache[set_index][i].lru_counter = current_time;
                pass2 = 1;
                break;
        }
        }
    }

    //if miss and also no empty room
    if (pass1 == 0 && pass2 == 0){
        int min = cache[set_index][0].lru_counter;
        int which_one;
        eviction_count++;
        for (int i = 0; i < E; i++) {
            if(cache[set_index][i].lru_counter <= min) {
                min = cache[set_index][i].lru_counter;
                which_one = i;
            }
        }
        current_time++;
        cache[set_index][which_one].tag = tag;
        cache[set_index][which_one].lru_counter = current_time;
    }
}

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

    //Initialize the whole CacheLine
    for (int i = 0; i < S; i++) {
        cache[i] = (CacheLine *)malloc(E * sizeof(CacheLine));
        for(int j = 0; j < E; j++) {
            cache[i][j].lru_counter = 0;
            cache[i][j].tag = 0;
            cache[i][j].valid = 0;
        }
    }

    //Scan the file.
    FILE *pFile;
    pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("Error : File %s not found.\n", filename);
        exit(1);
    }
    
    char identifier;
    unsigned long long address;
    int size;

    while (fscanf(pFile, " %c  %llx, %d", &identifier, &address, &size) > 0) {
        if (identifier == 'I'){
           continue;
        }
        unsigned long long set_index = (address >> b) & ((1U<<s)-1);
        unsigned long long tag = address >> (s+b);
        
        printf("%c %llx %llx %llx\n", identifier, address, set_index, tag);
        if (identifier == 'L') {
            access_cache(set_index, tag, E, cache);
        }
        else if (identifier == 'S'){
            access_cache(set_index, tag, E, cache);
        }
        else if (identifier == 'M'){
            access_cache(set_index, tag, E, cache);
            access_cache(set_index, tag, E, cache);
        }
        
    }

    fclose(pFile);

    //Free cache
    for(int i = 0; i < S; i++){
        free(cache[i]);  
    }
    free(cache);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}