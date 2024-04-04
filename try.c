#include <stdio.h>

// Function to generate combinations with exactly 7 bits set to 1
void generateCombinations(unsigned long long int current, int bits_set, int position, int *count) {
    // Base case: if we've set 7 bits, increment the count
    if (bits_set == 7) {
        (*count)++;
        if (*count % 100000 == 0) {
            printf("\rCombinations: %d", *count);
            fflush(stdout);
        }
        return;
    }

    // Recursively generate combinations
    for (int i = position; i < 48; i++) {
        generateCombinations(current | (1ULL << i), bits_set + 1, i + 1, count);
    }
}

int main() {
    int count = 0;
    generateCombinations(0, 0, 0, &count);
    printf("\nTotal Combinations: %d\n", count);
    return 0;
}
