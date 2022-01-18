
#include "IRsource.h"
using namespace std;

string typeToString(basictype type) {
    switch (type) {
        case INT_TYPE: return " i32 ";
        case BYTE_TYPE: return " i8 ";
        case BOOL_TYPE: return " i1 ";
        default: assert(false);
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

    if(op->type == DIV) {
        int loc = handleZeroError(right->var);
        if(left->type == BYTE_TYPE && right->type == BYTE_TYPE)
        {
            // TODO what about byte div int or int div byte
            op_str = "udiv";
        }
        string label = CodeBuffer::instance().genLabel();
        CodeBuffer::instance().bpatch(CodeBuffer::makelist({loc,SECOND}),label);
    }
    string var = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(var + " = " + op_str + " i32 " + left->var + " " + right->var);

    if(result_type == BYTE_TYPE)
    {
        string new_var = RegGenerator::Instance().genRegister();
        CodeBuffer::instance().emit(new_var + " = trunc i32 " + var + " to i8");
        var = new_var;
    }

    ExpNode* res = new ExpNode(right->lineno, result_type, var);
    return res;
}


ExpNode *HandleIDExp(IdNode *id) {
    try {
        Symbol id_sym = SymbolsRepo::Instance().findSymbol(id->name);
        if (id_sym.type.is_function) {
            throw SymbolNotFound();
        }
        if(id_sym.symbol_type != LOCAL) {
            return new ExpNode(id->lineno, id_sym.getType(), id_sym.value, id_sym.symbol_type == LITERAL);
        }
        // symbol is a local variable
        string ptr = RegGenerator::Instance().genRegister();
        string var = RegGenerator::Instance().genRegister();
        string bp = RegGenerator::Instance().getCurrentBasePointer();
        CodeBuffer::instance().emit(ptr + " = getelementptr i32, i32* " + bp + " , i32 " + to_string(id_sym.offset));
        CodeBuffer::instance().emit(var + " = load i32, i32* " + ptr);
        if(id_sym.getType() != INT_TYPE) {
            string new_var = RegGenerator::Instance().genRegister();
            CodeBuffer::instance().emit(new_var + " = trunc i32 " +  var + " to " + typeToString(id_sym.getType()));
            var = new_var;
        }
        return new ExpNode(id->lineno, id_sym.getType(), var, id_sym.symbol_type == LITERAL);
    }
    catch(SymbolNotFound& e) {
        output::errorUndef(id->lineno, id->name);
        exit(0);
    }
}

int handleZeroError(string var) {

    string cond = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(cond + " = icmp eq i32" + var + " , 0");

    int loc = CodeBuffer::instance().emit("br i1 " + cond + ", label @, label @");

    string true_label = CodeBuffer::instance().genLabel();
    CodeBuffer::instance().bpatch(CodeBuffer::makelist({loc,FIRST}),true_label);

    CodeBuffer::instance().emit("call void @print(i8* getelementptr [4 x i8], [4 x i8]* @.zero_error, i32 0, i32 0)");
    CodeBuffer::instance().emit("call void @exit(i32 0)");

    return loc;
}


string generateValue(basictype type, string value) {
    string new_reg = RegGenerator::Instance().genRegister();
    if(type == BOOL_TYPE)
    {
        CodeBuffer::instance().emit(new_reg + "= add i1 " + value + " 0");
    }
    else if(type == INT_TYPE)
    {
        CodeBuffer::instance().emit(new_reg + "= add i32 " + value + " 0");
    }
    else if(type == BYTE_TYPE)
    {
        CodeBuffer::instance().emit(new_reg + "= add i8 " + value + " 0");
    }
    else{
        assert(type==STRING_TYPE);
        string loc = RegGenerator::Instance().genString();
        string len = to_string(value.length() + 1);
        CodeBuffer::instance().emitGlobal(loc + " = constant [" + len + " x i8] c\"" + value + "\"");
        CodeBuffer::instance().emit(new_reg + " = getelementptr " + "[" + len + " x i8] " + "[" + len + " x i8]* " + loc + ", i32 0, i32 0");
    }
    return new_reg;
}

StatementNode *HandleDeclaration(bool is_const, TypeNode *type, IdNode *id) {
    if(is_const) {
        output::errorConstDef(id->lineno);
        exit(0);
    }
    string default_value;
    switch (type->type) {
        case INT_TYPE:
        case BYTE_TYPE:
            default_value = "0";
            break;
        case BOOL_TYPE:
            default_value = "false";
            break;
        default:
            assert(false);
    }
    ExpNode* default_exp = new ExpNode(DONT_CARE, type->type, default_value, true);
    return HandleDeclarationAssignment(is_const, type, id, default_exp);
}

StatementNode *HandleDeclarationAssignment(bool is_const, TypeNode *type, IdNode *id, ExpNode *exp) {

    if((type->type != exp->type) and not (type->type == INT_TYPE and exp->type == BYTE_TYPE)) {
        output::errorMismatch(exp->lineno);
        exit(0);
    }
    try {
        if(is_const and exp->is_literal) {
            SymbolsRepo::Instance().insertSymbolAsLiteral(id->name, Type(is_const, type->type), exp->var);
            return new StatementNode(exp->lineno);
        }
        else {
            SymbolsRepo::Instance().insertSymbol(id->name, Type(is_const, type->type));
            Symbol id_sym = SymbolsRepo::Instance().findSymbol(id->name);
            string bp = RegGenerator::Instance().getCurrentBasePointer();
            string ptr = RegGenerator::Instance().genRegister();
            if(exp->type == BYTE_TYPE)
            {
                string new_var = RegGenerator::Instance().genRegister();
                CodeBuffer::instance().emit(new_var + " = zext i8 " + exp->var + " to i32");
                exp->var = new_var;
            }
            CodeBuffer::instance().emit(ptr + " = getelementptr i32, i32* " + bp + " , i32 " + to_string(id_sym.offset));
            CodeBuffer::instance().emit("store i32 " + exp->var + " i32* " + ptr);
        }
    }
    catch (SymbolAlreadyDefinedInScope& e) {
        output::errorDef(id->lineno, id->name);
        exit(0);
    }
}



BoolExpNode::BoolExpNode(int lineno, basictype type, bool value) :
        ExpNode(lineno, type, RegGenerator::Instance().genRegister(),true)
{
    int loc = CodeBuffer::instance().emit("br label @");
    if (value)
    {
        true_list = CodeBuffer::makelist({loc,FIRST});
    }
    else {
        false_list = CodeBuffer::makelist({loc,FIRST});
    }

}


void BoolExpNode::applyNOT() {
    auto temp = false_list;
    false_list = true_list;
    true_list = temp;
}
void BoolExpNode::applyAND(const string& right_label, BoolExpNode* right) {
    CodeBuffer::instance().bpatch(this->true_list, right_label);
    this->false_list = CodeBuffer::merge(this->false_list, right->false_list);
    this->true_list = right->true_list;
}
void BoolExpNode::applyOR(const string& right_label, BoolExpNode* right) {
    CodeBuffer::instance().bpatch(this->false_list, right_label);
    this->true_list = CodeBuffer::merge(this->true_list, right->true_list);
    this->false_list = right->false_list;
}


















