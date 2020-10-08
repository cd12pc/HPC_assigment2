
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cell_distances.h>
#include <immintrin.h>


int read_block_from_file(FILE* fp, int num_floats, int* elem_to_process, float* coords, int* end_of_file) {
  
  //FILE *fp = fopen(file_name, "r");

  //if (fp == NULL) {
  //  printf("Error opening file\n");
  //  exit(1);
  //}

  //fseek(fp, 0, SEEK_END);
  //*file_size = ftell(fp);
  //fseek(fp, saved, SEEK_SET);
  
  int index = 0;
  fseek(fp, -8, SEEK_CUR);

  while( fscanf(fp,"%f ", &coords[index]) != EOF && index < num_floats ) {
    index++;
  }
 
  if (feof(fp)) {
    *end_of_file = 1;
  }
  //saved = ftell(fp);
  //fclose(fp);
  *elem_to_process = index;
  
  return *end_of_file;
}




void find_16_distance_indices(int32_t* result, float* base, float* input) {

    __m512 _a, _a_off, _a2; // First index
    __m512 _b, _b_off, _b2; // Second index
    __m512 _c, _c_off, _c2; // Third index
    __m512 _r, _r2, _r_t100; // Results
    __m512i _index;

    const int32_t a_index_arg[16] = {0,3,6,9 ,12,15,18,21,24,27,30,33,36,39,42,45};
    const int32_t b_index_arg[16] = {1,4,7,10,13,16,19,22,25,28,31,34,37,40,43,46};
    const int32_t c_index_arg[16] = {2,5,8,11,14,17,20,23,26,29,32,35,38,41,44,47}; //Should go to 16*3 - 1 = 47

    __m512i _a_index = _mm512_load_epi64(a_index_arg);
    __m512i _b_index = _mm512_load_epi64(b_index_arg);
    __m512i _c_index = _mm512_load_epi64(c_index_arg); //Converting them to __m512 indicies

    _a = _mm512_i32gather_ps(_a_index, input, sizeof(float)); //Loading data into a
    _b = _mm512_i32gather_ps(_b_index, input, sizeof(float)); //Loading data into b
    _c = _mm512_i32gather_ps(_c_index, input, sizeof(float)); //Loading data into c

    _a_off = _mm512_set1_ps(base[0]); // a_off = the a coordinate of the other point
    _b_off = _mm512_set1_ps(base[1]); // b_off = the b coordinate of the other point
    _c_off = _mm512_set1_ps(base[2]); // c_off = the c coordinate of the other point
    
    //Actual computations start here 
    _a = _mm512_sub_ps(_a, _a_off); // a = a - a_off
    _b = _mm512_sub_ps(_b, _b_off); // b = b - b_off
    _c = _mm512_sub_ps(_c, _c_off); // c = c - c_off

    // Squaring
    _a2 = _mm512_mul_ps(_a, _a); // a^2 = a * a
    _b2 = _mm512_mul_ps(_b, _b); // b^2 = b * b
    _c2 = _mm512_mul_ps(_c, _c); // c^2 = c * c

    // Addition of squared
    _r2 = _mm512_add_ps(_a2, _b2); //r^2 = a^2 + b^2
    _r2 = _mm512_add_ps(_r2, _c2); //r^2 = c^2 + (a^2 + b^2)
    // Square root
    _r = _mm512_sqrt_ps(_r2); // r = sqrt(r^2)

    //Multiplication of PRECISION and rounding to nearest int
    _r_t100 = _mm512_mul_round_ps(_r, _mm512_set1_ps(PRECISION), _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC );
    //Converting to indicies
    _index = _mm512_cvtps_epi32(_r_t100);
    _mm512_storeu_epi32(result, _index); // Store data in results
}



void find_distrution_from_data(
        unsigned long long* distribution, 
        float* data_to_process,
        int elements_to_process,
        float* trip)
{

    int32_t indicies[16];
    //OLD:
    // #pragma omp parallel for reduction(+:distribution[:]) shared(trip, elements_to_process) private(indicies) 
    #pragma omp parallel for shared(trip, elements_to_process) private(indicies) 
    for(size_t i = 0; i < elements_to_process; i += ELEM_PER_BLOCK) {

        find_16_distance_indices(indicies,trip, data_to_process + 3*i); 

        #pragma omp atomic
        distribution[indicies[0]]++;
        #pragma omp atomic
        distribution[indicies[1]]++;
        #pragma omp atomic
        distribution[indicies[2]]++;
        #pragma omp atomic
        distribution[indicies[3]]++;
        #pragma omp atomic
        distribution[indicies[4]]++;
        #pragma omp atomic
        distribution[indicies[5]]++;
        #pragma omp atomic
        distribution[indicies[6]]++;
        #pragma omp atomic
        distribution[indicies[7]]++;
        #pragma omp atomic
        distribution[indicies[8]]++;
        #pragma omp atomic
        distribution[indicies[9]]++;
        #pragma omp atomic
        distribution[indicies[10]]++;
        #pragma omp atomic
        distribution[indicies[11]]++;
        #pragma omp atomic
        distribution[indicies[12]]++;
        #pragma omp atomic
        distribution[indicies[13]]++;
        #pragma omp atomic
        distribution[indicies[14]]++;
        #pragma omp atomic
        distribution[indicies[15]]++;
    }
    int rem = elements_to_process % ELEM_PER_BLOCK;

    int32_t indicies2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    find_16_distance_indices(indicies2, trip, data_to_process + 3*(elements_to_process - rem));

    for(size_t i = 0; i < rem; ++i) {
        distribution[indicies[i]]++;
    }

}


 int find_distrution_in_file(FILE* file_name) {

    int file_location = 0;
    unsigned long long* distribution = (unsigned long long*) calloc(DIST_SIZE, sizeof(unsigned long long));

    float trip[3] = {0., 0., 0.};
    int elements_to_process = 0;
    int end_of_file = 0;
    //long saved;
    float* data_to_process = (float *) malloc(FLOATS_IN_MEMORY * sizeof(float));

    while(!read_block_from_file(
                file_name,
		FLOATS_IN_MEMORY,
		&elements_to_process,
		data_to_process,
                &end_of_file)) {
        trip[0] = data_to_process[0];
        trip[1] = data_to_process[1];
        trip[2] = data_to_process[2];

        find_distrution_from_data(
                distribution, 
                data_to_process+3,
                elements_to_process,
                trip);
    }

    float distance, freq;
    for(int i = 0; i < DIST_SIZE; ++i) {
        distance = i / 100.f;
        freq = distribution[i];
        if(distance > 0) 
            printf("%f %d\n", distance, freq);
    }
}
