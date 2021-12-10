#pragma once

#include <string>

class Code {
public:
    Code(int code);

    int getOp();
    int getCode();

    bool isBlx();
    bool isBranch();

    std::string getRm();
    std::string getRs();
    std::string getRd();
    std::string getRn();

protected:
    int code;
    std::string cond;
    std::string rm, rs, rd, rn;

private:
    void setRegister();
    void setCond();
};