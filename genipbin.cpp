#include<stdio.h>
#include<stdlib.h>

int main() {
    for(int i=0; i<500000; i++) {
        int v = rand();
        putchar(v&0xFF);
        putchar((v>>8)&0xFF);
        putchar((v>>16)&0xFF);
        putchar((v>>24)&0xFF);
    }
}
