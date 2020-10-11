#include <random>
#include <cstring>
#include "celero/Celero.h"
#include <iostream>
#include <omp.h>
#include <cell_distances.h>

constexpr size_t szs = 50;
constexpr size_t szb = 1000;

constexpr size_t size = 32;
CELERO_MAIN

mASELINE(Distance, Baseline , 0, 0) {
    char* str = "data/test_data/cell_e3";
    omp_set_dynamic(0);  // Explicitly disable dynamic teams
    omp_set_num_threads(1);
    celero::DoNotOptimizeAway(find_distrution_in_file(str));
}

BENCHMARK(Distance, AVX_indices, 0, 0) {
}
