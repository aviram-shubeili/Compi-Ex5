
#include "IRsource.h"


string Load(int offset);
int ZextAndStore(int offset, string value, basictype value_type);
int Store(int offset, string value);
string LoadAndTrunc(int offset, basictype value_type);

string jmpListToString(vector<pair<int, BranchLabelIndex>> vector);

using namespace std;

string RelopTypeToString(reloptype type) {
    switch (type) {
        case EQUALS: return " eq ";
        case NOT_EQUALS: return " ne ";
        case BIGGER: return " sgt ";
        case SMALLER: return " slt ";
        case BIGGER_EQUALS: return " sge ";
        case SMALLER_EQUALS: return " sle ";
        default: assert(false);
    }
}
string Zext(string reg, basictype source_type) {
    if(source_type == INT_TYPE) {
        return reg;
    }
    string new_reg = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(new_reg + " = zext " + typeToString(source_type) + " " + reg + " to i32");
    return new_reg;
}
string Trunc(string reg, basictype result_type) {
    if(result_type == INT_TYPE) {
        return reg;
    }
    string new_reg = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(new_reg + " = trunc i32 " + reg + " to " + typeToString(result_type));
    return new_reg;
}
int Store(int offset, string value) {
    string bp = RegGenerator::Instance().getCurrentBasePointer();
    string ptr = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(ptr + " = getelementptr i32, i32* " + bp + " , i32 " + to_string(offset));
    int loc = CodeBuffer::instance().emit("store i32 " + value + ", i32* " + ptr);
    return loc;
}
string Load(int offset) {
    string bp = RegGenerator::Instance().getCurrentBasePointer();
    string ptr = RegGenerator::Instance().genRegister();
    string var = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(ptr + " = getelementptr i32, i32* " + bp + " , i32 " + to_string(offset));
    CodeBuffer::instance().emit(var + " = load i32, i32* " + ptr);
    return var;
}
int ZextAndStore(int offset, string value, basictype value_type) {
    value = Zext(value, value_type);
    return Store(offset, value);
}
string LoadAndTrunc(int offset, basictype value_type) {
    string var = Load(offset);
    return Trunc(var, value_type);
}
int handleZeroError(string var, basictype type) {

    string cond = RegGenerator::Instance().genRegister();

    CodeBuffer::instance().emit(cond + " = icmp eq " + typeToString(type) + " " + var + " , 0");

    int loc = CodeBuffer::instance().emit("br i1 " + cond + ", label @, label @");

    string true_label = CodeBuffer::instance().genLabel();
    CodeBuffer::instance().bpatch(CodeBuffer::makelist({loc,FIRST}),true_label);

    string ptr = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(ptr + " = getelementptr [23 x i8], [23 x i8]* @.zero_error, i32 0, i32 0");
    CodeBuffer::instance().emit("call void @print(i8* " + ptr + ")");
    CodeBuffer::instance().emit("call void @exit(i32 0)");
    CodeBuffer::instance().emit("unreachable");
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
        string len = to_string(value.length() -1);
        value[value.length() -1] = '\\';
        CodeBuffer::instance().emitGlobal(loc + " = constant [" + len + " x i8] c" + value + "00\"");
        CodeBuffer::instance().emit(new_reg + " = getelementptr " + "[" + len + " x i8], " + "[" + len + " x i8]* " + loc + ", i32 0, i32 0");
    }
    return new_reg;
}

