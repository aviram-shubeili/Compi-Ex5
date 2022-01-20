
#ifndef COMPI_EX5_IRSOURCE_H
#define COMPI_EX5_IRSOURCE_H
#include <sstream>
#include "source.hpp"
#include "RegGenerator.h"
#include "bp.hpp"
#include "hw3_output.hpp"


class MNode : public Node {
public:
    string label;
    MNode() : Node(DONT_CARE), label(CodeBuffer::instance().genLabel()) {}
};

class NNode : public Node {
public:
    std::vector<std::pair<int,BranchLabelIndex>> next_list;
    NNode() : Node(DONT_CARE) {
        int loc = CodeBuffer::instance().emit("br label @");
        next_list = CodeBuffer::instance().makelist({loc,FIRST});
    }
};

class BoolExpNode : public ExpNode {
    bool is_evaluated;
    std::string ignore_label;
public:
    void applyNOT();
    void applyAND(const string& right_label, BoolExpNode* right);
    void applyOR(const string& right_label, BoolExpNode* right);
    std::vector<std::pair<int,BranchLabelIndex>> true_list;
    std::vector<std::pair<int,BranchLabelIndex>> false_list;
    BoolExpNode(int lineno, bool value);
    BoolExpNode(int lineno, std::string value);
//    BoolExpNode(int lineno, basictype type, std::vector<std::pair<int,BranchLabelIndex>> true_list, std::vector<std::pair<int,BranchLabelIndex>> false_list)
//            : ExpNode(lineno, type, RegGenerator::Instance().genRegister()),
//              true_list(true_list),
//              false_list(false_list)
//    {}
    std::string getVar() override;
    std::string getVar(bool is_const) override;
    void bpatchTrue(string label);
    void bpatchFalse(string label);

};

class StatementNode : public Node {
public:
    std::vector<std::pair<int,BranchLabelIndex>> next_list;
    void bpatchNextList(string label);
    void MergeNextList(std::vector<std::pair<int,BranchLabelIndex>> other_list);
    explicit StatementNode(int lineno);
    StatementNode(int lineno, bool is_there_jump);
};

string Zext(string reg, basictype type);
string Trunc(string reg, basictype type);


ExpNode* HandleBinopExp(ExpNode* left, Binop* op, ExpNode* right);
ExpNode* HandleIDExp(IdNode* id);
StatementNode* HandleDeclaration(bool is_const, TypeNode* type, IdNode* id);
StatementNode* HandleDeclarationAssignment(bool is_const, TypeNode* type, IdNode* id, ExpNode* exp);
string generateValue(basictype type, string value);
int handleZeroError(string var);
CallNode* HandleFunctionCall(IdNode* func_id, ExpListNode* expList = new ExpListNode(DONT_CARE));
void HandleFunctionDeclaration(RetTypeNode* return_type, IdNode* id, FormalsNode* formals);
StatementNode *HandleAssignment(IdNode *id, ExpNode *exp);
void HandleReturnVoid();
StatementNode* HandleReturnExp(ExpNode* exp);
StatementNode* HandleBreak(int lineno);
StatementNode* HandleContinue(int lineno);
StatementNode* HandleIfStatement(BoolExpNode* exp, string label, StatementNode* s);
StatementNode* HandleIfElseStatement(BoolExpNode* exp, string label1, StatementNode* s1, NNode* N, string label2, StatementNode* s2);
StatementNode* HandleWhileStatement(string cond_label, BoolExpNode* exp, string s_label, StatementNode* s);
BoolExpNode* HandleRelopExp(ExpNode* left, reloptype type , ExpNode* right);
void ImplementPrintingFunctions();
#endif //COMPI_EX5_IRSOURCE_H
