#include <iostream>
#include "antlr4-runtime.h"
#include "relop.h"

using namespace antlr4;
int main(int argc, const char* argv[])
{
    // std::cout << "Hello, world." << std::endl;
    std::string str;
    std::getline(std::cin, str);
    ANTLRInputStream input(str);
    relop lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (auto token : tokens.getTokens()) {
        //std::cout << token->getText() << std::endl;
        std::string text = token->getText();
        int length = token->getStopIndex() - token->getStartIndex() + 1;
        //std::cout << length << std::endl;
        if(text == "=" || text == ">" || text == "<" || text == ">=" || text == "<=" || text == "<>"){
            std::cout << "(relop," << text << ")";
        }
        else {
            if(length == 0) break;
            std::cout << "(other," << length << ")";
        }
    }
    
    return 0;
}