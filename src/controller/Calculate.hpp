//
//  Calculate.hpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#ifdef __cplusplus
#include <stdio.h>
#include <iomanip>
#include "Variable.hpp"
#include "Token.hpp"

extern "C"
{
    class Calculate
    {
    private:
        std::string answer = "";
        std::vector<Token> token_stream;
        int runTime = 0;
        int iteration = 0;
        int firstParenthesesIndex = 0;
        int secondParenthesesIndex = 0;
        int numberIndex = 0;
        int negative = 0;
        int extraIterator = 0;
        int parenthesesPair = 0;
        int decimalPrecision = 0;
        long double tempRes = 0;
        bool isParentheses = false;
        bool isTrigger = false;
        bool isReady = true;
        bool isVariable = false;
        bool isPow = false;
    public:
        // Testing from here
        void addOperator(int& i, std::string& token, std::string& theNum, Variable& va);
        void addVariable(int& i, std::string& token, Variable& va);
        void addNumber(int& i, std::string& token, std::string& theNum);
        //
        const char* parsing(const char* input, Variable& v);
        void result();
        void checkForParenthesesPower();
        void checkForMultiplicationDivisionModulo();
        void checkForMinusPlus();
        void MultiplicationDivisionModulo(char op);
        void MinusPlus(char op);
        void foundOperatorFirst(char op);
        void insert(int index, Token token);
        void remove(int removeIndex, int removeTimes);
        void cleanUp(int whichFunction);
        bool isOperator(char op);
        std::string removeSpaces(std::string str);
        std::string setDecimalPoint(long double value);
    };
}
#endif