void ImplementPrintingFunctions() {

    CodeBuffer::instance().emit("declare i32 @printf(i8*, ...)                                                  ");
    CodeBuffer::instance().emit("declare void @exit(i32)                                                        ");
    CodeBuffer::instance().emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"                       ");
    CodeBuffer::instance().emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"                       ");
    CodeBuffer::instance().emit("                                                                               ");
    CodeBuffer::instance().emit("define void @printi(i32) {                                                     ");
    CodeBuffer::instance().emit("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
    CodeBuffer::instance().emit("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)                         ");
    CodeBuffer::instance().emit("    ret void                                                                   ");
    CodeBuffer::instance().emit("}                                                                              ");
    CodeBuffer::instance().emit("                                                                               ");
    CodeBuffer::instance().emit("define void @print(i8*) {                                                      ");
    CodeBuffer::instance().emit("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
    CodeBuffer::instance().emit("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)                         ");
    CodeBuffer::instance().emit("    ret void                                                                   ");
    CodeBuffer::instance().emit("}                                                                              ");

}

void HandleFunctionDeclaration(RetTypeNode* return_type, IdNode* id, FormalsNode* formals) {

    try {
        SymbolsRepo::Instance().insertSymbol(id->name, (Type(return_type->type, formals->getArgumentsTypes())));
        SymbolsRepo::Instance().currentFunctionName = id->name;
        SymbolsRepo::Instance().openNewScope();

        for(auto& arg : formals->arguments) {
            SymbolsRepo::Instance().insertSymbolAsArgument(arg.name, arg);
        }
    }
    catch (SymbolAlreadyDefinedInScope& e) {
        output::errorDef(id->lineno, id->name);
        exit(0);
    }
    CodeBuffer::instance().emit("define " + typeToString(return_type->type) + "@" + id->name + formals->argListToString() + " {");
    RegGenerator::Instance().genBP();
}


StatementNode::StatementNode(int lineno, jump_type jump_t)  : Node(lineno) {
    int loc;
    switch (jump_t) {
        case NEXT_JMP:
            loc = CodeBuffer::instance().emit("br label @");
            next_list = CodeBuffer::makelist({loc, FIRST});
            break;
        case BREAK_JMP:
            loc = CodeBuffer::instance().emit("br label @");
            break_list = CodeBuffer::makelist({loc, FIRST});
            break;
        case NONE: return;
    }
}

void StatementNode::MergeNextList(std::vector<std::pair<int, BranchLabelIndex>> other_list) {
    this->next_list = CodeBuffer::merge(this->next_list, other_list);
}

void StatementNode::bpatchNextList(string label) {
    CodeBuffer::instance().bpatch(next_list, label);
    next_list.clear();
}

void StatementNode::MergeBreakList(std::vector<std::pair<int, BranchLabelIndex>> other_list) {
    this->break_list = CodeBuffer::merge(this->break_list, other_list);
}
void StatementNode::bpatchBreakList(string label) {
    CodeBuffer::instance().bpatch(break_list, label);
    break_list.clear();
}


StatementNode *HandleDeclaration(bool is_const, TypeNode *type, IdNode *id) {
    if(is_const) {
        output::errorConstDef(id->lineno);
        exit(0);
    }
    string default_value = "0";
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
            SymbolsRepo::Instance().insertSymbolAsLiteral(id->name, Type(is_const, type->type), exp->getVar(true));
            CodeBuffer::instance().emit("; literal definition here");
            return new StatementNode(exp->lineno, NONE);
        }
        else {
            SymbolsRepo::Instance().insertSymbol(id->name, Type(is_const, type->type));
            Symbol id_sym = SymbolsRepo::Instance().findSymbol(id->name);
            string bp = RegGenerator::Instance().getCurrentBasePointer();
            string ptr = RegGenerator::Instance().genRegister();
            if(exp->type != INT_TYPE)
            {
                exp->setVar(Zext(exp->getVar(), exp->type));
            }
            Store(id_sym.offset, exp->getVar());
            return new StatementNode(exp->lineno, NONE);
        }
    }
    catch (SymbolAlreadyDefinedInScope& e) {
        output::errorDef(id->lineno, id->name);
        exit(0);
    }
}
StatementNode *HandleAssignment(IdNode *id, ExpNode *exp) {
    try {
        Symbol id_sym = SymbolsRepo::Instance().findSymbol(id->name);
        if(id_sym.type.is_const) {
            output::errorConstMismatch(id->lineno);
            exit(0);
        }
        if((id_sym.getType() != exp->type) and not (id_sym.getType() == INT_TYPE and exp->type == BYTE_TYPE)) {
            output::errorMismatch(exp->lineno);
            exit(0);
        }

        ZextAndStore(id_sym.offset, exp->getVar(), exp->type);
        return new StatementNode(exp->lineno, NONE);
    }
    catch(SymbolNotFound& e) {
        output::errorUndef(id->lineno, id->name);
        exit(0);
    }

}
StatementNode * HandleReturnVoid(int lineno) {
    try {
        Symbol current_function = SymbolsRepo::Instance().findSymbol(SymbolsRepo::Instance().currentFunctionName);
        if(current_function.getType() != VOID_TYPE) {
            output::errorMismatch(lineno);
            exit(0);
        }
        CodeBuffer::instance().emit("ret void");
        return new StatementNode(lineno, NONE);
    }
    catch(SymbolNotFound& e) {
        assert(false);
    }
}
StatementNode* HandleReturnExp(ExpNode* exp) {
    try {
        Symbol current_function = SymbolsRepo::Instance().findSymbol(SymbolsRepo::Instance().currentFunctionName);

        if((current_function.getType() != exp->type and
            not (current_function.getType() == INT_TYPE and exp->type == BYTE_TYPE)) or
           current_function.getType() == VOID_TYPE)
        {
            output::errorMismatch(exp->lineno);
            exit(0);
        }
        CodeBuffer::instance().emit("ret " + typeToString(exp->type) + " " + exp->getVar());
        return new StatementNode(exp->lineno, NONE);
    }
    catch(SymbolNotFound& e) {
        assert(false);
    }
}
StatementNode* HandleBreak(int lineno) {
    // TODO
    if(not SymbolsRepo::Instance().isInLoop()) {
        output::errorUnexpectedBreak(lineno);
        exit(0);
    }
    return new StatementNode(lineno, BREAK_JMP);
}
StatementNode* HandleContinue(int lineno) {
    if(not SymbolsRepo::Instance().isInLoop()) {
        output::errorUnexpectedContinue(lineno);
        exit(0);
    }
    return new StatementNode(lineno, NEXT_JMP);
}
StatementNode* HandleIfStatement(BoolExpNode* exp, string label, StatementNode* s) {
    exp->bpatchTrue(label);
    s->MergeNextList(exp->false_list);
    return s;
}
StatementNode* HandleIfElseStatement(BoolExpNode* exp, string label1, StatementNode* s1, NNode* N, string label2, StatementNode* s2) {
    exp->bpatchTrue(label1);
    exp->bpatchFalse(label2);


    s2->MergeNextList(N->next_list);
    s2->MergeNextList(s1->next_list);
    s2->MergeBreakList(s1->break_list);
    return s2;
}

