
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cell_distances.h>
#include <immintrin.h>
#include <math.h>

static inline int16_t getInt(char * string) {
  char v = string[0];  

  int16_t a;
  a =  (string[1] - '0') * 10000;
  a += (string[2] - '0') * 1000;
  a += (string[4] - '0') * 100;
  a += (string[5] - '0') * 10;
  a += (string[6] - '0');
  if(v == '-') 
    return -a;
  return a;
}

int read_first_block_from_file(
    char* file_name, 
    long* file_location,
    int16_t** chunks, 
    int16_t * first, 
    int* elem_to_process, 
    int* end_of_file) {

  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    printf("Error opening file (%s)\n", fp);
    exit(1);
  }

  *end_of_file = 0;

  // Set location place
  fseek(fp, *file_location, SEEK_SET);
  if(feof(fp)) {
    *end_of_file = 1;
    return 0;
  }
  fseek(fp, -8, SEEK_CUR);

  //Load character data
  char* char_data = (char *) malloc(FLOATS_IN_MEMORY * sizeof(char));
  size_t read = fread(char_data, sizeof(char), FLOATS_IN_MEMORY, fp);

  first[0] = getInt(char_data);
  first[1] = getInt(char_data + 8);
  first[2] = getInt(char_data + 16);

#pragma omp parallel for shared(chunks, char_data)
  for(size_t i = 0; i < read/8 - 1; i++) {
    int v = getInt(char_data + 8 * i + 24);
    //  printf("%d %d %d %d\n", i, v, i % 3 + (i / 96), (i / 3) % FLOATS_PER_CHUNK);
    chunks[i % 3 + 3 *(i  / 96)][(i / 3) % FLOATS_PER_CHUNK] = v;
  }

  *elem_to_process = read / (8 * 3);
  *file_location = ftell(fp);

  if(feof(fp)) {
    *end_of_file = 1;
  }
  fclose(fp);
  free(char_data);
  return 1;
}

int read_block_from_file( 
    char* file_name, 
    long* file_location,
    int16_t** chunks, 
    int* elem_to_process, 
    int* end_of_file) {

  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    printf("Error opening file (%s)\n", fp);
    exit(1);
  }

  // Set location place
  fseek(fp, *file_location, SEEK_SET);
  if(feof(fp)) {
    *end_of_file = 1;
    return 0;
  }
  fseek(fp, -8, SEEK_CUR);

  //Load character data
  char* char_data = (char *) malloc(FLOATS_IN_MEMORY * sizeof(char));
  size_t read = fread(char_data, sizeof(char), FLOATS_IN_MEMORY, fp);

#pragma omp parallel for shared(chunks, char_data)
  for(size_t i = 0; i < read / 8; ++i) {
    int v = getInt(char_data + 8 * i + 24);
    // printf("%d %d %d %d\n", i, v, i % 3 + (i / (3 * FLOATS_PER_CHUNK)), (i / 3) % FLOATS_PER_CHUNK);
    chunks[i % 3 + 3 * (i / 96)][(i / 3) % FLOATS_PER_CHUNK] = v;
  }

  *elem_to_process = read / (8 * 3)-1;
  *file_location = ftell(fp);

  if(feof(fp)) {
    *end_of_file = 1;
  }
  fclose(fp);
  free(char_data);
  return 1;
}

