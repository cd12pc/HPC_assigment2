
#ifndef _INCLUDE_cell_distance
#define _INCLUDE_cell_distance

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

static const int SPACE_WIDTH = 20;
static const int PRECISION = 100;

static const int DIST_SIZE = (int) (sqrt(3) * SPACE_WIDTH) + 10; // Has to be larger than sqrt(3) * 20 * precision (In the task = 3464)

static const int NUM_BLOCKS = 10;
static const int ELEM_PER_BLOCK = 16;
static const int FLOATS_PER_ELEM = 3;
static const int FLOATS_PER_BLOCK = FLOATS_PER_ELEM * ELEM_PER_BLOCK;


static const int ELEM_IN_MEMORY = NUM_BLOCKS * ELEM_PER_BLOCK + 1;
static const int FLOATS_IN_MEMORY = ELEM_IN_MEMORY * FLOATS_PER_ELEM;

void find_16_distance_indices(int32_t* result, float* base, float* input);

#ifdef __cplusplus
}
#endif

#endif


