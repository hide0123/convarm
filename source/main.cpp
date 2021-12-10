#include <iostream>

#include "instruction.hpp"

int main(int argc, char *argv[]) {

    int code;

    if(argc != 2) {
        std::cout << "Error: Bad argument." << std::endl;
        return -1;
    }

    sscanf(argv[1], "%x", &code);

    Instruction inst(code);    

    if(inst.isBlx()) {
        inst.blxCode();
    } else {
        switch(inst.getOp()) {
            case 0: inst.dataProcessing(); break;
            case 1: inst.singleMemory(); break;
            case 2: inst.isBranch() ? inst.branch() : inst.multiMemory(); break;
            case 3: inst.swInterrupt(); break;
        }
    }

    return 0;
}