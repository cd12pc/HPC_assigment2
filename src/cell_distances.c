
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cell_distances.h>
#include <immintrin.h>
#include <math.h>

int read_block_from_file(char* file_name, 
    long* file_location,
    int num_floats, 
    int* elem_to_process, 
    int32_t* coords, 
    int* end_of_file) {
  *end_of_file = 0;


  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    printf("Error opening file (%s)\n", fp);
    exit(1);
  }
  //  printf("Moving to: %d\n", *file_location);
  fseek(fp, *file_location, SEEK_SET);
  if(feof(fp)) {
    *end_of_file = 1;
    return 0;
  }
  fseek(fp, -8, SEEK_CUR);


  char* char_data = (char *) malloc(FLOATS_IN_MEMORY * sizeof(char));
  size_t read = fread(char_data, sizeof(char), FLOATS_IN_MEMORY, fp);

  const int chunk = 400;
  #pragma omp parallel for shared(coords) 
  for(size_t j_sec = 0; j_sec < read; j_sec += chunk) {
    
    for(size_t i_f = j_sec; i_f < j_sec + chunk; i_f += 8) {
  
      char v = char_data[i_f];
      int32_t a = 1000 * (char_data[i_f+1] - '0');
      a += 100 *     (char_data[i_f+2] - '0');
      a += 10 *   (char_data[i_f + 4] - '0');
      a += 1 *  (char_data[i_f + 5] - '0');
      //a += (char_data[i_f + 6] - '0');
      if(v == '-') 
        a = -a;
      coords[i_f / 8] = a;
    }
  }

  /*
  for(size_t i_f = 0; i_f < read; i_f += 8) {
    printf("%c %c %c", char_data[i_f], char_data[i_f+1]);
  }*/
  *elem_to_process = read / (8 * 3);

  *file_location = ftell(fp);
  if (feof(fp)) {
    *end_of_file = 1;
//    printf("EOF: %d\n", *file_location);
  }
  fclose(fp);
 
  free(char_data);
  return 1;
}

static inline int find_distance(int a, int a_off, int b, int b_off, int c, int c_off) {

  float aV = a - a_off;
  float bV = b - b_off;
  float cV = c - c_off;
        //printf("r: %f\n",r);
  return (int) roundf(PRECISION*sqrtf(aV*aV + bV*bV + cV*cV));
}


void find_16_distance_indices(int32_t * result, int32_t* base, int32_t* input) {
//#ifdef _DEBUG_BUILD
    //printf("DIST\n");
    __m512i _a, _a_off, _a2; // First index
    __m512i _b, _b_off, _b2; // Second index
    __m512i _c, _c_off, _c2; // Third index
    __m512i _r, _r2, _r_t100; // Results
    __m512i _index;

    const uint32_t a_index_arg[16] = {0,3,6,9 ,12,15,18,21,24,27,30,33,36,39,42,45};
    const uint32_t b_index_arg[16] = {1,4,7,10,13,16,19,22,25,28,31,34,37,40,43,46};
    const uint32_t c_index_arg[16] = {2,5,8,11,14,17,20,23,26,29,32,35,38,41,44,47}; //Should go to 16*3 - 1 = 47

    __m512i _a_index = _mm512_load_epi32(a_index_arg);
    __m512i _b_index = _mm512_load_epi32(b_index_arg);
    __m512i _c_index = _mm512_load_epi32(c_index_arg); //Converting them to __m512 indicies

    _a = _mm512_i32gather_epi32(_a_index, input, sizeof(int32_t)); //Loading data into a
    _b = _mm512_i32gather_epi32(_b_index, input, sizeof(int32_t)); //Loading data into b
    _c = _mm512_i32gather_epi32(_c_index, input, sizeof(int32_t)); //Loading data into c

    _a_off = _mm512_set1_epi32(base[0]); // a_off = the a coordinate of the other point
    _b_off = _mm512_set1_epi32(base[1]); // b_off = the b coordinate of the other point
    _c_off = _mm512_set1_epi32(base[2]); // c_off = the c coordinate of the other point
    
    //Actual computations start here 
    _a = _mm512_sub_epi32(_a, _a_off); // a = a - a_off
    _b = _mm512_sub_epi32(_b, _b_off); // b = b - b_off
    _c = _mm512_sub_epi32(_c, _c_off); // c = c - c_off

    // Squaring
    _a2 = _mm512_mullo_epi32(_a, _a); // a^2 = a * a
    _b2 = _mm512_mullo_epi32(_b, _b); // b^2 = b * b
    _c2 = _mm512_mullo_epi32(_c, _c); // c^2 = c * c

    // Addition of squared
    _r2 = _mm512_add_epi32(_a2, _b2); //r^2 = a^2 + b^2
    _r2 = _mm512_add_epi32(_r2, _c2); //r^2 = c^2 + (a^2 + b^2)
    
    __m512i div = _mm512_set1_epi32(2^32);
    //_r2 = _mm512_mullo_epi32(_r2, div);

    // Square root
    _mm512_storeu_epi32(result, _r2); // Store data in results
/*#else
    float a_off = base[0];
    float b_off = base[1];
    float c_off = base[2];

    for(size_t i = 0; i < 16; ++i) {
        float a = input[3*i] - a_off;
        float b = input[3*i + 1] - b_off;
        float c = input[3*i + 2] - c_off;
        //printf("r: %f\n",r);
        result[i] = (size_t) roundf(PRECISION*sqrtf(a*a + b*b + c*c));

    }

#endif
*/

}



