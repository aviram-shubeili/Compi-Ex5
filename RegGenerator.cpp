
#include "RegGenerator.h"

#include "SymbolsRepo.h"
#include <cassert>

using namespace std;
RegGenerator::RegGenerator() : free_index(0) {
    CodeBuffer::instance().emitGlobal("@.zero_error = constant [23 x i8] c\"Error division by zero\\00\" ");

}

// make SymbolsRepo singleton
RegGenerator &RegGenerator::Instance() {
    static RegGenerator instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
}

string RegGenerator::genRegister() {
    std::stringstream reg;
    reg << "%t";
    reg << free_index++;
    std::string ret(reg.str());
    return ret;
}
std::string RegGenerator::genString() {
    std::stringstream res;
    res << "@.s" << free_index++;
    return res.str();
}
std::string RegGenerator::genConst() {
    std::stringstream res;
    res << "@.c" << free_index++;
    return res.str();
}

std::string RegGenerator::genBP() {
    std::stringstream bp;
    bp << "%bp";
    bp << free_index++;
    std::string ret(bp.str());
    CodeBuffer::instance().emit(ret + " = alloca i32, i32 50");
    insertBasePointer(ret);
    return ret;
}

std::string RegGenerator::getCurrentBasePointer() {
    return bp_stack.top();
}

void RegGenerator::insertBasePointer(std::string new_bp) {
    bp_stack.push(new_bp);
}

void RegGenerator::popBasePointer() {
    bp_stack.pop();
}



