#include "source.hpp"
#include <memory>
#include <cassert>
#include <algorithm>



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

FormalDeclNode::FormalDeclNode(int lineno, bool is_const, basictype type, std::string name) : Node(lineno),
                                                                                              arg(Symbol(name, Type(is_const,type),DONT_CARE)) {}

void ExpListNode::addExp(basictype type) {
    types.emplace_back(type);
}



int i = 0;

int eager() {
    printf("EAGER");
    exit(0);
}

int normalLazy() {
    i++;
    return 1;
}

void f(int a) {
    if(a == 1) {
        if(a == 2) {
            return;
        }
    }

}

void g(int b) {
}


/**
	Run this function!!!
	Run this function!!!
	Run this function!!!
*/
void ourFunction() {
    g(eager());
    f(normalLazy());
    if(i == 1) {
        printf("LAZY");
    }
    else if(i == 2) {
        printf("NORMAL");
    }
}










