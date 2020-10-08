//#ifdef BENCHMARK
#include <random>
#include <cstring>
#include "celero/Celero.h"
#include <iostream>

#include <cell_distances.h>

constexpr size_t szs = 50;
constexpr size_t szb = 1000;

constexpr size_t size = 16;

auto rd = std::default_random_engine();
auto udist = std::uniform_real_distribution<float> (-10.,10.);

class DistanceFixture : public celero::TestFixture
{
    public:
        int32_t* data;
        int32_t trip[3];
        int32_t res[16];

        DistanceFixture() {
            data = new int32_t[size * 3]();
        }


        virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) {
            for(int i = 0; i < size; i++) {
                data[3 * i] = 2;
                data[3 * i + 1] = 2;
                data[3 * i + 2] = 2;
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
        a = data[3*i] - trip[0];
        b = data[3*i + 1] - trip[1];
        c = data[3*i + 2] - trip[2];
        res[i] = round(sqrt(a*a + b*b + c*c));
    }
    celero::DoNotOptimizeAway(res);
}

BENCHMARK_F(Distance, AVX_indices, DistanceFixture, 0, 0) {
    find_16_distance_indices(res, trip, data);
    for(size_t i = 0; i < 16; i++) {
        printf("%d: %f %f %f | %d\n", i, 
                data[3*i],
                data[3*i+1],
                data[3*i+2],
                res[i]);
    }
    celero::DoNotOptimizeAway(res);
}


