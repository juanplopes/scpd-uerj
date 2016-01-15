#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <bitset>
#include "CImg.h"
#include "murmur.h"
#define HUE 8
#define SATURATION 3
#define VALUE 3
#define POSITION 8
#define THRESHOLD 10

using namespace std;

struct rgb {
    double r, g, b;
    rgb(unsigned char r, unsigned char g, unsigned char b) : r(r/256.0), g(g/256.0), b(b/256.0) {};
};

struct hsv {
    double h, s, v;
};

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0;
        return out;
    }
    if( max > 0.0 ) {
        out.s = (delta / max);
    } else {
        out.s = 0.0;
        out.h = NAN;
        return out;
    }
    if( in.r >= max ) 
        out.h = ( in.g - in.b ) / delta; 
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta; 
    else
        out.h = 4.0 + ( in.r - in.g ) / delta; 

    out.h *= 60.0;                             

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

uint64_t imghash(char* file) {
    cimg_library::CImg<unsigned char> img(file);
 
    unsigned char A[5];
    uint64_t B[2];
    int R[64];
    
    memset(A, 0, sizeof A);
    memset(R, 0, sizeof R);

    int h = img.height(), w = img.width(), c = img.spectrum();

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            for(int k=0; k<3; k++)
                A[k] = img(i, j, 0, std::min(k, c-1));        

            rgb x1 = rgb(A[0], A[1], A[2]);
            hsv x2 = rgb2hsv(x1);
            A[0] = (unsigned char)(x2.h / 360.0 * HUE);
            A[1] = (unsigned char)(x2.s * SATURATION);
            A[2] = (unsigned char)(x2.v * VALUE);
            A[3] = (int)((i/(double)w)*POSITION);
            A[4] = (int)((j/(double)h)*POSITION);
           
            MurmurHash3_x64_128(A, 5, 0, B);
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

struct ImageSet {
    map<uint64_t, uint64_t> values;
    map<uint64_t, vector<uint64_t> > S;
    map<uint64_t, int> SC;
    
    void add(int64_t id, int64_t value) {
        this->values[id] = value;
        uint64_t foundAt = 0;
        int foundCount = 100;
        for(auto it = S.begin(); it!=S.end(); it++) {
            for(int i=0; i<it->second.size(); i++) {
                int count = bitset<64>(values[it->second[i]] ^ value).count();
                if (count < foundCount) {
                    foundCount = count;
                    foundAt = it->first;
                }
            }

        }
        if (foundCount <= THRESHOLD) {
            S[foundAt].push_back(id);
            SC[id] = foundCount;
        } else {
            S[id].push_back(id);
            SC[id] = 0;
        }
    }
};
