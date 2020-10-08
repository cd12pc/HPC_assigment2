#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <cell_distances.h>
#include <stdint.h>
#include <omp.h>


void test_distance_index(int n_elements) {

    double _blocks = ceil(n_elements / (1.0* ELEM_PER_BLOCK));
    int blocks = _blocks;

    int elem_stored = blocks * ELEM_PER_BLOCK;
    int all_floats = FLOATS_PER_ELEM * elem_stored;
    
    //printf("%d\n", DIST_SIZE * sizeof(unsigned int));
    uint32_t * data = (uint32_t *) malloc(all_floats * sizeof(uint32_t));
    uint64_t * dist = (uint64_t *) malloc(DIST_SIZE * sizeof(uint64_t));
    for(size_t ix = 0; ix < DIST_SIZE; ix++) {
        dist[ix] = 0;
    }



    if(data == NULL){
        printf("ERROR");
    }
    if(dist == NULL) {
        printf("ERROR");
    }

  //  printf("DATA: %p -> %p\n", data, data+all_floats-1);
  //  printf("DIST: %p -> %p\n", dist, dist+DIST_SIZE-1);

    uint32_t ref[3] = {1,1,1};



    for(int i = 0; i < n_elements; ++i) {
        data[3*i] = 2;
        data[3*i + 1] = 2; 
        data[3*i + 2] = 2;
    }

    for(int i = 0; i < DIST_SIZE; ++i) {
        if(dist[i] > 0ULL) {
            printf("%d %d\n", i, dist[i]);//9
        }
    }


    for(int i = n_elements; i < elem_stored; ++i) {
        data[3*i] = 10;
        data[3*i + 1] = 10;
        data[3*i + 2] = 10;
    }
/*
    for(int i = 0; i < elem_stored; ++i) {
        printf("%f | %f | %f\n", data[3*i], data[3*i+1], data[3*i+2]);
    }
*/
    find_distrution_from_data(dist, data, n_elements, ref);    

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
    omp_set_num_threads(0);
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

