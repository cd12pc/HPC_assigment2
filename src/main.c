#include <stdio.h>
#include <cell_distances.h>

int main() {


    float input[3*16];
    float result[16];
    int i_result[16];
    float base[3] = {1.f, 1.f, 1.f}; 
    
    for(int i = 0; i < 16; ++i){
        input[3*i] = 1*i;
        input[3*i+1] = 1*i;
        input[3*i+2] = 1*i;
        result[i] = 0;
    }

    find_16_distance_indices(i_result, base, input);
    
    for(int i = 0; i < 16; ++i){
        printf("r %f (%d), a %f, b %f, c %f\n",result[i], i_result[i], input[3*i], input[3*i+1], input[3*i+2]);
    }
    // printf() displays the string inside quotation
       //    printf("Hello, World!");
       //       return 0;
       //       
}

