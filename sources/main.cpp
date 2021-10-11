#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <mutex>
#include <numeric>
#include <pthread.h>
#include <sstream>
#include <string>

//#define MAX_THREADS            2
//#define BLOCK_LINE_COUNT 5147315

#define MAX_THREADS             1
#define BLOCK_LINE_COUNT 10294630

enum {PU_LOCATION = 7, DO_LOCATION, PAYMENT_TYPE};

typedef struct threadParam {
    int         t_id;
    std::string t_filename;
} threadParam;

std::mutex m;
int pu_total(0), do_total(0);
void StoreResults(int pu_count, int do_count) {
    const std::lock_guard<std::mutex> lock(m);
    pu_total += pu_count;
    do_total += do_count;
}

void GotoLine(std::ifstream& file, int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
}

void * ReadFile(void * param) {
    threadParam *params = (threadParam*) param;

    int init_line_pos = params->t_id * BLOCK_LINE_COUNT;
    std::string line;
    std::ifstream file(params->t_filename);

    if (file.is_open()) {
        GotoLine(file, init_line_pos);

        int i(0);
        long int pu_location(-1), do_location(-1), payment_type(-1);
        bool pu_set(false), do_set(false), payment_set(false);
        int pu_count(0), do_count(0);
        for (i=0; std::getline(file, line) && i <= BLOCK_LINE_COUNT; i++) {
            std::stringstream lineStream(line);
            std::string       cell;
            pu_set          = false;
            do_set          = false;
            payment_set     = false;
            for(int j=0; std::getline(lineStream, cell, ','); j++) {
                switch(j) {
                    case PU_LOCATION:  pu_location  = strtol(cell.c_str(), NULL, 0); pu_set      = true; break;
                    case DO_LOCATION:  do_location  = strtol(cell.c_str(), NULL, 0); do_set      = true; break;
                    case PAYMENT_TYPE: payment_type = strtol(cell.c_str(), NULL, 0); payment_set = true; break;
                    default: continue;
                }
                if (pu_set && do_set && payment_set)
                    break;
            }

            if (payment_type == 3) {
                if (pu_location == 170)
                    pu_count++;
                else if (do_location == 170)
                    do_count++;
            }
        }
        StoreResults(pu_count, do_count);

        file.close();
    } else {
        std::cout << "Cannot open file" << std::endl;
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    pthread_t   threads[MAX_THREADS];
    threadParam mTD[MAX_THREADS];

    if ( argc != 2 ) {
        std::cout << "Usage: " << std::endl << "    ./file <filename>" << std::endl;
        return -1;
    }

    for( int j=0, rc(0); j<MAX_THREADS; j++ ) {
        mTD[j].t_id       = j;
        mTD[j].t_filename = std::string(argv[1]);
        rc = pthread_create(&threads[j], NULL, ReadFile, (void*)&mTD[j]);
        if (rc) {
            std::cout << "Unable to create thread: " << rc << std::endl;
            return -1;
        }
        pthread_join(threads[j], NULL);
    }

    std::cout << pu_total << std::endl;
    std::cout << do_total << std::endl;

    pthread_exit(NULL);
}
