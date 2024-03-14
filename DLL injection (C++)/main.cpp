#include "inject.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "usage: \"" << argv[0] << "\" [PID]" << std::endl;
        return 0;
    }

    if (!DLLInjection(
                atoi(argv[1]), 
                DLL, 
                sizeof(DLL) 
    )) {
        std::cerr << "[/] Injection failed." << std::endl;
        return 0;
    }

    std::cout << "[+] Injection done :3" << std::endl;
    return EXIT_SUCCESS;

}
