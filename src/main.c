#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <cell_distances.h>

int main(int argc, char *argv[]) {
    if( argc == 3 )
    {
        char* file_name = argv[1];
        if(access(file_name, F_OK) != -1)
        {
            // file exists
            int n_threads = atoi(argv[2]);
            omp_set_dynamic(0);  // Explicitly disable dynamic teams
            omp_set_num_threads(n_threads);
            return find_distrution_in_file(file_name);
        }
        else
        {
            printf("The file \"%s\" does not exists\n", file_name);
        }
    }
    else
    {
        printf("\nThe script requires two positional arguments:\n");
        printf("An input file, e.g. \"./data/test_data/cell_50\"\n");
        printf("A thread count, e.g. \"8\".\n");
        printf("\n");
        printf("The final command should look something like this: \"bin/run.a ./data/test_data/cell_50 8\"\n");
        printf("Floats: %d\n", FLOATS_IN_MEMORY);
        printf("Bytes %d\n", FLOATS_IN_MEMORY*32);
        printf("GB %f\n", FLOATS_IN_MEMORY*32 / pow(2,30));
    }

    return -1; 
}
