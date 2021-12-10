#pragma once

#include <string>
#include "code.hpp"

class Instruction : public Code {
public:
    using Code::Code;

    void dataProcessing();
    void singleMemory();
    void multiMemory();
    void branch();
    void swInterrupt();
    void blxCode();

private:
    void shiftCode(bool s_bit, bool imm_bit);
};