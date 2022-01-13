
#ifndef COMPI_EX3_SYMBOLSREPO_H
#define COMPI_EX3_SYMBOLSREPO_H
#include <vector>
#include <map>
#include "Type.h"
#include "hw3_output.hpp"
#define FUNC_OFFSET 0
class SymbolsRepo {
private:
     typedef std::vector<Symbol> SymbolMap;
     std::vector<int> offsetStack ;
     std::vector<SymbolMap> symbolMapStack;
     unsigned int loopCounter;
     SymbolsRepo();
public:
    std::string currentFunctionName;
    static SymbolsRepo& Instance();
    SymbolsRepo(SymbolsRepo const&)      = delete; // disable copy ctor
    void operator=(SymbolsRepo const&)  = delete; // disable = operator
    void openLoop() { loopCounter++; }
    void closeLoop() { loopCounter--; }
    bool isInLoop() { return loopCounter > 0; }
    void Initialize();
    void openNewScope();
    /**
 * Throws SymbolAlreadyDefinedInScope if symbol already defined in this scope
 * @param symbol_name
 * @return
 */
    void insertSymbol(std::string name, Type type );
    void insertSymbolAsArgument(std::string name, Symbol sym );
    void closeScope();

    /**
     * Throws SymbolNotFound if symbol not found.
     * @param symbol_name
     * @return
     */
    Symbol findSymbol(const std::string symbol_name);

    bool Contains(std::string symbol_name);
    };

class SymbolException : public std::exception {};

class SymbolNotFound : public SymbolException {
};

class SymbolAlreadyDefinedInScope : public SymbolException {
};


#endif //COMPI_EX3_SYMBOLSREPO_H
