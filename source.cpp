#include "source.hpp"



void FormalsListNode::addArgument(int lineno, Symbol arg) {
    for(int i = 0 ; i < args.size() ; i++) {
        if(arg.name == args[i].name) {
            output::errorDef(lineno, arg.name);
            exit(0);
        }
    }
    args.push_back(arg);
//    args.back().offset = (-args.size());

}

FormalsNode::FormalsNode(int num, std::vector<Symbol> args) : Node(num) {


    std::reverse(args.begin(),args.end());
    for(int i = 0 ; i < args.size() ; i++)
    {
        args[i].offset = -(i+1);
        args[i].value = "%" + to_string(i);
    }
    arguments = args;


}

std::vector<Type> FormalsNode::getArgumentsTypes() {
    std::vector<Type> result;
    for(Symbol& argument : arguments) {
        result.push_back(argument.type);
    }
    return result;
}

std::string FormalsNode::argListToString() {
    if(this->arguments.empty()) {
        return "()";
    }
    string result = "(";
    for( Symbol& arg : arguments) {
        result += typeToString(arg.getType());
        result += ",";
    }
    result[result.length()-1] = ')';

    return result;
}

FormalDeclNode::FormalDeclNode(int lineno, bool is_const, basictype type, std::string name) : Node(lineno),
                                                                                              arg(Symbol(name, Type(is_const,type),ARGUMENT,"")) {}


void ExpListNode::addExp(ExpNode *exp) {
    expressions.push_back(exp);
    types.emplace_back(exp->type);
}



std::string Binop::toString() {
    switch (type) {
        case PLUS: return "add";
        case MINUS: return "sub";
        case MUL: return "mul";
        case DIV: return "sdiv";
        default: assert(false);
    }
}

string typeToString(basictype type) {
    switch (type) {
        case INT_TYPE: return " i32 ";
        case BYTE_TYPE: return " i8 ";
        case BOOL_TYPE: return " i1 ";
        case VOID_TYPE: return " void ";
        case STRING_TYPE: return " i8* ";
        default: assert(false);
    }
}


std::string ExpNode::getVar(bool is_const) {
    return var;
}

std::string ExpNode::getVar() {
    return var;
}

void ExpNode::setVar(string new_var) {
    var = std::move(new_var);
}
