#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ht48.h"

void smht48(const uint8_t k[static 6], uint64_t blen, const uint8_t m[blen], uint8_t h[static 6])
{
    uint8_t m_k[6 + blen];

    memcpy(m_k, m, blen);
    memcpy(m_k + blen, k, 6);

    ht48(6 + blen, m_k, h);
}

void test_smht48(const uint8_t k[static 6], uint64_t blen, const uint8_t m[blen], char *expected)
{
    uint8_t h[6];
    smht48(k, blen, m, h);
    printf("\n-------------------------\nResult: ");
    printhash(h);
    printf("\n");
    printf("Expected: %s\n", expected);
    printf("-------------------------\n");
}

uint8_t *generateCombinations(unsigned long long int current, int bits_set, int position, int *count, const uint8_t m[static 6], const uint8_t target_tag[static 6], uint8_t *found_key)
{
    // Base case: if we've set 7 bits, check if the calculated tag matches the target tag
    if (bits_set == 7)
    {
        (*count)++;

        // Calculate the hash using the candidate key
        uint8_t calculated_tag[6];
        uint8_t key[6];
        for (int i = 0; i < 6; i++)
        {
            key[i] = (current >> (i * 8)) & 0xFF;
        }
        smht48(key, 6, m, calculated_tag);

        // Check if the calculated tag matches the target tag
        if (memcmp(calculated_tag, target_tag, 6) == 0)
        {
            printf("Found key with weight 7: ");
            for (int i = 0; i < 6; i++)
            {
                printf("%02X", key[i]);
            }
            printf("\n");
            
            // Copy the key to the pre-allocated buffer
            memcpy(found_key, key, 6);
            return found_key;
        }

        if (*count % 100000 == 0)
        {
            printf("Keys checked: %d\r", *count);
            fflush(stdout); // Flush buffer to ensure immediate output
        }

        return NULL;
    }

    // Recursively generate combinations
    for (int i = position; i < 48; i++)
    {
        if (generateCombinations(current | (1ULL << i), bits_set + 1, i + 1, count, m, target_tag, found_key) != NULL)
        {
            return found_key;
        }
    }

    return NULL;
}


uint8_t *keyrec(uint64_t blen, const uint8_t m[blen], char *tag)
{
    uint8_t target_tag[6];
    for (int i = 0; i < 6; i++)
    {
        sscanf(tag + 2 * i, "%2hhx", &target_tag[i]);
    }

    uint8_t *found_key = (uint8_t *)malloc(6 * sizeof(uint8_t));
    int count = 0;

    return generateCombinations(0, 0, 0, &count, m, target_tag, found_key);
}

void examples_keyrec()
{
    uint8_t m[6] = {9, 8, 7, 6, 5, 4};
    uint64_t blen = 6;
    uint8_t *k = keyrec(blen, m, "7D1DEFA0B8AD");
    test_smht48(k, blen, m, "7D1DEFA0B8AD");
    free(k);
}

void examples_smht48()
{
    uint8_t k1[6] = {0, 1, 2, 3, 4, 5};
    uint8_t m1[6] = {9, 8, 7, 6, 5, 4};
    uint64_t blen1 = 6;
    test_smht48(k1, blen1, m1, "EE75794547B8");

    uint8_t k2[6] = {0xE4, 0x16, 0x9F, 0x12, 0xD3, 0xBA};
    uint8_t m2[6] = {9, 8, 7, 6, 5, 4};
    uint64_t blen2 = 6;
    test_smht48(k2, blen2, m2, "5F265B72B5EC");
}

int main()
{
    clock_t t;
    t = clock();

    examples_keyrec();

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("\nIt took %f seconds to execute .\n", time_taken);
    exit(1);
}
