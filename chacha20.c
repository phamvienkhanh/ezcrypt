#include "helper.h"


static inline void quarter_round(word32 state[16], word32 a, word32 b, word32 c, word32 d) {
    state[a] += state[b]; state[d] ^= state[a]; state[d] = ROL32(state[d], 16);
    state[c] += state[d]; state[b] ^= state[c]; state[b] = ROL32(state[b], 12);
    state[a] += state[b]; state[d] ^= state[a]; state[d] = ROL32(state[d], 8);
    state[c] += state[d]; state[b] ^= state[c]; state[b] = ROL32(state[b], 7);
}

static inline void inner_block(word32 state[16]) {
    quarter_round(state, 0, 4, 8, 12);
    quarter_round(state, 1, 5, 9, 13);
    quarter_round(state, 2, 6, 10, 14);
    quarter_round(state, 3, 7, 11, 15);
    quarter_round(state, 0, 5, 10, 15);
    quarter_round(state, 1, 6, 11, 12);
    quarter_round(state, 2, 7, 8, 13);
    quarter_round(state, 3, 4, 9, 14);
}

static inline void init_state(word32 state[16], const byte *key, const byte *nonce, word32 counter)
{
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;

    for (int i = 0; i < 8; i++)
    {
        state[4 + i] = *(word32*)(key + i * 4);
    }

    state[12] = counter;

    for (int i = 0; i < 3; i++)
    {
        state[13 + i] = *(word32*)(nonce + i * 4);
    }
}

static inline void chacha20_block(word32 state[16], const byte *key, const byte *nonce, word32 counter, byte keystream [64]) {
    init_state(state, key, nonce, counter);

    word32 origin_state[16];
    for(int i = 0; i < 16; i++){
        origin_state[i] = state[i];
    }

    for (int i = 0; i < 10; i++)
    {
        inner_block(state);
    }

    for(int i = 0; i < 16; i++) {
        state[i] += origin_state[i];
    }

    for(int i = 0; i < 64; i++) {
        keystream[i] = *((byte*)state + i);
    }
}

static void chacha20_encrypt(const byte *key, const byte *nonce, word32 counter, const byte* in, word32 inLen, byte* out) {
    int blocks = inLen/64;
    
    for(int i = 0; i < blocks; i++) {        
        byte keystream[64];
        word32 state[16];
        chacha20_block(state, key, nonce, counter+i, keystream);

        for(int j = 0; j < 64; j++) {
            out[i*64 + j] = in[i*64 + j] ^ keystream[j];
        }
    }

    int left_over = inLen - blocks*64;
    if(left_over) {
        byte keystream[64];
        word32 state[16];
        chacha20_block(state, key, nonce, counter+blocks, keystream);
        for(int i = 0; i < left_over; i++) {
            out[blocks*64 + i] = in[blocks*64 + i] ^ keystream[i];
        }
    }
}

int main(int argc, char const *argv[])
{
    byte key[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 
        0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 
        0x1c, 0x1d, 0x1e, 0x1f};
    byte nonce[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
    
    byte in[] = {
        0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
        0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
        0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
        0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
        0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
        0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
        0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
        0x74, 0x2e
    };

    byte cipher[sizeof(in)];

    chacha20_encrypt(key, nonce, 1, in, sizeof(in), cipher);

    printf("cipher: \n");
    print_bytes(cipher, sizeof(cipher));

    byte plain[sizeof(cipher)];
    chacha20_encrypt(key, nonce, 1, cipher, sizeof(cipher), plain);
    printf("plain: \n");
    print_bytes(plain, sizeof(plain));

    return 0;
}