void find_32_distance_indices(int16_t * result, int16_t* base, int16_t* a, int16_t * b, int16_t* c) {

  __m512i _a, _a_off, _a_u, _a_l; // First index
  __m512i _b, _b_off, _b_u, _b_l; // Second index
  __m512i _c, _c_off, _c_u, _c_l; // Third index
  __m512i _r2_l, _r2_u, _r_l, _r_u, _r;
  __m512 _r_lf, _r_uf;

  _a = _mm512_loadu_si512(a); //This is 16 bit memory. Yes it is!!!
  _b = _mm512_loadu_si512(b); //It just loads in memory...
  _c = _mm512_loadu_si512(c); //It is still 16 bit. 16 BIT

  _a_off = _mm512_set1_epi16(base[0]); // a_off = the a coordinate of the other point
  _b_off = _mm512_set1_epi16(base[1]); // a_off = the a coordinate of the other point
  _c_off = _mm512_set1_epi16(base[2]); // a_off = the a coordinate of the other point



  _a = _mm512_sub_epi16(_a, _a_off); // a = a - a_off
  _b = _mm512_sub_epi16(_b, _b_off); // a = a - a_off
  _c = _mm512_sub_epi16(_c, _c_off); // a = a - a_off

  _a = _mm512_abs_epi16(_a);
  _b = _mm512_abs_epi16(_b);
  _c = _mm512_abs_epi16(_c);

  // Divide the 16 bit int into two
  _a_l = _mm512_unpacklo_epi16(_a, _mm512_set1_epi16(0));
  _b_l = _mm512_unpacklo_epi16(_b, _mm512_set1_epi16(0));
  _c_l = _mm512_unpacklo_epi16(_c, _mm512_set1_epi16(0));

  _a_u = _mm512_unpackhi_epi16(_a, _mm512_set1_epi16(0));
  _b_u = _mm512_unpackhi_epi16(_b, _mm512_set1_epi16(0));
  _c_u = _mm512_unpackhi_epi16(_c, _mm512_set1_epi16(0));


  _a_l = _mm512_mullo_epi32(_a_l, _a_l); // a * a
  _a_u = _mm512_mullo_epi32(_a_u, _a_u);    

  _b_l = _mm512_mullo_epi32(_b_l, _b_l); // b * b
  _b_u = _mm512_mullo_epi32(_b_u, _b_u);

  _c_l = _mm512_mullo_epi32(_c_l, _c_l); // c * c
  _c_u = _mm512_mullo_epi32(_c_u, _c_u);

  _r2_l = _mm512_add_epi32(_a_l, _b_l); // r^2 = a*a + b*b
  _r2_u = _mm512_add_epi32(_a_u, _b_u);

  _r2_l = _mm512_add_epi32(_r2_l, _c_l); // r^2 = (a*a + b*b) + c*c
  _r2_u = _mm512_add_epi32(_r2_u, _c_u);

  _r_lf = _mm512_sqrt_ps(_mm512_cvtepi32_ps(_r2_l)); //
  _r_uf = _mm512_sqrt_ps(_mm512_cvtepi32_ps(_r2_u));

  _r_lf = _mm512_mul_round_ps(_r_lf, 
      _mm512_set1_ps(0.1), 
      _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC );
  _r_uf = _mm512_mul_round_ps(_r_uf, 
      _mm512_set1_ps(0.1), 
      _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC );

  _r_l =_mm512_cvtps_epi32(_r_lf);
  _r_u =_mm512_cvtps_epi32(_r_uf);
  //_mm512_storeu_epi32(result, ;// Store data in results
  //_mm512_storeu_epi32(result, _a_l);// Store data in results
  _r = _mm512_packus_epi32(_r_l, _r_u);
  //_mm512_storeu_epi32(result, _r);

  _mm512_storeu_epi32(result, _r);
  //_r_uf = _mm512_mul_round_ps(_r_uf, 
  //    _§mm512_set1_ps(PRECISION), 
  //    _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC );


  //_mm512_storeu_epi32(result+16, _mm512_cvtps_epi32(_r_uf));// Store data in results
}

