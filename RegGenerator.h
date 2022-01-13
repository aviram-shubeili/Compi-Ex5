
#ifndef COMPI_EX5_REGISTERGENERATOR_H
#define COMPI_EX5_REGISTERGENERATOR_H
#include <string>

class RegGenerator {

private:
    unsigned int free_index;
    RegGenerator();
public:
    static RegGenerator& Instance();
    RegGenerator(RegGenerator const&)      = delete; // disable copy ctor
    void operator=(RegGenerator const&)  = delete; // disable = operator

    std::string genRegister();

};

class RegisterException : public std::exception {};

#endif //COMPI_EX5_REGISTERGENERATOR_H
