#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cell_distances.h>
#include <immintrin.h>
#include <math.h>

long int floats_in_memory = 32*3;

static inline int16_t getInt(char * string) {
    char v = string[0];  

    int16_t a = (string[1] - '0') * 10000;
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

    size_t read = 0;
    char* char_data = (char *) malloc((8*floats_in_memory+24) * sizeof(char));
//#pragma critical
 //   {
        FILE *fp = fopen(file_name, "r");
        if (fp == NULL) {
            printf("Error opening file (%s)\n", fp);
            exit(1);
        }

        *end_of_file = 0;

        // Set location place
        fseek(fp, *file_location, SEEK_SET);
        if(feof(fp)) {
            printf("END");
            *end_of_file = 1;
            return ftell(fp);
        }

        //Load character data
//#pragma omp critical
        read = fread(char_data, sizeof(char), 8*floats_in_memory+24, fp);

        *file_location = ftell(fp);
        //printf("END %d\n", file_location);
        if(feof(fp)) {
            *end_of_file = ftell(fp);
        }
        fclose(fp);

//    } //End critical

    first[0] = getInt(char_data);
    first[1] = getInt(char_data + 8);
    first[2] = getInt(char_data + 16);

    *elem_to_process = (read - 24)/ (8 * 3);
    for(size_t i = 0; i < *elem_to_process; ++i) {
//        printf("i %i\n",i);
        int block = 3*(i / FLOATS_PER_CHUNK);
        int loc = i % FLOATS_PER_CHUNK;
        chunks[block][loc] = getInt(char_data + 24*i + 24);
        chunks[block +1][loc] = getInt(char_data + 24*i + 8 + 24);
        chunks[block + 2][loc] = getInt(char_data + 24*i + 16 + 24);
    }

    free(char_data);
    return 1;
}

int read_block_from_file( 
        char* file_name, 
        long* file_location,
        int16_t** chunks, 
        int* elem_to_process, 
        int* end_of_file) {

    char* char_data = (char *) malloc(8*floats_in_memory * sizeof(char));
    size_t read = 0;
//#pragma critical
//    {

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

        read = fread(char_data, sizeof(char), 8*floats_in_memory, fp);

        if(feof(fp)) {
            *end_of_file = 1;
        }
        *file_location = ftell(fp);
        fclose(fp);
//    }//End Critical


    *elem_to_process = read / (8 * 3);
    for(size_t i = 0; i < *elem_to_process; ++i) {
//        printf("i %i\n",i);
        int block = 3*(i / FLOATS_PER_CHUNK);
        int loc = i % FLOATS_PER_CHUNK;
        chunks[block][loc] = getInt(char_data + 24*i);
        chunks[block +1][loc] = getInt(char_data + 24*i + 8);
        chunks[block + 2][loc] = getInt(char_data + 24*i + 16);
    }

    free(char_data);
    return 1;
}



static inline void find_32_distance_indices(
        int16_t * result, 
        int16_t* base, 
        int16_t* a, int16_t * b, int16_t* c) {
#if defined(DEBUG) || defined(PROF)
    for(int i = 0; i < 32; ++i) {
        uint32_t av = a[i] - base[0];
        uint32_t bv = b[i] - base[1];
        uint32_t cv = c[i] - base[2];
        result[i] = (int16_t) (0.1*sqrt(av*av + bv*bv + cv*cv));
    }
#else
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

#endif
    //_mm512_storeu_epi32(result+16, _mm512_cvtps_epi32(_r_uf));// Store data in results
    //*/
}


void find_distrution_from_data(
        uint64_t * distribution, 
        int16_t ** chunks_to_process,
        int end_elem,
        int16_t* trip,
        int16_t p_off)
{
    size_t end_block = end_elem / FLOATS_PER_CHUNK; 
    size_t ev = end_block;
    int16_t indicies[] = {
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };

    //printf("DISTZISE: %d\n", DIST_SIZE);
    for(int i_block = 0; i_block < ev; i_block++) {
        find_32_distance_indices(
                indicies,
                trip,
                *(chunks_to_process + 3*i_block),
                *(chunks_to_process + 3*i_block+1),
                *(chunks_to_process + 3*i_block+2));

        for(int i = 0; i < 32; ++i) {
            distribution[indicies[i]]++;
        }
    }

    find_32_distance_indices(
            indicies,
            trip,
            *(chunks_to_process + 3*ev),
            *(chunks_to_process + 3*ev+1),
            *(chunks_to_process + 3*ev+2));

    int rem = end_elem % FLOATS_PER_CHUNK;
    for(int i = 0; i < rem; ++i){
        distribution[indicies[i]]++;
    }
}



