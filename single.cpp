#include "murmur.cpp"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define LOG2M 16
#define M (1<<LOG2M)
#define ALPHA_MM ((0.7213 / (1 + 1.079 / M)) * M * M)

unsigned char T[M];

int cardinality() {
    double sum = 0;
    int count = M;

    for(int i=0; i<M; i++)
        sum += pow(2, -T[i]);

    double estimate = ALPHA_MM / sum;
    if (estimate <= (5.0 / 2.0) * count) {
        double zeros = 0.0;
        for(int i=0; i<M; i++)
            if (T[i]==0)
                zeros++;
        return round(count * log(count / zeros));
    } else {
        return round(estimate);
    }
}

int main() {
    char s[1000];
    int count = 0;

    while(scanf("%10c", s) != EOF) {
        count++;
        unsigned int a, b;
        MurmurHash3_x86_32(s, 10, 0, &a);
        MurmurHash3_x86_32(s, 10, 1, &b);
        
        int slot = a&(M-1);
        int value = __builtin_clz(b|1)+1;
        if (value > T[slot])
            T[slot] = value;
    }
    printf("%d %d\n", count, cardinality());
    return 0;
}
