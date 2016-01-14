#include <algorithm>
#include "CImg.h"
#include "murmur.h"

uint64_t imghash(char* file) {
    cimg_library::CImg<unsigned char> img(file);
 
    unsigned char A[3];
    uint64_t B[2];
    int R[64];
    
    memset(A, 0, sizeof A);
    memset(R, 0, sizeof R);

    int h = img.height(), w = img.width(), c = img.spectrum();

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            for(int k=0; k<3; k++)
                A[k] = img(i, j, 0, std::min(k, c-1))/64;        
            
            MurmurHash3_x64_128(A, 3, 0, B);
            B[0] ^= B[1];
            for(int k=0; k<64; k++) {
                if (B[0]&1)
                    R[k]++;
                else
                    R[k]--;
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