string jmpListToString(vector<pair<int, BranchLabelIndex>> vector) {
    string result = "***************\n";
    for ( auto jmp : vector) {
        result += to_string(jmp.first);
        result += ", ";
        result += to_string(jmp.second) + "\n";
    }
    result += "***************\n";
    return result;
}

StatementNode* HandleWhileStatement(string cond_label, BoolExpNode* exp, string s_label, StatementNode* s) {
    StatementNode* result = new StatementNode(s->lineno, NONE);
    // if cond is false goto next statement
    result->next_list = exp->false_list;
    result->MergeNextList(s->break_list);
    // if true do s..
    exp->bpatchTrue(s_label);
    // after s check cond again
    s->bpatchNextList(cond_label);

    CodeBuffer::instance().emit("br label %" + cond_label );
    return result;
}

ExpNode* HandleExpCall(CallNode* callNode) {
    if(callNode->type == BOOL_TYPE) {
        return new BoolExpNode(callNode->lineno, callNode->value);
    }

    return new ExpNode(callNode->lineno, callNode->type, callNode->value);
}

CallNode* HandleFunctionCall(IdNode* func_id, ExpListNode* expList) {
    try {
        Symbol id_sym = SymbolsRepo::Instance().findSymbol(func_id->name);
        if(not id_sym.type.is_function) {
            throw SymbolNotFound();
        }

        if(not hasSameArguments(id_sym.type.arguments, expList->types)) {
            std::vector<std::string> args_as_strings = id_sym.type.getArgumentsAsStrings();
            output::errorPrototypeMismatch(func_id->lineno, func_id->name, args_as_strings);
            exit(0);
        }
        string openning;
        string result_reg;
        if(id_sym.getType() != VOID_TYPE) {
            result_reg = RegGenerator::Instance().genRegister();
            openning = result_reg + " = ";
        }
        CodeBuffer::instance().emit(openning + "call" + typeToString(id_sym.getType()) + "@" + func_id->name +
                                    expList->argListToString(id_sym.type.arguments));

        return new CallNode(func_id->lineno, id_sym.getType(), result_reg);
    }
    catch(SymbolNotFound& e) {
        output::errorUndefFunc(func_id->lineno, func_id->name);
        exit(0);
    }
}
void HandleEndOfFunction(StatementNode* s) {

    try {
        Symbol current_function = SymbolsRepo::Instance().findSymbol(SymbolsRepo::Instance().currentFunctionName);
        int loc = CodeBuffer::instance().emit("br label @");
        string func_end_label = CodeBuffer::instance().genLabel();
        s->bpatchNextList(func_end_label);
        CodeBuffer::instance().bpatch(CodeBuffer::makelist({loc,FIRST}),func_end_label);
        if (current_function.getType() != VOID_TYPE) {
            CodeBuffer::instance().emit("ret " + typeToString(current_function.getType()) + " 0");
        }
        else {
            CodeBuffer::instance().emit("ret void");
        }
        RegGenerator::Instance().popBasePointer();
    }
    catch(SymbolNotFound& e) {
        assert(false);
    }
}

