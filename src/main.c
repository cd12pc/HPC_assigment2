#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
#include <cell_distances.h>

int main(int argc, char *argv[]) {
    char* file_name;
    int n_threads;
    size_t t_index;
    if(argc == 2)
    {
        if(strncmp(argv[1], "-t", 2) != 0)
        {
            printf("Must include flag for number of threads (e.g. -t10)\n");
            return -1;
        }
        t_index = 1;
        file_name = "./data/cell";
    }
    else if(argc == 3)
    {
        size_t file_index;
        if(strncmp(argv[1], "-t", 2) == 0)
        {
            t_index = 1;
            file_index = 2;
        }
        else if(strncmp(argv[2], "-t", 2) == 0)
        {
            t_index = 2;
            file_index = 1;
        }
        else
        {
            printf("Must include flag for number of threads (e.g. -t10)\n");
            return -1;
        }

        file_name = argv[file_index];
    }
    else
    {
        printf("\nThe script requires a parameter for the number of threads, e.g. \"-t10\"\n");
        printf("An additional argument can be given for an input file, e.g. \"./data/test_data/cell_50\"\n");
        printf("If no such argument is given, the default file is \"./data/cell\"\n");
        printf("\n");
        printf("The final command should look something like this: \"bin/run.a ./data/test_data/cell_50 8\"\n");
        return -1;
    }
    char* n_threads_str = argv[t_index];
    n_threads = atoi(n_threads_str + 2);
    printf("%s\n", file_name);
    printf("%d\n", n_threads);
    if(access(file_name, F_OK) != -1)
    {
        // file exists
        omp_set_dynamic(0);  // Explicitly disable dynamic teams
        omp_set_num_threads(n_threads);
        return find_distrution_in_file(file_name);
    }
    else
    {
        printf("The file \"%s\" does not exists\n", file_name);
    }
}