void find_distrution_from_data(
        uint64_t * distribution, 
        int32_t * data_to_process,
        int elements_to_process,
        int32_t* trip)
{

    int blocks_to_process = elements_to_process / ELEM_PER_BLOCK; 

    //printf("DISTZISE: %d\n", DIST_SIZE);
    int32_t indicies[16] = {0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0};
    //OLD:
    // #pragma omp parallel for reduction(+:distribution[:]) shared(trip, elements_to_process) private(indicies) 
    #pragma omp parallel for shared(trip, elements_to_process) private(indicies) 
    for(size_t i_block = 0; i_block < blocks_to_process; ++i_block) {
      
      find_16_distance_indices(
                indicies,
                trip,
                data_to_process + FLOATS_PER_BLOCK * i_block); 

        int32_t* currLoc = data_to_process + FLOATS_PER_BLOCK * i_block; 
        for(int i = 0; i < 16; ++i) {
          printf("(%d)%d - %d| %d - %d | %d - %d= %d \n", 
                i,  
                currLoc[3*i], trip[0],
                currLoc[3*i+1], trip[1],
                currLoc[3*i+2], trip[2],
                indicies[i]);
        }

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

    find_16_distance_indices(
        indicies, 
        trip, 
        data_to_process + 3*(elements_to_process- rem));
    for(size_t i = 0; i < rem; ++i) {
        distribution[indicies[i]]++;
    }

/*
    for(size_t i = 0; i < rem; ++i) {

        int a = data_to_process[3*(elements_to_process - rem + i)];
        int b = data_to_process[3*(elements_to_process - rem + i) +1];
        int c = data_to_process[3*(elements_to_process - rem + i) +2];
        
        int index = find_distance(a, trip[0], b, trip[1], c, trip[2]);
        distribution[index]++;
    }
*/
}


 int find_distrution_in_file(char* file_name) {
    long start_location = 0;
    long file_location = 0;

    uint64_t* distribution = (uint64_t*) calloc(DIST_SIZE, sizeof(uint64_t));

    int32_t trip[3] = {0, 0, 0};
    int elements_to_process = 0;
    int end_of_file = 0;
    int32_t* data_to_process = (int32_t *) malloc(FLOATS_IN_MEMORY * sizeof(int32_t));


    while(read_block_from_file(
        file_name,
        &file_location,
		FLOATS_IN_MEMORY,
		&elements_to_process,
        data_to_process,
        &end_of_file)) {

      /*for(int i = 0; i < ELEM_PER_BLOCK+1; i++) {
          printf("(1) %d = %f | %f | %f\n", i,
              data_to_process[3*i], 
              data_to_process[3*i + 1], 
              data_to_process[3*i + 2]);
      }*/

      //printf("1: sl %d\n", start_location);
      //printf("1 : fl %d\n", file_location);
      if(end_of_file) {
        for(int ie = 0; ie < elements_to_process; ++ ie) {
          trip[0] = data_to_process[3*ie];
          trip[1] = data_to_process[3*ie+1];
          trip[2] = data_to_process[3*ie+2];

          find_distrution_from_data(
              distribution, 
              data_to_process+3*(1+ie),
              elements_to_process-(1+ie),
              trip);
        }
        break;
      }

      trip[0] = data_to_process[0];
      trip[1] = data_to_process[1];
      trip[2] = data_to_process[2];

      find_distrution_from_data(
          distribution, 
          data_to_process+3,
          elements_to_process-1,
          trip);

      while(!end_of_file && read_block_from_file(
          file_name,
          &file_location,
          FLOATS_IN_MEMORY,
          &elements_to_process,
          data_to_process,
          &end_of_file)) {

      /*
      for(int i = 0; i < ELEM_PER_BLOCK+1; i++) {
          printf("(2) %d = %f | %f | %f\n", i,
              data_to_process[3*i], 
              data_to_process[3*i + 1], 
              data_to_process[3*i + 2]);
      }*/
      //printf("2 : fl %d\n", file_location);
      //printf("2 : eof %d\n", end_of_file);
        find_distrution_from_data(
          distribution, 
          data_to_process,
          elements_to_process,
          trip);
      }
      start_location++;
      file_location = 24 * start_location + 8;
    }

    float distance, freq;
    for(int i = 0; i < DIST_SIZE; ++i) {
      if(distribution[i] > 0) {
          distance = sqrt(i) / 100.f;
          printf("%.2f %d\n", distance, distribution[i]);
      } 
    }

    return 0;
}
