#include <string>
#include <vector>

#include "func.hpp"
#include "code.hpp"

Code::Code(int code) {
    this->code = code;
    setRegister();
    setCond();
}

void Code::setRegister() {
    this->rm = getRegisterName(this->code);
    this->rs = getRegisterName(this->code >> 8);
    this->rd = getRegisterName(this->code >> 12);
    this->rn = getRegisterName(this->code >> 16);
}

int Code::getOp() {
    return (this->code >> 26) & 3;
}

int Code::getCode() {
    return this->code;
}

void Code::setCond() {
    const std::vector<std::string> cond_name = {
        "eq",
        "ne",
        "hs",
        "lo",
        "mi",
        "pl",
        "vs",
        "vc",
        "hi",
        "ls",
        "ge",
        "lt",
        "gt",
        "le",
        ""      // "al"
    };
    
    this->cond = cond_name[(this->code >> 28) & 0xf];
}

bool Code::isBlx() {
    return ((this->code >> 28) & 0xf) == 0xf;
}

bool Code::isBranch() {
    return (this->code >> 25) & 1;
}

std::string Code::getRm() {
    return this->rm;
}

std::string Code::getRs() {
    return this->rs;
}

std::string Code::getRd() {
    return this->rd;
}

std::string Code::getRn() {
    return this->rn;
}