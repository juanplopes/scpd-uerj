#include<stdio.h>
#include<stdlib.h>
#include<set>

std::set<int> S;

int main() {
    while (true) {
        int v = getchar();
        if (v==EOF) break;
        v = v&0xFF;
        v = (v<<8)|(getchar()&0xFF);
        v = (v<<8)|(getchar()&0xFF);
        v = (v<<8)|(getchar()&0xFF);
        S.insert(v);        
    }
    printf("%lu\n", S.size());
}
