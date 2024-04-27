#include<vector>
#include<random>

#include "bloom_zobrist.h"

using namespace std;


vector<unsigned long long> generateZobristKeys(int size) {
    vector<unsigned long long> keys;
    keys.resize(size);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<unsigned long long> dis(0, std::numeric_limits<unsigned long long>::max());

    for (int i = 0; i < size; ++i) {
        keys.push_back(dis(gen));
    }

    return keys;
}