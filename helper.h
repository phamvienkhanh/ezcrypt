#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

typedef uint32_t word32;
typedef unsigned char byte;

static void print_hex(word32 val) {
    printf("0x%08x", val);
}

static void print_state(word32 state[16]) {
    for(int i = 0; i < 16; i++) {
        printf("0x%08x, ", state[i]);
        if((i+1)%4==0) printf("\n");
    }
    printf("\n");
}

static void print_bytes(byte* data, int len) {
    for(int i = 0; i < len; i++) {
        printf("0x%02x, ", data[i]);
        if((i+1)%16==0) printf("\n");
    }
    printf("\n");
}

static void print_words(word32* data, int len) {
    for(int i = 0; i < len; i++) {
        printf("0x%08x ", data[i]);
        if((i+1)%4==0) printf("\n");
    }
    printf("\n");
}

static void print_state4(word32 state[4]) {
    for(int i = 0; i < 4; i++) {
        printf("%08x\n", state[i]);
    }
    printf("\n");
}

static void print_progress(double progress) {
    int barWidth = 50;
    int pos = (int)(barWidth * progress);

    printf("[");
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %3.0f%%\r", progress * 100.0);
    fflush(stdout);
}

#define ROL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))


#endif