#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <bitset>
#include "CImg.h"
#include "murmur.h"
#define HUE 16
#define SATURATION 3
#define VALUE 3
#define POSITION 3
#define THRESHOLD 10

using namespace std;

struct rgb {
    double r, g, b;
    rgb(unsigned char r, unsigned char g, unsigned char b) : r(r/256.0), g(g/256.0), b(b/256.0) {};
};

struct hsl {
    double h, s, l;
};

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
    
    double difference(vector<uint64_t> &a, vector<uint64_t> &b) {
        int sum = 0, count = 0;
        for(int i=0; i<a.size(); i++) {
            for(int j=0; j<b.size(); j++) {
                sum += bitset<64>(values[a[i]] ^ values[b[i]]).count();
                count++;
            }
        }
        return (double)sum/count;
    }
};

inline hsl rgb2hsl(rgb c1)
{
    double themin,themax,delta;
    hsl c2;
    using namespace std;

    themin = std::min(c1.r,std::min(c1.g,c1.b));
    themax = std::max(c1.r,std::max(c1.g,c1.b));
    delta = themax - themin;
    c2.l = (themin + themax) / 2;
    c2.s = 0;
    if (c2.l > 0 && c2.l < 1)
        c2.s = delta / (c2.l < 0.5 ? (2*c2.l) : (2-2*c2.l));
    c2.h = 0;
    if (delta > 0) {
        if (themax == c1.r && themax != c1.g)
            c2.h += (c1.g - c1.b) / delta;
        if (themax == c1.g && themax != c1.b)
            c2.h += (2 + (c1.b - c1.r) / delta);
        if (themax == c1.b && themax != c1.r)
            c2.h += (4 + (c1.r - c1.g) / delta);
        c2.h *= 60;
    }
    return(c2);
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
            hsl x2 = rgb2hsl(x1);
            A[0] = (unsigned char)(x2.h / 360.0 * HUE);
            A[1] = (unsigned char)(x2.s * SATURATION);
            A[2] = (unsigned char)(x2.l * VALUE);
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
        answer <<= 1;
        if (R[i] >= 0)
            answer |= 1;
    }
    return answer;
}
