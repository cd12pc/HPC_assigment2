#ifndef _INCLUDE_cell_distance
#define _INCLUDE_cell_distance

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

static const int SPACE_WIDTH = 20;
static const int PRECISION = 100;

static const int32_t DIST_SIZE = (int32_t) PRECISION*PRECISION*(3 * SPACE_WIDTH * SPACE_WIDTH) + 10; // Has to be larger than sqrt(3) * 20 * precision (In the task = 3464)

static const int NUM_SUPB = 3000;
static const int BLOCK_PER_SUPB = 100; 

static const int NUM_BLOCKS = NUM_SUPB * BLOCK_PER_SUPB;
static const int ELEM_PER_BLOCK = 16;
static const int FLOATS_PER_ELEM = 3;
static const int FLOATS_PER_BLOCK = FLOATS_PER_ELEM * ELEM_PER_BLOCK;


static const int ELEM_IN_MEMORY = NUM_BLOCKS * ELEM_PER_BLOCK + 1;
static const int FLOATS_IN_MEMORY = ELEM_IN_MEMORY * FLOATS_PER_ELEM;

void find_16_distance_indices(int32_t * result, int32_t* base, int32_t* input);

int find_distrution_in_file(char* file_name);

void find_distrution_from_data(
        uint64_t* distribution, 
        int32_t* data_to_process,
        int elements_to_process,
        int32_t* trip);

#ifdef __cplusplus
}
#endif
#endif
