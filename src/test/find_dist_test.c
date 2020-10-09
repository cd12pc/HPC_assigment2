#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <cell_distances.h>
#include <stdint.h>
#include <omp.h>


void test_distance_index(int n_elements) {
    
    double _blocks = ceil(n_elements / (1.0* FLOATS_PER_CHUNK));
    int blocks = _blocks;

    int elem_stored = blocks * FLOATS_PER_CHUNK;
    int all_floats = blocks *  FLOATS_PER_CHUNK * CHUNKS_PER_BLOCK;
    
    printf("%d\n", all_floats);
    printf("%d\n", blocks);
    int16_t * data = (int16_t *) malloc(all_floats * sizeof(int16_t));
    int16_t ** data2 = (int16_t **) malloc(CHUNKS_PER_BLOCK * blocks * sizeof(int16_t));
    for(int i = 0; i < blocks; ++ i) {
      data2[3*i] = data + 96 * i;
      data2[3*i+1] = data + 96 * i + 32;
      data2[3*i+2] = data + 96 * i + 64;
    }

    uint64_t * dist = (uint64_t *) malloc(DIST_SIZE * sizeof(uint64_t));
    for(size_t ix = 0; ix < DIST_SIZE; ix++) {
        dist[ix] = 0;
    }

    if(data2 == NULL){
        printf("ERROR");
    }
    if(data == NULL){
        printf("ERROR");
    }
    if(dist == NULL) {
        printf("ERROR");
    }

  //  printf("DATA: %p -> %p\n", data, data+all_floats-1);
  //  printf("DIST: %p -> %p\n", dist, dist+DIST_SIZE-1);
  //
    int16_t ref[3];
    
    ref[0] = 1;
    ref[1] = 1;
    ref[2] = 1;

    for(int i = 0; i < n_elements; ++i) {
        int h = i / FLOATS_PER_CHUNK;
        int v = i % FLOATS_PER_CHUNK;

        data2[h][v] = 2;
        data2[h+1][v] = 2; 
        data2[h+2][v] = 2;
    }


    for(int i = n_elements; i < elem_stored; ++i) {
        int h = i / FLOATS_PER_CHUNK;
        int v = i % FLOATS_PER_CHUNK;

        data2[h][v] = 10;
        data2[h+1][v] = 10; 
        data2[h+2][v] = 10;
    }

    for(int i = 0; i < elem_stored; ++i) {
        int h = i / FLOATS_PER_CHUNK;
        int v = i % FLOATS_PER_CHUNK;
        //printf("%d [%d][%d] = %d | %d | %d\n", i, h, v, data2[h][v], data2[h+1][v], data2[h][v]);
    }

    //printf("CPB: %d", FLOATS_PER_CHUNK);
    find_distrution_from_data(dist, data2, 0, n_elements, ref);    

    for(size_t i = 0; i < DIST_SIZE; ++i) {
        if(i == 173) {
            TEST_ASSERT_EQUAL_UINT64(dist[i], n_elements);
        } else {
            TEST_ASSERT_EQUAL_UINT64(dist[i], 0);
        }        
    }

    free(dist);
    free(data);
}


void test_0_50_elem(void) {
    for(int i = 1; i < 50; i++) {
        test_distance_index(i);
    }
}

void test_20_random_elem(void) {
    for(int i = 0; i < 10; i++) {
        int r = 1 + rand() % 1000;
        test_distance_index(r);
    }
}

void setUp(void) {
    // set stuff up here

}

void tearDown(void) {
    // clean stuff up here
    // 
}

int main() {
    UNITY_BEGIN();
    omp_set_num_threads(1);
    RUN_TEST(test_0_50_elem);
    RUN_TEST(test_20_random_elem);

    /*test_distance_index(10);

      test_distance_index(11);
      test_distance_index(12);
      test_distance_index(13);
      */ // printf() displas the string inside quotation
    //    printf("Hello, World!");
    //       return 0;
    return UNITY_END();      //       
}

