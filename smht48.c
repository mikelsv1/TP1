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
    if (bits_set == 7)
    {
        (*count)++;

        uint8_t calculated_tag[6];
        uint8_t key[6];
        for (int i = 0; i < 6; i++)
        {
            key[i] = (current >> (i * 8)) & 0xFF;
        }
        smht48(key, 6, m, calculated_tag);

        if (memcmp(calculated_tag, target_tag, 6) == 0)
        {
            printf("Found key with weight 7: ");
            for (int i = 0; i < 6; i++)
            {
                printf("%02X", key[i]);
            }
            printf("\n");

            memcpy(found_key, key, 6);
            return found_key;
        }

        if (*count % 100000 == 0)
        {
            printf("Keys checked: %d\r", *count);
            fflush(stdout);
        }

        return NULL;
    }

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

void colsearch(uint8_t *m)
{
    uint8_t *hash_table[16777216][65536];

    for (int i = 0; i < 16777216; i++)
    {
        for (int j = 0; j < 65536; j++)
        {
            hash_table[i][j] = NULL;
        }
    }

    for (int i = 0; i < 65536; i++)//compute all possible messagse
    {
        memcpy(m, &i, 2);

        uint8_t h1[6];
        tcz48_dm(m, h1);

        uint32_t bucket_index = 0;
        for (int k = 0; k < 6; k++)
        {
            bucket_index += h1[k]; 
        }
        bucket_index %= 65536; 

        int index = 0;
        while (hash_table[bucket_index][index] != NULL)
        {
            index++;
        }
        hash_table[bucket_index][index] = m;

        for (int j = 0; hash_table[bucket_index][j] != NULL; j++)
        {
            uint8_t *stored_m = hash_table[bucket_index][j];
            // Check for collision
            if (memcmp(m, stored_m, 16) == 0)
            {
                printf("Collision found:\n");
                printf("m1: ");
                printhash(stored_m);
                printf("\nm2: ");
                printhash(m);
                printf("\n");
                return;
            }
        }
    }

    printf("No collision found.\n");
}

void smht48ef()
{
    uint8_t key[16];
    for (int i = 0; i < 16; i++)
    {
        key[i] = rand() % 256;
    }

    uint8_t m[16];
    colsearch(m);

    uint8_t tag[6];
    smht48(key, 16, m, tag);

    printf("Forgery:\n");
    printf("m: ");
    printhash(m);
    printf("\nTag: ");
    printhash(tag);
    printf("\n");
}

int main()
{
    char choice;
    clock_t t;

    do
    {
        t = clock();
        printf("Choose an option:\n");
        printf("1. Verify implementation of smht48 on the test vectors (Q1 & Q2)\n");
        printf("2. Implement a function keyrec to search for k (Q3)\n");
        printf("3. Implement functions colsearch and smht48ef (Q6)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf(" %c", &choice) != 1)
        {
            printf("Failed to read integer.\n");
        }

        if (choice == '1')
        {
            examples_smht48();
        }
        else if (choice == '2')
        {
            examples_keyrec();
        }
        else if (choice == '3')
        {
            smht48ef();
        }
        else if (choice == '4')
        {
            printf("Exiting...\n");
        }
        else
        {
            printf("Invalid choice! Please choose again.\n");
        }
        t = clock() - t;
        double time_taken = ((double)t) / CLOCKS_PER_SEC;
        printf("\nIt took %f seconds to execute .\n\n", time_taken);

    } while (choice != '4');

    return 0;
}