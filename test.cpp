#include "CImg.h"
#include "murmur.h"
#include <iostream>
#include <bitset>

using namespace std;
using namespace cimg_library;

uint64_t hash(char* file) {
    CImg<unsigned char> img(file);
 
    char A[3];
    uint64_t B[2];
    int R[64];
    
    memset(R, 0, sizeof R);
    
    for(int i=0; i<img.width(); i++) {
        for(int j=0; j<img.height(); j++) {
            A[0] = img(i, j, 0, 0);        
            A[1] = img(i, j, 0, 1);        
            A[2] = img(i, j, 0, 2);        
       
            MurmurHash3_x64_128(A, 3, 0, B);
            for(int i=0; i<64; i++) {
                if (B[0]&1)
                    R[i]++;
                else
                    R[i]--;
                B[0]>>=1;
            }
        }
    }

    uint64_t answer = 0;
    for(int i=0; i<64; i++) {
        if (R[i] >= 0)
            answer |= 1;
        answer <<= 1;
    }
    return answer;
}

int main() {
    cout << bitset<64>(hash("test.jpg")) << endl;
    cout << bitset<64>(hash("test.png")) << endl;
    cout << bitset<64>(hash("test.png") ^ hash("test.jpg")).count() << endl;

}
