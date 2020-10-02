//#ifdef BENCHMARK

#include <cstring>
#include "celero/Celero.h"
#include <iostream>

constexpr size_t szs = 50;
constexpr size_t szb = 1000;

constexpr size_t nmbsmpl = 10;
constexpr size_t nmbiter = 4;

constexpr size_t size = (1 << 20);

const char *  file_ssd = "/run/mount/scratch/hpcuser228/data.bin";
const char *  file_hdd = "data.bin"; 

class FileDataFixture : public celero::TestFixture
{
    public:
        int * data;
        FILE * fptr;

        FileDataFixture() {
//            printf("Size: %d\n", size);
            data = (int *) malloc(size*sizeof(int));
            for(int i = 0; i < size; ++i)  {
                data[i] = i;
            }
        }

        ~FileDataFixture() {
            free(data);
        }

};

class HddWriteFixture : public FileDataFixture {
    public:
        void setUp(const celero::TestFixture::ExperimentValue&) override {
            fptr = fopen(file_hdd,"w");
            if(fptr == NULL) {
                printf("Failure to open %s", file_hdd);
                exit(1);
            }
        }

        void tearDown() override {
            fclose(fptr);
        }
};

class HddReadFixture : public FileDataFixture {
    public:
        void setUp(const celero::TestFixture::ExperimentValue&)  override{
            fptr = fopen(file_hdd,"r");
            if(fptr == NULL) {
                printf("Failure to open %s", file_hdd);
                exit(1);
            }
        }

        void tearDown()  override{
            fclose(fptr);
        }
};


class SsdWriteFixture : public FileDataFixture {
    public:
        void setUp(const celero::TestFixture::ExperimentValue&)  override{
            fptr = fopen(file_ssd,"w");
            if(fptr == NULL) {
                printf("Failure to open %s", file_ssd);
                exit(1);
            }
        }

        void tearDown() override {
            fclose(fptr);
        }
};

class SsdReadFixture : public FileDataFixture {
    public:
        void setUp(const celero::TestFixture::ExperimentValue&) override {
            fptr = fopen(file_ssd,"r");
            if(fptr == NULL) {
                printf("Failure to open %s", file_ssd);
                exit(1);
            }
        }

        void tearDown()  override{
            fclose(fptr);
        }
};


CELERO_MAIN

BASELINE_F(Write, hdd_all, HddWriteFixture, nmbsmpl, nmbiter)
{
    fwrite(this->data, sizeof(int), size, this->fptr);
    fflush(fptr);
}

BENCHMARK_F(Write, hdd_part, HddWriteFixture, nmbsmpl, nmbiter)
{

    for(int i = 0; i < size; ++i) {
        fwrite(this->data + i, sizeof(int), 1,this->fptr);
        fflush(fptr);
    }
}

BENCHMARK_F(Write, sdd_all, SsdWriteFixture, nmbsmpl, nmbiter)
{
    fwrite(this->data, sizeof(int), size, this->fptr);
    fflush(fptr);
}

BENCHMARK_F(Write, sdd_part, SsdWriteFixture, nmbsmpl, nmbiter)
{
    for(int i = 0; i < size; ++i) {
        fwrite(this->data + i, sizeof(int), 1, this->fptr);
        fflush(fptr);
    }
}

BASELINE_F(Read, hdd_all, HddReadFixture, nmbsmpl, nmbiter) {
    fread(this->data, sizeof(int), size, this->fptr);
}

BENCHMARK_F(Read, hdd_part, HddReadFixture, nmbsmpl, nmbiter) {
    for(int i = 0; i < size; ++i) {
        fread(this->data + i, sizeof(int), 1, this->fptr);
    }
}

BENCHMARK_F(Read, ssd_all, SsdReadFixture, nmbsmpl, nmbiter) {
    fread(this->data, sizeof(int), size, this->fptr);
}

BENCHMARK_F(Read, ssd_part, SsdReadFixture, nmbsmpl, nmbiter) {
    for(int i = 0; i < size; ++i) {
        fread(this->data + i, sizeof(int), 1, this->fptr);
    }
}