void find_distrution_from_data_start(
        uint64_t * distribution, 
        int16_t ** chunks_to_process,
        int start_elem,
        int end_elem,
        int16_t* trip,
        int16_t p_off)
{



    size_t start_block = start_elem / FLOATS_PER_CHUNK;
    size_t start_elem_in_block = start_elem % FLOATS_PER_CHUNK;
    size_t end_block = end_elem / FLOATS_PER_CHUNK; 
    size_t ev = end_block + 1;
    int increm = 1 + (end_block - start_block) / omp_get_max_threads();

    int16_t indicies[] = {
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };

    for(int j_block = start_block; j_block < ev; j_block += increm) {
        for(int i_block = j_block; i_block < j_block + increm & i_block < ev; i_block++) {
            int loc = 3 * i_block;

            find_32_distance_indices(
                    indicies,
                    trip,
                    *(chunks_to_process + 3 * i_block),
                    *(chunks_to_process + 3 * i_block + 1),
                    *(chunks_to_process + 3 * i_block + 2));

            int start = (start_block == i_block) * start_elem % FLOATS_PER_CHUNK; 
            int end = 0;
            if(end_block != i_block) {
                end = FLOATS_PER_CHUNK;
            } else {
                end = end_elem % FLOATS_PER_CHUNK;
            }

            for(int i = start; i < end; ++i) {
                distribution[indicies[i]]++;
         /*       if(indicies[i] > 3000) {
                    printf("%d | %5d - %5d | %5d - %5d | %5d - %5d = %5d | %2d\n", i + p_off,
                            chunks_to_process[3*i_block][i], trip[0], 
                            chunks_to_process[3*i_block+1][i], trip[1], 
                            chunks_to_process[3*i_block+2][i], trip[2], 
                            indicies[i], distribution[indicies[i]]
                            );
                }*/
            }
        }

    }

}



int find_distrution_in_file(char* file_name) {
    long start_location = 0;

    floats_in_memory = FLOATS_IN_MEMORY / omp_get_num_threads();

    uint64_t* distribution = (uint64_t*) calloc(DIST_SIZE, sizeof(uint64_t));

    FILE* fp = fopen(file_name, "r");
    fseek(fp,0L, SEEK_END);
    long num_elem = ftell(fp) / BYTES_PER_ELEM;
    long num_elem_w_full_blocks = num_elem - floats_in_memory/3 + 1000;
    fclose(fp);

#pragma omp parallel
    {


        int16_t trip[3] = {0, 0, 0};
        int elements_to_process = 0;
        int end_of_file = 0;
        int16_t* data_to_process = (int16_t *) calloc(floats_in_memory, sizeof(int16_t));

        int16_t** chunks_to_process = (int16_t **) calloc(floats_in_memory, sizeof(int16_t *));
        for(int i = 0; i < floats_in_memory; ++i) {
            chunks_to_process[i] = data_to_process + (i * FLOATS_PER_CHUNK);
        }

        uint64_t* l_distribution = (uint64_t*) calloc(DIST_SIZE, sizeof(uint64_t));
//#pragma omp single
//        printf("(0)\n");

#pragma omp for
        for(int curr_elem = 0; curr_elem < num_elem_w_full_blocks; ++curr_elem) {
            long int file_location = curr_elem * BYTES_PER_ELEM;
            read_first_block_from_file(
                    file_name,
                    &file_location,
                    chunks_to_process,
                    trip,
                    &elements_to_process,
                    &end_of_file);

            find_distrution_from_data(
                    l_distribution, 
                    chunks_to_process,
                    elements_to_process,
                    trip, curr_elem+1);

            int l_el = curr_elem;
            while(!end_of_file && read_block_from_file(
                        file_name,
                        &file_location,
                        chunks_to_process,
                        &elements_to_process,
                        &end_of_file)) {

                find_distrution_from_data(
                        l_distribution, 
                        chunks_to_process,
                        elements_to_process,
                        trip, l_el);
                l_el += elements_to_process;
            }
        }


            long int file_location = 0;
            if(num_elem_w_full_blocks > 0)
                file_location = num_elem_w_full_blocks * BYTES_PER_ELEM;

//#pragma omp single
            if(!read_block_from_file(
                file_name,
                &file_location,
                chunks_to_process,
                &elements_to_process,
                &end_of_file))
            {
                printf("ERROR; HASN'T REACHED EOF LOCATION: %d %d\n", file_location, end_of_file);
                exit(1);
            }

#pragma omp for
            for(int ie = 0; ie < elements_to_process; ++ie){
                int block = ie / FLOATS_PER_CHUNK;
                int loc = ie % FLOATS_PER_CHUNK;

                trip[0] = chunks_to_process[3*block][loc];
                trip[1] = chunks_to_process[3*block + 1][loc];
                trip[2] = chunks_to_process[3*block + 2][loc];

                find_distrution_from_data_start(
                        l_distribution,
                        chunks_to_process,
                        ie+1,
                        elements_to_process,
                        trip,
                        num_elem_w_full_blocks + ie
                        );
            }

    
#pragma omp critical
        for(int d = 0; d < DIST_SIZE; ++d) {
            distribution[d] += l_distribution[d];
        }

        free(chunks_to_process);
        free(data_to_process);
        free(l_distribution);
    }

    for(int i = 0; i < DIST_SIZE; ++i) {
        if(distribution[i] != 0)
            printf("%05.2f %d\n", i / 100.f, distribution[i]); 
    }
    free(distribution);

    //    printf("%d\n", DIST_SIZE);


    return 0;
}
