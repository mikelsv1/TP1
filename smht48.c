#include <stdint.h>
#include "ht48.h"

/*
* Input k: a 48-bit key stored as an array of 6 bytes
* Input blen: the byte length of m, stored on 64 bits
* Input m: the message to be hashed, whose length is required to be an
integral number of bytes↪→
* Input h: placeholder for the 48-bit resulting tag, to be stored as an
array of 6 bytes. Must have been allocated by the caller.↪→
* Output: void, h is overwritten with the result ht48(m||k)
* Warning: the key bytes in k must be appended _in order_ (k[0] first)
*/

void smht48(const uint8_t k[static 6], uint64_t blen, const uint8_t m[blen], uint8_t h[static 6]) {
    int len_m_k = blen + 6;
    uint8_t* m_k = (uint8_t*) malloc(len_m_k * sizeof(uint8_t));

    for (int i = 0; i < (int) blen; i++) {
        m_k[i] = m[i];
    }
    for (int i = 0; i < 6; i++) {
        m_k[i + (int) blen] = k[i];
    }
    
    ht48(len_m_k, m_k, h);
    free(m_k);
}

void test_smht48(const uint8_t k[static 6], uint64_t blen, const uint8_t m[blen], char* expected) {
    uint8_t h[6];
    smht48(k, blen, m, h);
    printf("\n-------------------------\nResult: ");
    printhash(h);
    printf("\n");
    printf("Expected: %s\n", expected);
    printf("-------------------------\n");
}

void keyrec(uint64_t blen, const uint8_t m[blen], char* tag){
    
}


void examples_keyrec(){
    uint8_t m = {9, 8, 7, 6, 5, 4}
    keyrec(m, 6, "7D1DEFA0B8AD");
}

void examples_smht48() {
    // 1. Key value: {0, 1, 2, 3, 4, 5}
    // Message value: {9, 8, 7, 6, 5, 4}
    // Tag value: EE75794547B8
    uint8_t k1[6] = {0, 1, 2, 3, 4, 5};
    uint8_t m1[6] = {9, 8, 7, 6, 5, 4};
    uint64_t blen1 = 6;
    test_smht48(k1, blen1, m1, "EE75794547B8");

    // 2. Key value: {0xE4, 0x16, 0x9F, 0x12, 0xD3, 0xBA}
    // Message value: {9, 8, 7, 6, 5, 4}
    // Tag value: 5F265B72B5EC
    uint8_t k2[6] = {0xE4, 0x16, 0x9F, 0x12, 0xD3, 0xBA};
    uint8_t m2[6] = {9, 8, 7, 6, 5, 4};
    uint64_t blen2 = 6;
    test_smht48(k2, blen2, m2, "5F265B72B5EC");
}

int main() {
    keyrec();
    return 0;
}