#include "helper.h"

const byte sbox[] ={
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 
};

const word32 rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 
    0x10000000, 0x20000000, 0x40000000, 0x80000000, 
    0x1B000000, 0x36000000, 0x6C000000, 0xD8000000,
    0xAB000000, 0x4D000000, 0x9A000000
}; 

byte subbyte(byte b) {
    int r = (b & 0xF0) >> 4;
    int c = b & 0x0F;

    return sbox[r*16 + c];
}

word32 subword(word32 word) {
    return (subbyte(word >> 24) << 24) 
        | (subbyte((word & 0x00FF0000) >> 16) << 16)
        | (subbyte((word & 0x0000FF00) >> 8) << 8)
        | (subbyte(word & 0x000000FF)); 
}

word32 rotword(word32 word, int n) {
    return (word << 8*n) | (word >> (32 - 8*n));
}

void key_expansion(byte key[16], word32 words[44], int nk, int nr)
{
    for (int i = 0; i < nk; i++)
    {
        words[i] = key[i * 4] << 24 | key[i * 4 + 1] << 16 | key[i * 4 + 2] << 8 | key[i * 4 + 3];
    }

    int n = 4 * (nr + 1);
    for (int i = nk; i < n; i++)
    {
        word32 temp = words[i - 1];
        if (i % nk == 0)
        {
            temp = subword(rotword(temp, 1)) ^ rcon[i / nk - 1];
        }
        else if (i > 6 && i % nk == 4)
        {
            temp = subword(temp);
        }

        words[i] = words[i - nk] ^ temp;
    }
}

void init_state(word32 state[4], byte in[16]) {
    state[0] = (in[0] << 24) | (in[4] << 16 & 0x00FF0000) | (in[8] << 8 & 0x0000FF00) | (in[12] & 0x000000FF); 
    state[1] = (in[1] << 24) | (in[5] << 16 & 0x00FF0000) | (in[9] << 8 & 0x0000FF00) | (in[13] & 0x000000FF); 
    state[2] = (in[2] << 24) | (in[6] << 16 & 0x00FF0000) | (in[10] << 8 & 0x0000FF00) | (in[14] & 0x000000FF); 
    state[3] = (in[3] << 24) | (in[7] << 16 & 0x00FF0000) | (in[11] << 8 & 0x0000FF00) | (in[15] & 0x000000FF); 
}

void sub_bytes(word32 state[4]) {
    state[0] = subword(state[0]);
    state[1] = subword(state[1]);
    state[2] = subword(state[2]);
    state[3] = subword(state[3]);
}

void shift_rows(word32 state[4]) {
    state[1] = rotword(state[1], 1);
    state[2] = rotword(state[2], 2);
    state[3] = rotword(state[3], 3);
}

byte x2(byte x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1B : 0x00);
}

byte x3(byte x) {
    return x2(x) ^ x;
}

byte xtime(byte x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1B : 0x00);
}

byte mul_gf(byte a, byte b) {
    byte res = 0x00;
    while (b)
    {
        if(b&0x01) {
            res ^= a;
        }

        a = xtime(a);
        b = b >> 1;
    }
    
    return res;
}

void mix_col(byte* a0, byte* a1, byte* a2, byte* a3) {
    byte b0 = mul_gf(*a0, 0x02) ^ mul_gf(*a1, 0x03) ^ *a2 ^ *a3; 
    byte b1 = *a0 ^ mul_gf(*a1, 0x02) ^ mul_gf(*a2, 0x03) ^ *a3; 
    byte b2 = *a0 ^ *a1 ^ mul_gf(*a2, 0x02) ^ mul_gf(*a3, 0x03);
    byte b3 = mul_gf(*a0, 0x03) ^ *a1 ^ *a2 ^ mul_gf(*a3, 0x02); 

    *a0 = b0; *a1 = b1; *a2 = b2; *a3 = b3;
}

void mix_cols(word32 state[4]) {
    mix_col((byte*)state + 0, (byte*)state + 4, (byte*)state + 8,  (byte*)state + 12);
    mix_col((byte*)state + 1, (byte*)state + 5, (byte*)state + 9,  (byte*)state + 13);
    mix_col((byte*)state + 2, (byte*)state + 6, (byte*)state + 10, (byte*)state + 14);
    mix_col((byte*)state + 3, (byte*)state + 7, (byte*)state + 11, (byte*)state + 15);
}

