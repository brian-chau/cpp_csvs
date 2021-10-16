#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

enum {PU_LOCATION = 7, DO_LOCATION, PAYMENT_TYPE};


int main(int argc, char** argv) {
    if ( argc != 2 ) {
        std::cout << "Usage: " << std::endl << "    ./file <filename>" << std::endl;
        return -1;
    }

    std::string line;
    std::ifstream file(argv[1]);

    if (file.is_open()) {
        long int pu_location(-1), do_location(-1), payment_type(-1);
        bool pu_set(false), do_set(false), payment_set(false);
        int pu_count(0), do_count(0);
        for (int i=0; std::getline(file, line); i++) {
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
        std::cout << pu_count << std::endl;
        std::cout << do_count << std::endl;
        file.close();
    } else {
        std::cout << "Cannot open file" << std::endl;
    }
}
