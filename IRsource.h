
#ifndef COMPI_EX5_IRSOURCE_H
#define COMPI_EX5_IRSOURCE_H
#include "source.hpp"
#include "RegGenerator.h"
#include "bp.hpp"

class BinopExp : public ExpNode {
    ExpNode* left;
    binoptype op;
    ExpNode* right;

public:
    BinopExp(ExpNode* left, Binop* op, ExpNode* right);
};


#endif //COMPI_EX5_IRSOURCE_H
