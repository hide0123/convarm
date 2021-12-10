#include <iostream>
#include <sstream>

#include "func.hpp"

std::string hexToString(int i, bool sign) {
    std::stringstream ss;
    ss << "#" << (sign ? "" : "-") << "0x" << std::hex << i;
    return ss.str();
}

std::string getRegisterName(int pos) {
    int reg = pos & 0xf;
    std::string reg_name = "r" + std::to_string(reg);

    switch(reg){
        case 11: reg_name = "fp"; break;
        case 12: reg_name = "ip"; break;
        case 13: reg_name = "sp"; break;
        case 14: reg_name = "lr"; break;
        case 15: reg_name = "pc"; break;
    }
    return reg_name;
}