BoolExpNode::BoolExpNode(int lineno, bool value) :
        ExpNode(lineno, BOOL_TYPE, "",true)
{
    int loc = CodeBuffer::instance().emit("br label @");
    ignore_label = CodeBuffer::instance().genLabel();
    if (value)
    {
        true_list = CodeBuffer::makelist({loc,FIRST});
        var = "1";
    }
    else {
        false_list = CodeBuffer::makelist({loc,FIRST});
        var = "0";
    }
    this->is_evaluated = false;
}
BoolExpNode::BoolExpNode(int lineno, std::string value) :
        ExpNode(lineno, BOOL_TYPE, "",false /*TODO is this ok? */ ) {

    string cond = RegGenerator::Instance().genRegister();
    assert(!value.empty());
    CodeBuffer::instance().emit(cond + " = icmp eq i1 " + value + " , 1");
    int loc = CodeBuffer::instance().emit("br i1 " + cond + ", label @, label @");
    ignore_label = CodeBuffer::instance().genLabel();

    true_list = CodeBuffer::makelist({loc,FIRST});
    false_list = CodeBuffer::makelist({loc,SECOND});

    this->is_evaluated = false;

}
void BoolExpNode::applyNOT() {
    this->is_literal = false;
    auto temp = false_list;
    false_list = true_list;
    true_list = temp;
}
void BoolExpNode::applyAND(const string& right_label, BoolExpNode* right) {
    this->is_literal = false;
    CodeBuffer::instance().bpatch(this->true_list, right_label);
    this->false_list = CodeBuffer::merge(this->false_list, right->false_list);
    this->true_list = right->true_list;
    this->ignore_label = right->ignore_label;
}
void BoolExpNode::applyOR(const string& right_label, BoolExpNode* right) {
    this->is_literal = false;
    CodeBuffer::instance().bpatch(this->false_list, right_label);
    this->true_list = CodeBuffer::merge(this->true_list, right->true_list);
    this->false_list = right->false_list;
    this->ignore_label = right->ignore_label;
}

std::string BoolExpNode::getVar(bool is_const) {
    if(this->is_evaluated)
    {
        return var;
    }
    if(is_const and this->is_literal)
    {
        CodeBuffer::instance().bpatch(this->false_list,ignore_label);
        CodeBuffer::instance().bpatch(this->true_list,ignore_label);
        this->is_evaluated = true;
        return var;
    }

    // true bp
    int start_evaluating = CodeBuffer::instance().emit( string("\nbr label @ ") + "; skip boolean evaluation\n" +  ";start evaluation of boolean ");

    string true_label = CodeBuffer::instance().genLabel();


    int true_label_loc = CodeBuffer::instance().emit("br label @");
    CodeBuffer::instance().bpatch(this->true_list,true_label);
    //false bp
    string false_label = CodeBuffer::instance().genLabel();
    int false_label_loc = CodeBuffer::instance().emit("br label @");
    CodeBuffer::instance().bpatch(this->false_list,false_label);
    //end bp
    string end_label = CodeBuffer::instance().genLabel();
    string result_reg = RegGenerator::Instance().genRegister();
    auto end_list = CodeBuffer::makelist({true_label_loc, FIRST});
    end_list = CodeBuffer::merge(end_list, CodeBuffer::makelist({false_label_loc, FIRST}));
    CodeBuffer::instance().bpatch(end_list,end_label);


    CodeBuffer::instance().emit(result_reg + " = phi i1 [1, %"  + true_label + "], [0, %" + false_label + "]");
    CodeBuffer::instance().emit("br label %" + ignore_label + " ; finish evaluating boolean --> return to code\n");
    string ended_evaluation_label = CodeBuffer::instance().genLabel();
    CodeBuffer::instance().bpatch(CodeBuffer::makelist({start_evaluating,FIRST}),ended_evaluation_label);

    this->true_list.clear();
    this->false_list.clear();
    this->var = result_reg;
    this->is_evaluated = true;

    return result_reg;

}
std::string BoolExpNode::getVar() {
    return getVar(false);
}

