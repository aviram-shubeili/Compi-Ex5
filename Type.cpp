#include "Type.h"
#include <algorithm>

// TODO do we need this?
//template<typename Base, typename T>
//inline bool instanceof(const T*) {
//    return std::is_base_of<Base, T>::name;
//}
//
//bool isFunction(const Type& t) {
//    return instanceof<FunctionType>(&t);
//}

bool operator==(const Type& lhs, const Type& rhs) {
    // TODO: do we need to know if its a function or not or do we care only of the type and const
//    if((isFunction(*lhs) and not isFunction(*rhs)) or
//       (not isFunction(*lhs) and isFunction(*rhs))    )
//    {
//        return false;
//    }

    return (lhs.is_const == rhs.is_const) and (lhs.type == rhs.type);

}

std::string Type::toString() {
    std::string result;
    switch (type) { // TODO: handle function types.
        case INT_TYPE: result = "INT";
            break;
        case STRING_TYPE: result = "STRING";
            break;
        case BOOL_TYPE: result = "BOOL";
            break;
        case BYTE_TYPE: result = "BYTE";
            break;
        case VOID_TYPE: result = "VOID";
            break;
        default: assert(false);
    }
    if (not is_function) {
        return result;
    } else {
        std::vector<std::string> args = getArgumentsAsStrings();
        result = output::makeFunctionType(result, args);
        return result;
    }
}

bool operator!=(const Type &lhs, const Type &rhs) {
    return not (lhs == rhs);
}

std::vector<std::string> Type::getArgumentsAsStrings() {
    std::vector<std::string> result;
    for(Type& argument : arguments) {
        result.push_back(argument.toString());
    }
    return result;
}


bool operator<(const Symbol &lhs, const Symbol &rhs) {
    return (lhs.offset < rhs.offset);
}

bool hasSameArguments(std::vector<Type> expected, std::vector<Type> actual) {
    // actual args are inserted in reverse order.
    std::reverse(actual.begin(),actual.end());
    if(expected.size() != actual.size()) {
        return false;
    }
    for (int i = 0; i < expected.size(); i++) {
        if(expected[i].type != actual[i].type and not (expected[i].type == INT_TYPE and actual[i].type == BYTE_TYPE)) {
            return false;
        }
    }
    return true;
}
