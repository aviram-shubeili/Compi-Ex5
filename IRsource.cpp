
#include "IRsource.h"
using namespace std;
BinopExp::BinopExp(ExpNode* left, Binop* op, ExpNode* right) : ExpNode(right->lineno, right->type, RegGenerator::Instance().genRegister()),
left(left),
op(op->type),
right(right)  {
        if(this->op == DIV) {

        }

        stringstream result;
        result << var << " = " ;
        result << toString(this->op) << " i32 ";
        result << left->var << " " << right->var;
        CodeBuffer.instance().emit(result.str());
}