void BoolExpNode::bpatchTrue(string label) {
    CodeBuffer::instance().bpatch(this->true_list, label);
    true_list.clear();
}

void BoolExpNode::bpatchFalse(string label) {
    CodeBuffer::instance().bpatch(this->false_list, label);
    false_list.clear();
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

    string operation_size = " i32 ";

    string left_arg = left->getVar();
    string right_arg = right->getVar();
    if(left->type == BYTE_TYPE && right->type == BYTE_TYPE)
    {
        operation_size = " i8 ";
    }
    else {
        left_arg = Zext(left_arg, left->type);
        right_arg = Zext(right_arg, right->type);
    }

    if(op->type == DIV) {
        int loc = handleZeroError(right->getVar(), right->type);
        if(left->type == BYTE_TYPE && right->type == BYTE_TYPE)
        {
            op_str = "udiv";
        }
        string label = CodeBuffer::instance().genLabel();
        CodeBuffer::instance().bpatch(CodeBuffer::makelist({loc,SECOND}),label);
    }
    string var = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(var + " = " + op_str + operation_size + left_arg + ", " + right_arg);

//    if(result_type == BYTE_TYPE)
//    {
//        string new_var = RegGenerator::Instance().genRegister();
//        CodeBuffer::instance().emit(new_var + " = trunc i32 " + var + " to i8");
//        var = new_var;
//    }

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
            if(id_sym.getType() == BOOL_TYPE) {
                return new BoolExpNode(id->lineno,id_sym.value);
            }
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
        if(id_sym.getType() == BOOL_TYPE) {
            return new BoolExpNode(id->lineno,var);
        }
        return new ExpNode(id->lineno, id_sym.getType(), var, id_sym.symbol_type == LITERAL);
    }
    catch(SymbolNotFound& e) {
        output::errorUndef(id->lineno, id->name);
        exit(0);
    }
}
BoolExpNode* HandleRelopExp(ExpNode* left, reloptype type , ExpNode* right) {
    if((left->type != INT_TYPE) and (left->type != BYTE_TYPE)) {
//        cout << ("left?");
        output::errorMismatch(left->lineno);
        exit(0);
    }
    if((right->type != INT_TYPE) and (right->type != BYTE_TYPE)) {
//        cout << ("right?");
        output::errorMismatch(right->lineno);
        exit(0);
    }
    string right_reg = Zext(right->getVar(), right->type);
    string left_reg = Zext(left->getVar(), left->type);
    string cond = RegGenerator::Instance().genRegister();
    CodeBuffer::instance().emit(cond + " = icmp" + RelopTypeToString(type) + "i32 " + left_reg + " , " + right_reg);
    return new BoolExpNode(right->lineno, cond);
}


std::string ExpListNode::argListToString(vector<Type> expected_types) {

    if(expressions.empty()) {
        return "()";
    }
    vector<ExpNode*> expressions_cpy = expressions;
    std::reverse(expressions_cpy.begin(),expressions_cpy.end());

    string result = "(";
    for(int i = 0 ; i < expressions_cpy.size() ; i++) {
        string var = expressions_cpy[i]->getVar();
        string type = typeToString(expressions_cpy[i]->type);
        if(expected_types[i].type == INT_TYPE ) {
            var = Zext(var, expressions_cpy[i]->type);
            type = typeToString(INT_TYPE);
        }
        result += type;
        result += var;
        result += ",";
    }
    result[result.length()-1] = ')';

    return result;
}


ExpNode* HandleCasting(TypeNode* result_type, ExpNode* exp) {
    if(exp->type != INT_TYPE and exp->type != BYTE_TYPE) {
        output::errorMismatch(exp->lineno);
        exit(0);
    }
    string result_var = exp->getVar();
    if(result_type->type == INT_TYPE) {
        result_var = Zext(result_var, exp->type);
    }
    else if(exp->type != BYTE_TYPE) {
        result_var = Trunc(result_var, result_type->type);
    }
    return new ExpNode(exp->lineno, result_type->type, result_var, false);

}