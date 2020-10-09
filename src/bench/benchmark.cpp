//#ifdef BENCHMARK
#include <random>
#include <cstring>
#include "celero/Celero.h"
#include <iostream>

#include <cell_distances.h>

constexpr size_t szs = 50;
constexpr size_t szb = 1000;

constexpr size_t size = 32;

auto rd = std::default_random_engine();
auto udist = std::uniform_real_distribution<float> (-10.,10.);

class DistanceFixture : public celero::TestFixture
{
    public:
        int16_t* data;
        int16_t** data2;
        int16_t trip[3];
        int16_t res[32];


        DistanceFixture() {
            data = new int16_t[size * 3]();
            data2 = new int16_t*[3];
        }


        virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) {
            data2[0] = data;
            data2[1] = data + 32;
            data2[2] = data + 64;
            for(int i = 0; i < size; i++) {
                data2[0][i] = 2;
                data2[1][i] = 2;
                data2[2][i] = 2;
                res[i] = 0;
            }
            trip[0] = 1;
            trip[1] = 1;
            trip[2] = 1;
            //trip[2] = udist(rd);
        }

        ~DistanceFixture() {
            delete data;
        }
};




CELERO_MAIN

BASELINE_F(Distance, Baseline , DistanceFixture, 0, 0) {
    float a;
    float b;
    float c;
    for(int i = 0; i < size; i++) {
        a = data2[0][i] - trip[0];
        b = data2[1][i] - trip[1];
        c = data2[2][i] - trip[2];
        res[i] = round(sqrt(a*a + b*b + c*c));
    }
    celero::DoNotOptimizeAway(res);
}

BENCHMARK_F(Distance, AVX_indices, DistanceFixture, 0, 0) {
    find_32_distance_indices(res, trip, data2[0], data2[1], data2[2]);
    for(size_t i = 0; i < 16; i++) {
        printf("%d: %f %f %f | %d\n", i, 
                data2[0][i],
                data2[1][i],
                data2[2][i],
                res[i]);
    }
    celero::DoNotOptimizeAway(res);
}