void find_distrution_from_data(
    uint64_t * distribution, 
    int16_t ** chunks_to_process,
    int start_elem,
    int end_elem,
    int16_t* trip)
{

  size_t start_block = start_elem / FLOATS_PER_CHUNK;
  size_t start_elem_in_block = start_elem % FLOATS_PER_CHUNK;
  size_t end_block = end_elem / FLOATS_PER_CHUNK; 
  size_t ev = end_block + 1;
  //printf("DISTZISE: %d\n", DIST_SIZE);
  int16_t indicies[32] = {0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0};

#pragma omp parallel for shared(trip, chunks_to_process, start_block, end_block, distribution) private(indicies)
  for(int i_block = start_block; i_block < ev; i_block++) {
    int loc = 3 * i_block;
    //printf("(1)%d %d\n", i_block, loc);
    /*
       for(int i = 0; i < 32; ++i) {
       printf("[%d][%d]   %d - %d| %d - %d | %d - %d \n", 
       3*i_block, i, 
       chunks_to_process[3*i_block][i], trip[0],
       chunks_to_process[3*i_block + 1][i], trip[1],
       chunks_to_process[3*i_block + 2][i], trip[2]);
       }
       */
    find_32_distance_indices(
        indicies,
        trip,
        *(chunks_to_process + loc),
        *(chunks_to_process + loc+1),
        *(chunks_to_process + loc+2));

    //  printf("(2)%d\n", i_block);


    //printf("(3)%d\n", i_block);
    int start = (start_block == i_block) * start_elem % FLOATS_PER_CHUNK; 
    int end;
    if(end_block == i_block) {
      end = end_elem % FLOATS_PER_CHUNK;
    } else {
      end = FLOATS_PER_CHUNK;
    }

    /*        for(int i = start; i < end; ++i) {
              printf("[%d][%d]   %d - %d| %d - %d | %d - %d = %d \n", 
              3*i_block, i, 
              chunks_to_process[3*i_block][i], trip[0],
              chunks_to_process[3*i_block + 1][i], trip[1],
              chunks_to_process[3*i_block + 2][i], trip[2],
              indicies[i]);
              }*/
    for(int i = start; i < end; ++i) {
      /*          printf("[%d][%d]   %d - %d| %d - %d | %d - %d = %d \n", 
                  3*i_block, i, 
                  chunks_to_process[3*i_block][i], trip[0],
                  chunks_to_process[3*i_block + 1][i], trip[1],
                  chunks_to_process[3*i_block + 2][i], trip[2],
                  indicies[i]);*/
#pragma omp atomic
    distribution[indicies[i]]++;
    }
  }
}


int find_distrution_in_file(char* file_name) {
  long start_location = 0;
  long file_location = 0;

  uint64_t* distribution = (uint64_t*) calloc(DIST_SIZE, sizeof(uint64_t));

  int16_t trip[3] = {0, 0, 0};
  int elements_to_process = 0;
  int end_of_file = 0;
  int16_t* data_to_process = (int16_t *) malloc(FLOATS_IN_MEMORY * sizeof(int16_t));

  int16_t** chunks_to_process = (int16_t **) malloc(CHUNKS_IN_MEMORY * sizeof(int16_t *));
  for(int i = 0; i < CHUNKS_IN_MEMORY; ++i) {
    chunks_to_process[i] = data_to_process + i * FLOATS_PER_CHUNK;
  }

  while(read_first_block_from_file(
        file_name,
        &file_location,
        chunks_to_process,
        trip,
        &elements_to_process,
        &end_of_file)) {
    printf("(1) %i = %i | %i | %i\n",0,trip[0], trip[1],trip[2]);  
    for(int i = 0; i < elements_to_process-1; i++) {
      printf("(1) %i = %i | %i | %i\n", i+1,
      chunks_to_process[0][i], 
      chunks_to_process[1][i], 
      chunks_to_process[2][i]);
    }
    printf("(1) eof: %i\n", end_of_file);

    //printf("1: sl %d\n", start_location);
    //printf("1 : fl %d\n", file_location);
    if(end_of_file) {
      for(int ie = 0; ie < elements_to_process; ++ ie) {
        find_distrution_from_data(
            distribution, 
            chunks_to_process,
            ie,
            elements_to_process-1,
            trip);

        int block = ie / FLOATS_PER_CHUNK;
        int loc = ie % FLOATS_PER_CHUNK;
        trip[0] = chunks_to_process[3*block][loc];
        trip[1] = chunks_to_process[3*block + 1][loc];
        trip[2] = chunks_to_process[3*block + 2][loc];
      }
      break;
    }

    find_distrution_from_data(
        distribution, 
        chunks_to_process,
        0,
        elements_to_process,
        trip);
    file_location -= 16;
    while(!end_of_file && read_block_from_file(
          file_name,
          &file_location,
          chunks_to_process,
          &elements_to_process,
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
      for(int i = 0; i < elements_to_process; i++) {
	printf("(2) %i = %i | %i | %i\n", i,
		  chunks_to_process[0][i],
		  chunks_to_process[1][i],
		  chunks_to_process[2][i]);
      }
      printf("(2) eof: %i\n", end_of_file);  
      find_distrution_from_data(
          distribution, 
          chunks_to_process,
          0,
          elements_to_process,
          trip);
    }
    start_location++;
    file_location = 24 * start_location + 8;
  }

  float distance, freq;
  for(int i = 0; i < DIST_SIZE; ++i) {
    if(distribution[i] > 0) {
      distance = i / 100.f;
      printf("%.2f %d\n", distance, distribution[i]);
    } 
  }

  return 0;
}
