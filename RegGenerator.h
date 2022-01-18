
#ifndef COMPI_EX5_REGISTERGENERATOR_H
#define COMPI_EX5_REGISTERGENERATOR_H
#include <string>
#include <sstream>
#include <stack>
#include "bp.hpp"
class RegGenerator {

private:
    unsigned int free_index;

    RegGenerator();
    std::stack<std::string> bp_stack;
public:
    static RegGenerator& Instance();
    RegGenerator(RegGenerator const&)      = delete; // disable copy ctor
    void operator=(RegGenerator const&)  = delete; // disable = operator
    std::string getCurrentBasePointer();
    void insertBasePointer(std::string new_bp);
    void popBasePointer();
    std::string genRegister();
    std::string genBP();
    std::string genString();
    std::string genConst();

};

class RegisterException : public std::exception {};

#endif //COMPI_EX5_REGISTERGENERATOR_H