void add_roundkey(word32 state[4], word32 keyExpand[44], int r) {
    word32 wk3 = ((keyExpand[r*4 + 0] & 0x000000FF) << 24) | 
                 ((keyExpand[r*4 + 1] & 0x000000FF) << 16) | 
                 ((keyExpand[r*4 + 2] & 0x000000FF) <<  8) | 
                 ((keyExpand[r*4 + 3] & 0x000000FF) <<  0) ;
    
    word32 wk2 = ((keyExpand[r*4 + 0] & 0x0000FF00) << 16) | 
                 ((keyExpand[r*4 + 1] & 0x0000FF00) <<  8) | 
                 ((keyExpand[r*4 + 2] & 0x0000FF00) <<  0) | 
                 ((keyExpand[r*4 + 3] & 0x0000FF00) >>  8) ;

    word32 wk1 = ((keyExpand[r*4 + 0] & 0x00FF0000) <<  8) | 
                 ((keyExpand[r*4 + 1] & 0x00FF0000) <<  0) | 
                 ((keyExpand[r*4 + 2] & 0x00FF0000) >>  8) | 
                 ((keyExpand[r*4 + 3] & 0x00FF0000) >> 16) ;

    word32 wk0 = ((keyExpand[r*4 + 0] & 0xFF000000) <<  0) | 
                 ((keyExpand[r*4 + 1] & 0xFF000000) >>  8) | 
                 ((keyExpand[r*4 + 2] & 0xFF000000) >> 16) | 
                 ((keyExpand[r*4 + 3] & 0xFF000000) >> 24) ;

    state[0] ^= wk0;
    state[1] ^= wk1;
    state[2] ^= wk2;
    state[3] ^= wk3;
}

void aes_128_enc(byte key[16], byte inp[16], byte out[16])
{
    word32 keyExpand[44];
    key_expansion(key, keyExpand, 4, 10);

    word32 state[4];
    init_state(state, inp);

    add_roundkey(state, keyExpand, 0);

    for (int i = 1; i < 10; i++)
    {
        sub_bytes(state);
        shift_rows(state);
        mix_cols(state);
        add_roundkey(state, keyExpand, i);
    }
    
    sub_bytes(state);
    shift_rows(state);
    add_roundkey(state, keyExpand, 10);

    word32 w = 0; 
    w = ((state[3] & 0xFF000000) >>  0) |
        ((state[2] & 0xFF000000) >>  8) |
        ((state[1] & 0xFF000000) >> 16) |
        ((state[0] & 0xFF000000) >> 24);
    memcpy(out, &w, 4); out += 4;

    w = ((state[3] & 0x00FF0000) <<  8) |
        ((state[2] & 0x00FF0000) >>  0) |
        ((state[1] & 0x00FF0000) >>  8) |
        ((state[0] & 0x00FF0000) >> 16);
    memcpy(out, &w, 4); out += 4;

    w = ((state[3] & 0x0000FF00) << 16) |
        ((state[2] & 0x0000FF00) <<  8) |
        ((state[1] & 0x0000FF00) >>  0) |
        ((state[0] & 0x0000FF00) >>  8);
    memcpy(out, &w, 4); out += 4;

    w = ((state[3] & 0x000000FF) << 24) |
        ((state[2] & 0x000000FF) << 16) |
        ((state[1] & 0x000000FF) <<  8) |
        ((state[0] & 0x000000FF) <<  0);
    memcpy(out, &w, 4);
}

int main(int argc, char const *argv[])
{
    byte key[] = {0x2b ,0x7e ,0x15 ,0x16 ,0x28 ,0xae ,0xd2 ,0xa6 ,0xab ,0xf7 ,0x15 ,0x88 ,0x09 ,0xcf ,0x4f ,0x3c};
    byte inp[] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    byte exp[] = {0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb, 0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32};
    byte out[16];

    aes_128_enc(key, inp, out);

    if(memcmp(out, exp, 16) != 0) {
        printf("compare falied\n");
        print_bytes(out, 16);
        print_bytes(exp, 16);
    }
    else {
        printf("[AES][128] test pass\n");
    }


    return 0;
}
