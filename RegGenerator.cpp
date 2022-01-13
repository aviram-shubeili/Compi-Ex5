
#include "RegGenerator.h"

#include "SymbolsRepo.h"
#include <cassert>
using namespace std;
RegGenerator::RegGenerator() : free_index(0) {}

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


