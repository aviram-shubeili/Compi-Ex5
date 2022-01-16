
#ifndef COMPI_EX5_IRSOURCE_H
#define COMPI_EX5_IRSOURCE_H
#include <sstream>
#include "source.hpp"
#include "RegGenerator.h"
#include "bp.hpp"
#include "hw3_output.hpp"
string typeToString(basictype type);
ExpNode* HandleBinopExp(ExpNode* left, Binop* op, ExpNode* right);


#endif //COMPI_EX5_IRSOURCE_H
