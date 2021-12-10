#include <iostream>
#include <string>
#include <vector>

#include "func.hpp"
#include "instruction.hpp"

void Instruction::shiftCode(bool s_bit, bool imm_bit) {
    std::string cmd;

    if(imm_bit) {
        std::cout << "mov" << (s_bit ? "s" : "") << cond << " " << rd << ", " << hexToString(code & 0xff, true) << std::endl;
        return;
    }
    
    if(((code >> 4) & 0xff) == 0) {
        std::cout << "mov" << (s_bit ? "s" : "") << cond << " " << rd << ", " << rm << std::endl;
    } else {
        switch((code >> 5) & 2) {
            case 0: cmd = "lsl"; break;
            case 1: cmd = "lsr"; break;
            case 2: cmd = ((code >> 7) & 0xff ? "ror" : "rrx"); break;
        }
        
        (code >> 4) & 1 ?
            std::cout << cmd << (s_bit ? "s" : "") << cond << " " << rd << ", " << rm << ", " << rs << std::endl:
            std::cout << cmd << (s_bit ? "s" : "") << cond << " " << rd << ", " << rm << ", " << hexToString((code >> 7) & 0x1f, true) << std::endl;
    }
}

void Instruction::dataProcessing() {
    std::string cmd, flag;

    bool psr = (code >> 22) & 1;
    bool s_bit = (code >> 20) & 1;
    bool imm_bit = (code >> 25) & 1;
    bool bx_mode = ((code >> 20) & 0x1f) == 0x12;
    bool cmp_mode = ((code >> 23) & 3) == 2;
    bool mul_mode = !imm_bit && ((code >> 4) & 0xf) == 9;

    std::vector<std::string> cmd_name = {
        
        mul_mode ? "mul" : "and",
        mul_mode ? "mla" : "eor",
        
        "sub",
        "rsb",
        
        mul_mode ? "umull" : "add",
        mul_mode ? "umlal" : "adc",
        mul_mode ? "smull" : "sbc",
        mul_mode ? "smlal" : "rsc",
        
        s_bit ? "tst" : "mrs",
        s_bit ? "teq" : "msr",
        s_bit ? "cmp" : "mrs",
        s_bit ? "cmn" : "msr",
        
        "orr",
        "",     // shift code
        "bic",
        "mvn"
    };

    std::vector<std::string> flag_list = {
        "c",
        "x",
        "s",
        "f"
    };

    for(int i = 0; i < 4; i++) {
        if((code >> (16 + i)) & 1) {
            flag += flag_list[i];
        }
    } 

    // nop
    if(code == 0xe320f000) {
        std::cout << "nop" << std::endl;
        return;
    }

    // Opcode is shift code
    if(((code >> 21) & 0xf) == 0xd) {
        shiftCode(s_bit, imm_bit);
        return;
    }

    cmd = cmd_name[(code >> 21) & 0xf];

    // An immediate value exists
    if(imm_bit) {
        if(cmd == "msr") {
            std::cout << cmd << " " << (psr ? "spsr" : "cpsr") << "_" << flag << ", " << hexToString(code & 0xff, true) << std::endl;
        } else if(cmp_mode) {
            std::cout << cmd << cond << " " << rn << ", " << hexToString(code & 0xff, true) << std::endl;
        } else {
            std::cout << cmd << (s_bit ? "s" : "") << cond << " " << rd << ", " << (cmd == "mvn" ? "" : rn + ", ") \
            << hexToString(code & 0xff, true) << std::endl;
        }
        return;
    }

    if(bx_mode) {
        std::cout << "bx" << cond << " " << rm << std::endl;
    } else if(cmp_mode) {
        if(cmd == "mrs")
            std::cout << cmd << " " << rd << ", " << (psr ? "spsr" : "cpsr") << std::endl;
        else if(cmd == "msr")
            std::cout << cmd << " " << (psr ? "spsr" : "cpsr") << "_" << flag << ", " << rm << std::endl;
        else
            std::cout << cmd << cond << " " << rn << ", " << rm << std::endl;
    } else if(mul_mode) {
        if(cmd == "mla")
            rd.swap(rn);

        std::cout << cmd << (s_bit ? "s" : "") << cond << " " << rd << ", " \
        << ((code >> 23) & 1 ? (rn + ", ") : "") << rm << ", " << rs \
        << (cmd == "mla" ? (", " + rn) : "") << std::endl;
    } else {
        std::cout << cmd << (s_bit ? "s" : "") << cond << " " << rd << ", " \
        << (cmd == "mvn" ? "" : rn + ", ") << rm << std::endl;
    }
}

void Instruction::singleMemory() {
    bool l_bit = (code >> 20) & 1;
    bool w_bit = (code >> 21) & 1;
    bool b_bit = (code >> 22) & 1;
    bool u_bit = (code >> 23) & 1;
    bool p_bit = (code >> 24) & 1;
    bool imm_bit = !((code >> 25) & 1);

    int imm12 = code & 0xfff;

    std::string cmd = l_bit ? "ldr" : "str";
    std::string byte = b_bit ? "b" : "";
    std::string write_back = w_bit ? "!" : "";
    std::string imm = imm12 ? (", " + hexToString(imm12, u_bit)) : "";

    if(imm_bit) {
        p_bit?
            std::cout << cmd << byte << cond << " " << rd << ", [" << rn << imm << "]" << write_back << std::endl:
            std::cout << cmd << byte << cond << " " << rd << ", [" << rn << "], " << hexToString(imm12, u_bit) << std::endl;
    } else {
        p_bit?
            std::cout << cmd << byte << cond << " " << rd << ", [" << rn << ", " << rm << "]" << write_back << std::endl:
            std::cout << cmd << byte << cond << " " << rd << ", [" << rn << "], " << rm << std::endl;
    }
}

void Instruction::multiMemory() {
    bool cmd = (code >> 20) & 1;
    std::string reg_list;

    std::vector<std::string> adr_mode = {
        rn == "sp" ? (cmd ? "fa" : "ed") : "da",
        rn == "sp" ? (cmd ? "fd" : "ea") : "ia",
        rn == "sp" ? (cmd ? "ea" : "fd") : "db",
        rn == "sp" ? (cmd ? "ed" : "fa") : "ib"
    };

    for(int i = 0; i < 16; i++) {
        if((code >> i) & 1)
            reg_list += (reg_list.empty() ? "{" : ", ") + getRegisterName(i);
        
        if(i == 15)
            reg_list += "}";
    }

    std::cout << (cmd ? "ldm" : "stm") << adr_mode[(code >> 23) & 3] << cond << " " << rn << "!, " << reg_list << std::endl;
}

void Instruction::branch() {
    bool link = (code >> 24) & 1;
    int imm = (((code & 0xffffff) | (0xff << 24)) << 2) + 8;

    std::cout << (link ? "bl" : "b") << cond << " " << hexToString(imm, true) << std::endl;
}

void Instruction::swInterrupt() {
    int svc_num = code & 0xffffff;
    std::cout << "svc" << cond << " " << hexToString(svc_num, true) << std::endl;
}

void Instruction::blxCode() {
    int imm = (((code & 0xffffff) | (0xff << 24)) << 2) + 8;
    std::cout << "blx " << hexToString(imm, true) << std::endl;
}