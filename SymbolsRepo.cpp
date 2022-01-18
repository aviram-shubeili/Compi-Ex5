
#include "SymbolsRepo.h"
#include <cassert>

SymbolsRepo::SymbolsRepo() : loopCounter(0) {}

void SymbolsRepo::Initialize() {
    offsetStack.push_back(0);
    symbolMapStack.emplace_back();
    insertSymbol("print",  Type(VOID_TYPE, {Type(STRING_TYPE)} ));
    insertSymbol("printi",  Type( VOID_TYPE, {Type(INT_TYPE)} ));
}

void SymbolsRepo::openNewScope() {
    symbolMapStack.emplace_back();
    assert(not offsetStack.empty());
    offsetStack.push_back((offsetStack.back()));
}

void SymbolsRepo::insertSymbol(std::string name, Type type) {
    if (Contains(name)) {
        throw SymbolAlreadyDefinedInScope();
    }
    if(type.is_function) {
        symbolMapStack.back().push_back(Symbol(name,type ,FUNC_OFFSET));
    }
    else {
        symbolMapStack.back().push_back(Symbol(name,type ,offsetStack.back()++));
    }
}

void SymbolsRepo::insertSymbolAsLiteral(std::string name, Type type, std::string value){
    if (Contains(name)) {
        throw SymbolAlreadyDefinedInScope();
    }
    symbolMapStack.back().push_back(Symbol(name,type ,LITERAL,value));
}


void SymbolsRepo::closeScope() {
    output::endScope();
    SymbolMap::iterator it;
    for (Symbol& symbol : symbolMapStack.back())
    {
        output::printID(symbol.name, symbol.offset, symbol.type.toString());
    }
    symbolMapStack.pop_back();
    offsetStack.pop_back();
}

Symbol SymbolsRepo::findSymbol(const std::string symbol_name) {
    for(SymbolMap& currentSymbolMap : symbolMapStack ) {
        for (Symbol &symbol : currentSymbolMap) {
            if (symbol.name == symbol_name) {
                return symbol;
            }
        }
    }
    throw SymbolNotFound();
}


// make SymbolsRepo singleton
SymbolsRepo &SymbolsRepo::Instance() {
    static SymbolsRepo instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
}

bool SymbolsRepo::Contains(std::string symbol_name) {
    try {
        findSymbol(symbol_name);
        return true;
    }
    catch( SymbolNotFound& e) {
        return false;
    }

}

void SymbolsRepo::insertSymbolAsArgument(std::string name, Symbol sym) {
    assert(sym.offset < 0); // TODO
    symbolMapStack.back().push_back(sym);
}
