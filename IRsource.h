
#ifndef COMPI_EX5_IRSOURCE_H
#define COMPI_EX5_IRSOURCE_H
#include <sstream>
#include "source.hpp"
#include "RegGenerator.h"
#include "bp.hpp"
#include "hw3_output.hpp"


class MNode : Node {
public:
    string label;
    MNode() : Node(DONT_CARE), label(CodeBuffer::instance().genLabel()) {}
};

class NNode : Node {
public:
    std::vector<std::pair<int,BranchLabelIndex>> next_list;
    NNode() : Node(DONT_CARE) {
        int loc = CodeBuffer::instance().emit("br label @");
        next_list = CodeBuffer::instance().makelist({loc,FIRST});
    }
};

class BoolExpNode : ExpNode {
public:
    void applyNOT();
    void applyAND(const string& right_label, BoolExpNode* right);
    void applyOR(const string& right_label, BoolExpNode* right);
    std::vector<std::pair<int,BranchLabelIndex>> true_list;
    std::vector<std::pair<int,BranchLabelIndex>> false_list;
    std::string true_label;
    std::string false_label;
    BoolExpNode(int lineno, basictype type, bool value);
    BoolExpNode(int lineno, basictype type, std::vector<std::pair<int,BranchLabelIndex>> true_list, std::vector<std::pair<int,BranchLabelIndex>> false_list)
            : ExpNode(lineno, type, RegGenerator::Instance().genRegister()),
              true_list(true_list),
              false_list(false_list)
    {}
};

class StatementNode : public Node {
public:
    std::vector<std::pair<int,BranchLabelIndex>> next_list;
    explicit StatementNode(int lineno) : Node(lineno) {}
};


string typeToString(basictype type);
ExpNode* HandleBinopExp(ExpNode* left, Binop* op, ExpNode* right);
ExpNode* HandleIDExp(IdNode* id);
StatementNode* HandleDeclaration(bool is_const, TypeNode* type, IdNode* id);
StatementNode* HandleDeclarationAssignment(bool is_const, TypeNode* type, IdNode* id, ExpNode* exp);
string generateValue(basictype type, string value);
int handleZeroError(string var);


#endif //COMPI_EX5_IRSOURCE_H
