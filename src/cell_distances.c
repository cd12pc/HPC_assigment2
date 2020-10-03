
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cell_distances.h>



int read_block_from_file(int a, float* b, int c, int* d) {
    //READ A BLOCK OF DATA FROM FILE
}



// This function probably doens't need to exist
// Possibly static inline
int find_distance(float* result, float* input) {
    //d*d = a*a + b*b + c*c
    //Compute 100 * distance using vector instructions

    // __mm512_loadu_ps(* mem_adress)
    // __mm512_mul_ps(__m512 a, __m512 b)
    // __mm512_sqrt_ps(__m512 a)
    // __mm512_sqrt_round_ps(__m512 a, int rounding)
}

void find_distrution_from_data(
        unsigned long long* distribution, 
        float* data_to_process,
        int elements_to_process,
        float* trip) 
{
    //#pragma omp parallel for shared(distribution)
    for(size_t i = 0; i < elements_to_process; ++i) {
        int index = find_distance(trip, data_to_process+3*i); 
        //#pragma omp atomic
        distribution[index]++;
    }
}


int find_distrution_in_file(char* file_name) {

    int file_location = 0;
    

    int dist_size = 5000;
    unsigned long long* distribution = (unsigned long long*) calloc(dist_size, sizeof(unsigned long long));

    float trip[3] = {0., 0., 0.};
    
    int max_elements = 10;
    float* data_to_process = (float *) malloc(3*max_elements*sizeof(float));
    int elements_to_process = 0;


    while(read_block_from_file(
                file_location,
                data_to_process,
                max_elements,
                &elements_to_process)) {
        trip[0] = data_to_process[0];
        trip[1] = data_to_process[1];
        trip[2] = data_to_process[2];

        find_distrution_from_data(
                distribution, 
                data_to_process,
                elements_to_process,
                trip);
    }

    for(int i = 0; i < dist_size; ++i) {
        //PRINT RESULTS
    }
}
