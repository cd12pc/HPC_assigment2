#ifndef _INCLUDE_cell_distance
#define _INCLUDE_cell_distance

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

static const int SPACE_WIDTH = 20;
static const int PRECISION = 100;

static const int32_t DIST_SIZE = (int32_t) 1000*(sqrt(3) * SPACE_WIDTH) + 10; // Has to be larger than sqrt(3) * 20 * precision (In the task = 3464)


static const int NUM_BLOCKS = 3000; //Slower with 1000
static const int CHUNKS_PER_BLOCK = 3;
static const int FLOATS_PER_CHUNK = 32;
static const int BYTES_PER_ELEM = 24;


static const int ELEM_IN_MEMORY = NUM_BLOCKS * FLOATS_PER_CHUNK;

static const int CHUNKS_IN_MEMORY = NUM_BLOCKS * CHUNKS_PER_BLOCK;
static const int FLOATS_IN_MEMORY = CHUNKS_IN_MEMORY * FLOATS_PER_CHUNK;


//void find_32_distance_indices(int16_t * result, int16_t* base, int16_t* a, int16_t * b, int16_t* c);

int find_distrution_in_file(char* file_name);

void find_distrution_from_data(
        uint64_t * distribution, 
        int16_t ** chunks_to_process,
        int end_elem,
        int16_t* trip);

void find_distrution_from_data_start(
        uint64_t * distribution, 
        int16_t ** chunks_to_process,
        int start_elem,
        int end_elem,
        int16_t* trip);
#ifdef __cplusplus
}
#endif
#endif
