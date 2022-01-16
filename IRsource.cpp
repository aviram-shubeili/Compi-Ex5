
#include "IRsource.h"

using namespace std;

string typeToString(basictype type) {
    switch (type) {
        case INT_TYPE: return " i32 ";
        case BYTE_TYPE: return " i8 ";
        case BOOL_TYPE: return " i1 ";
        default: throw invalid_argument();
    }
}

ExpNode* HandleBinopExp(ExpNode* left, Binop* op, ExpNode* right) {

    if((left->type != INT_TYPE) and (left->type != BYTE_TYPE)) {
        output::errorMismatch(left->lineno);
        exit(0);
    }
    if((right->type != INT_TYPE) and (right->type != BYTE_TYPE)) {
        output::errorMismatch(right->lineno);
        exit(0);
    }

    string op_str = op->toString();
    basictype result_type = (left->type == INT_TYPE or right->type == INT_TYPE)
                            ? INT_TYPE
                            : BYTE_TYPE;

    if(op == DIV) {
        int loc = handleZeroError(right->var);
        if(left->type == BYTE_TYPE && right->type == BYTE_TYPE)
        {
            // TODO what about byte div int or int div byte
            op_str = "udiv";
        }
        string label = CodeBuffer::instance().genLabel();
        CodeBuffer::instance().bpatch(makelist({loc,SECOND}),label);
    }
    string var = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(var + " = " + op_str + " i32 " + left->var + " " + right->var);

    if(result_type == BYTE_TYPE)
    {
        string new_var = RegGenerator::Instance().genRegister();
        CodeBuffer::instance().emit(new_var + " = and i32 " + var + ", 255");
        var = new_var;
    }

    ExpNode* res = new ExpNode(right->lineno, result_type, var);
    return res;
}

int handleZeroError(string var) {

    string cond = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(cond + " = icmp eq i32" + var + " , 0");

    int loc = CodeBuffer::instance().emit("br i1 " + cond + ", label @, label @");

    string true_label = CodeBuffer::instance().genLabel();
    CodeBuffer::instance().bpatch(makelist({loc,FIRST}),true_label);

    // TODO push @.zero_error to global buffer
    CodeBuffer::instance().emit("call void @print(i8* getelementptr [4 x i8], [4 x i8]* @.zero_error, i32 0, i32 0)");
    CodeBuffer::instance().emit("call void @exit(i32 0)");

    return loc;


}