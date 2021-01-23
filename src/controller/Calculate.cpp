//
//  Calculate.cpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#include "Calculate.hpp"
void Calculate::parsingError(Variable& va)
{
    if (temp != NULL)
    {
        va.addVariable(*temp);
        temp = NULL;
    }
    throw std::exception();
}

void Calculate::addNumber(int& i, std::string& token, std::string& theNum, Variable& va)
{
    bool isDecimal = false;
    for (int j = i; j < token.length(); j++)
    {
        if (isdigit(token.at(j)) || token.at(j) == '.')
        {
            /**---------------------
            Error more than one '.'
            ----------------------*/
            if (token.at(j) == '.' && isDecimal) throw std::exception();
            
            theNum += token.at(j);
            i++;
            if (token.at(j) == '.') isDecimal = true;
        }
        else break;
    }
    
    /**------------------
    Error only '.' in input
    ---------------------*/
    if (theNum == ".") parsingError(va);

    long double num = std::stold(theNum);
    token_stream.push_back(Token(num,number));
    theNum = "";
    i--;
}

void Calculate::declareVariable(int& i, std::string& token, Variable& va)
{
    if (((i + 1) < token.length() && token.at(i + 1) != '=') || (i + 1) == token.length())
    {
    
        /**-------------------------------------------------------------------------------------------------------
        Checks whether the variable has either numbers or variables on either one or both of its sides
        ---------------------------------------------------------------------------------------------------------*/
        if (i > 0 && (i + 1) < token.length() && (token.at(i - 1) == '.' || token.at(i - 1) == ')' || isdigit(token.at(i - 1))))
        {
            if ((token.at(i + 1) == '(' || isdigit(token.at(i + 1)) || va.isVariable(token.at(i + 1)))
                && (token.at(i - 1) == '.' || token.at(i - 1) == ')' || isdigit(token.at(i - 1))))
            {
                token_stream.push_back(Token('*'));
                token_stream.push_back(Token(va.getValue(token.at(i)), number));
                token_stream.push_back(Token('*'));
                i++;
                token.insert((i-1), "*");
                token.insert((i + 1), "*");
                i++;
            }
        }
        
        /**-----------------------------------------------------------------------------
        Checks whether the variable has either numbers or variables infront of it
        ---------------------------------------------------------------------------------*/
        else if ((i + 1) < token.length() && (token.at(i + 1) == '.' || token.at(i + 1) == '(' || isdigit(token.at(i + 1)) || va.isVariable(token.at(i + 1))))
        {
            token_stream.push_back(Token(va.getValue(token.at(i)), number));
            token_stream.push_back(Token('*'));
            token.insert((i + 1), "*");
            i++;
        }
        
        /**---------------------------------------------------------------------------
        Checks whether the variable has either numbers or variables behind it
        -------------------------------------------------------------------------------*/
        else if (i > 0 && (token.at(i - 1) == ')' || isdigit(token.at(i - 1)) || va.isVariable(token.at(i - 1))))
        {
            i++;
            token.insert((i - 1), "*");
            token_stream.push_back(Token('*'));
            token_stream.push_back(Token(va.getValue(token.at(i)), number));
        }
        
        /**----------------------------------------------------------------------------------
        Inserts the variables value as there are or need for the insertion of operators
        -------------------------------------------------------------------------------------*/
        else token_stream.push_back(Token(va.getValue(token.at(i)), number));
    }
}

void Calculate::addOperatorOrVariable(int& i, std::string& token, std::string& theNum, Variable& va)
{
    if (isOperator(token.at(i))) token_stream.push_back(Token(token.at(i)));
    else
    {
        if (i == 0 && !isVariable)
        {
            if (token.length() >= 3 && (isalpha(token.at(i)) && token.at(i + 1) == '=' && !isVariable))
            {
                isVariable = true;
                char name = token.at(i);
                token.erase(0, 2);
                theNum = std::string(parsing(token.c_str(), va));
                if (isReady)
                {
                    long double num = std::stold(theNum);
                    va.addVariable(Variable(name, num));
                    answer = name;
                    answer += " = ";
                    answer += theNum;
                    theNum = "";
                    i--;
                    token = "";
                    isReady = false;
                }
                isVariable = false;
            }
            /**---------------
            Error invalid input
            ------------------*/
            else parsingError(va);
        }
        /**----------------
        Error invalid input
        ------------------*/
        else parsingError(va);
    }
}

/**--------------------------------------------------------------------------
Receives the calculation as a string and parses the string into tokens
-----------------------------------------------------------------------------*/
const char* Calculate::parsing(const char* input, Variable& va)
{
    std::string theNum = "";
    std::string token(input);
    token = removeSpaces(input);
    
    /**-----------
    Error no input
    -------------*/
    if (token == "") throw std::exception();
    
    /**-----------------------------------------------------
    This is for redeclaring an already declared variable
    ---------------------------------------------------------*/
    if(va.isVariable(token.at(0)) && token.length() >= 3)
        if(token.at(1) == '=')
        {
            temp = va.getVar(token.at(0));
            va.removeVariable(token.at(0));
        }
    
    /**-----------------------------
    Iterates over the token stream
    --------------------------------*/
    for (int i = 0; i < token.length(); i++)
    {
        isdigit(token.at(i)) || token.at(i) == '.' ? addNumber(i, token, theNum, va) :
        va.isVariable(token.at(i)) && (i + 1) <= token.length() ? declareVariable(i, token, va) : addOperatorOrVariable(i, token, theNum, va);
    }
    operatorExponent();
    if (isReady) result();
    if (!isVariable) cleanUp(0);
    return answer.c_str();
}

/**-------------------------------------------------------------------------------------------------------------------------------------------
 Checks if the exponent's first char is a '+' or '-' and wraps a parentheses around the exponent's expression if it finds a case
 --------------------------------------------------------------------------------------------------------------------------------------------*/
void Calculate::operatorExponent()
{
    for (int i = 0; i < token_stream.size(); i++)
    {
        if (token_stream[i].getKind() == '^' && (i + 1) <= (token_stream.size() - 1) && (token_stream[i + 1].getKind() == '+' || token_stream[i + 1].getKind() == '-'))
        {
            token_stream.insert(token_stream.begin() + i + 1, Token('('));
            i += 2;
            for (int j = i; j < token_stream.size(); j++)
            {
                if (token_stream[j].getKind() == '^')
                {
                    token_stream.insert(token_stream.begin() + j + 1, Token(')'));
                    operatorExponent();
                    return;
                }
                
                if (token_stream[j].getKind() == '(')
                {
                    int parenthesesMatch = -1;
                    for (int k = j; k < token_stream.size(); k++)
                    {
                        if (token_stream[k].getKind() == '(') parenthesesMatch++;
                        
                        if (token_stream[k].getKind() == ')') parenthesesMatch--;
                        
                        if (parenthesesMatch < 0)
                        {
                            token_stream.insert(token_stream.begin() + k + 1, Token(')'));
                            operatorExponent();
                            return;
                        }
                    }
                }
                
                if (token_stream[j].getKind() == number)
                {
                    token_stream.insert(token_stream.begin() + j + 1, Token(')'));
                    operatorExponent();
                    return;
                }
            }
        }
    }
}

/**------------------------------------------------------------------------------
Calculates the token stream until there is only one token left/the answer
---------------------------------------------------------------------------------*/
void Calculate::result()
{
    while (token_stream.size() > 1 && isReady)
    {
        if (runTime == 0) ParenthesesPowerSyntax();
        
        else if (runTime == 1) MultiplicationDivisionModuloSyntax();
        
        else if (runTime == 2) MinusPlusSyntax();
    }
    
    if (isReady)
    {
        long double result = token_stream[0].getValue();
        if (result == -0 || result == +0) result = 0;
        answer = setDecimalPoint(result);
    }
}

/**-----------------------------------------------------
Handles the syntax for parentheses and power '^'
-------------------------------------------------------*/
void Calculate::parenthesesCheck()
{
    for (int i = iteration; i < token_stream.size(); i++)
    {

        if (token_stream[i].getKind() == '^' && !isParentheses)
        {
            isPow = true;
            iteration = i - 1;
        }
        
        if (token_stream[i].getKind() == '(' && secondParenthesesIndex <= 0) firstParenthesesIndex = i;
        
        if (token_stream[i].getKind() == '(') parenthesesPair++;
        
        if (token_stream[i].getKind() == ')' && secondParenthesesIndex <= 0)
        {
            secondParenthesesIndex = i;
            isParentheses = true;
        }
        
        if (token_stream[i].getKind() == ')') parenthesesPair--;
    }
    
    /**-----------------------
    Error open parentheses
    --------------------------*/
    if (parenthesesPair != 0) throw std::exception();
}

void Calculate::ParenthesesPowerSyntax()
{
    if (isReady)
    {
        unsigned long length = token_stream.size() - 1;
        switch (token_stream[iteration].getKind())
        {
            case ')':
                /**----------------------
                Error found ')' before '('
                -------------------------*/
                throw std::exception();
                
            case '(':
                parenthesesCheck();
                break;
                
            case '^':
                for (unsigned long i = length; i >= 0; i--)
                {
                    if (token_stream[i].getKind() == '^')
                    {
                        /**---------------------------------------------------------------------------------------------------------------------------------------------------------
                        Checks for number ^ number and incase the right hand side number has a further exponent it continues e.g. number ^ number ^ number
                        -----------------------------------------------------------------------------------------------------------------------------------------------------------*/
                        if (token_stream[i - 1].getKind() == number && token_stream[i + 1].getKind() == number)
                        {
                            tempRes = pow(token_stream[i - 1].getValue(), token_stream[i + 1].getValue());
                            remove(i - 1, 3);
                            insert(i - 1, Token(tempRes, number));
                            cleanUp(1);
                            isPow = false;
                            return;
                        }
                        
                        else if (token_stream[i - 1].getKind() == number && token_stream[i + 1].getKind() == '(')
                        {
                            cleanUp(1);
                            isPow = false;
                            iteration = i + 1;
                            ParenthesesPowerSyntax();
                            break;
                        }
                        /**----------------------------------------
                         Skip to the next '^' if:    (...) ^ number
                         ----------------------------------------*/
                        else if (token_stream[i - 1].getKind() == ')' && token_stream[i + 1].getKind() == number) {}
                        
                        else if (token_stream[i - 1].getKind() == ')' && token_stream[i + 1].getKind() == '(')
                        {
                            cleanUp(1);
                            isPow = false;
                            iteration = i + 1;
                            ParenthesesPowerSyntax();
                            break;
                        }
                        
                        else if (token_stream[i - 1].getKind() == number && (token_stream[i + 1].getKind() == '+' || token_stream[i + 1].getKind() == '-') && token_stream[i + 2].getKind() == number)
                        {
                            cleanUp(1);
                            isPow = false;
                            iteration = i + 1;
                            foundOperatorFirst(token_stream[i + 1].getKind());
                            iteration = i - 1;
                            break;
                        }
                        
                        else if (token_stream[i - 1].getKind() == number && (token_stream[i + 1].getKind() == '+' || token_stream[i + 1].getKind() == '-') && token_stream[i + 2].getKind() == '(')
                        {
                            cleanUp(1);
                            isPow = false;
                            iteration = i + 2;
                            ParenthesesPowerSyntax();
                            break;
                        }
                        
                        else if (token_stream[i - 1].getKind() == ')' && (token_stream[i + 1].getKind() == '+' || token_stream[i + 1].getKind() == '-') && token_stream[i + 2].getKind() == '(')
                        {
                            cleanUp(1);
                            isPow = false;
                            iteration = i + 2;
                            ParenthesesPowerSyntax();
                            break;
                        }
                        
                        /**----------------------------------------------------
                         Skip to the next '^' if:    (...) ^ minus/plus number
                         ------------------------------------------------------*/
                        else if (token_stream[i - 1].getKind() == ')' && (token_stream[i + 1].getKind() == '+' || token_stream[i + 1].getKind() == '-') && token_stream[i + 2].getKind() == number) {}
                        
                        /**---------------------
                        Error invalid input to '^'
                        ------------------------*/
                        else throw std::exception();
                    }
                }
                
                /**-------------------------
                 Error empty parentheses
                 ---------------------------*/
                if (secondParenthesesIndex-firstParenthesesIndex == 1) throw std::exception();
                
                /**------------------------------------------------
                 Error empty parentheses with operator inside
                 --------------------------------------------------*/
                if (secondParenthesesIndex-firstParenthesesIndex == 2 && isOperator(token_stream[firstParenthesesIndex + 1].getKind())) throw std::exception();
                
                
                if (token_stream[firstParenthesesIndex + 1].getKind() == number && secondParenthesesIndex-firstParenthesesIndex == 2)
                {
                    tempRes = token_stream[firstParenthesesIndex + 1].getValue();
                    remove(firstParenthesesIndex, 3);
                    insert(firstParenthesesIndex, Token(tempRes, number));
                    
                    if (firstParenthesesIndex - 1 >= 0 && (token_stream[firstParenthesesIndex - 1].getKind() == number && token_stream[firstParenthesesIndex + 1].getKind() == number))
                    {
                        insert(firstParenthesesIndex, Token('*'));
                        insert(firstParenthesesIndex + 2, Token('*'));
                        cleanUp(1);
                        return;
                    }
                    
                    if (firstParenthesesIndex - 1 >= 0 && token_stream[firstParenthesesIndex - 1].getKind() == number && !isTrigger)
                    {
                        insert(firstParenthesesIndex, Token('*'));
                        isTrigger = true;
                    }
                    
                    if ((token_stream[firstParenthesesIndex + 1].getKind() == number && !isTrigger) || (token_stream[firstParenthesesIndex + 1].getKind() == '(' && !isTrigger)) insert(firstParenthesesIndex + 1, Token('*'));
                    cleanUp(1);
                    return;
                }
                
                if ((iteration >= length && iteration != -1) || (isParentheses && !isPow))
                {
                    if (isParentheses) iteration = firstParenthesesIndex;
                    else iteration = 0;
                    runTime++;
                    if (runTime > 0) runTime = 1;
                    return;
                }
        }
            
        
        /**-------------------------
         Error empty parentheses
         ---------------------------*/
        if (secondParenthesesIndex-firstParenthesesIndex == 1) throw std::exception();
        
        /**------------------------------------------------
         Error empty parentheses with operator inside
         --------------------------------------------------*/
        if (secondParenthesesIndex-firstParenthesesIndex == 2 && isOperator(token_stream[firstParenthesesIndex + 1].getKind())) throw std::exception();
        
        if (token_stream[firstParenthesesIndex + 1].getKind() == number && secondParenthesesIndex-firstParenthesesIndex == 2)
        {
            tempRes = token_stream[firstParenthesesIndex + 1].getValue();
            remove(firstParenthesesIndex, 3);
            insert(firstParenthesesIndex, Token(tempRes, number));
            
            if (firstParenthesesIndex - 1 >= 0 && (token_stream[firstParenthesesIndex - 1].getKind() == number && token_stream[firstParenthesesIndex + 1].getKind() == number))
            {
                insert(firstParenthesesIndex, Token('*'));
                insert(firstParenthesesIndex + 2, Token('*'));
                cleanUp(1);
                return;
            }
            
            if (firstParenthesesIndex - 1 >= 0 && token_stream[firstParenthesesIndex - 1].getKind() == number && !isTrigger)
            {
                insert(firstParenthesesIndex, Token('*'));
                isTrigger = true;
            }
            
            if ((token_stream[firstParenthesesIndex + 1].getKind() == number && !isTrigger) || (token_stream[firstParenthesesIndex + 1].getKind() == '(' && !isTrigger)) insert(firstParenthesesIndex + 1, Token('*'));
            cleanUp(1);
            return;
        }
        
        if ((iteration >= length && iteration != -1) || (isParentheses && !isPow))
        {
            if (isParentheses) iteration = firstParenthesesIndex;
            else iteration = 0;
            runTime++;
            if (runTime > 0) runTime = 1;
            return;
        }
        iteration++;
    }
}

/**--------------------------------------
 Searches for '*', '/' and '%' (modulo)
 ---------------------------------------*/
void Calculate::MultiplicationDivisionModuloSyntax()
{
    if (isReady)
    {
        unsigned long length = token_stream.size() - 1;
        
        /**----------------------------------------------
         Error '*', '/' and '%' missing number infront
         -----------------------------------------------*/
        if (token_stream[0].getKind() == '*' || token_stream[0].getKind() == '/' || token_stream[0].getKind() == '%') throw std::exception();
        
        switch (token_stream[iteration].getKind())
        {
            case '*': case '/': case '%':
                MultiplicationDivisionModulo(token_stream[iteration].getKind());
                if (isParentheses == true) iteration = firstParenthesesIndex;
                else iteration = 0;
                break;
        }
        if ((isParentheses && iteration >= secondParenthesesIndex) || (!isParentheses && iteration >= length))
        {
            if (isParentheses) iteration = firstParenthesesIndex;
            else iteration = 0;
            runTime++;
            if (runTime > 1) runTime = 2;
            return;
        }
        iteration++;
    }
}

/**-------------------
 Searches for '+', '-'
 ---------------------*/
void Calculate::MinusPlusSyntax()
{
    if (isReady)
    {
        unsigned long length = token_stream.size() - 1;
        
        /**--------------------------------------
         Error '+', '-' missing number behind
         ----------------------------------------*/
        if (token_stream[length].getKind() == '+' || token_stream[length].getKind() == '-') throw std::exception();
        
        switch (token_stream[iteration].getKind())
        {
            case '-': case '+':
                if ((isParentheses && iteration == firstParenthesesIndex + 1) || (!isParentheses && iteration == 0)) foundOperatorFirst(token_stream[iteration].getKind());
                else
                {
                    MinusPlus(token_stream[iteration].getKind());
                    if (isParentheses) iteration = firstParenthesesIndex - 1;
                    else iteration = - 1;
                }
                break;
        }
        if ((isParentheses && iteration == secondParenthesesIndex) || (!isParentheses && iteration >= length))
        {
            iteration = 0;
            runTime = 0;
            isParentheses = false;
            return;
        }
        iteration++;
    }
}

/**----------------------------------------
 Calculates '*', '/' and '%' expressions
 ------------------------------------------*/
void Calculate::MultiplicationDivisionModulo(char op)
{
    if (isReady)
    {
        unsigned long length = token_stream.size() - 1;
        while(1)
        {
            /**---------------------------------------------------------------------------------------
             Error '*', '/' or '%' followed by operator of same priority e.g. number * * number
             -----------------------------------------------------------------------------------------*/
            if (extraIterator > 0 && (token_stream[iteration + extraIterator].getKind() == '*' || token_stream[iteration + extraIterator].getKind() == '/' || token_stream[iteration + extraIterator].getKind() == '%')) throw std::exception();
            
            if (token_stream[iteration + extraIterator].getKind() == number)
            {
                numberIndex = iteration + extraIterator;
                extraIterator = 0;
                break;
            }
            
            /**---------------------------------------------
             Error '*', '/' or '%' missing number behind
             ----------------------------------------------*/
            if (isParentheses) if (iteration + extraIterator == secondParenthesesIndex) throw std::exception();
            
            /**---------------------------------------------
             Error '*', '/' or '%' missing number behind
             ----------------------------------------------*/
            if (!isParentheses) if (iteration + extraIterator == length) throw std::exception();
            
            if (token_stream[iteration + extraIterator].getKind() == '-') negative++;
            
            extraIterator++;
        }
        /**---------------------------------------------
         Error '*', '/' or '%' missing number infront
         ----------------------------------------------*/
        if (isOperator(token_stream[iteration - 1].getKind()) && token_stream[iteration - 1].getKind() != ')') throw std::exception();
        
        if (negative == 0 || (negative % 2 == 0 && negative != 0))
        {
            if (op == '*') tempRes = token_stream[iteration - 1].getValue() * token_stream[numberIndex].getValue();
            
            if (op == '/')
            {
                /**------------------------------
                 Error undefined division by 0
                 --------------------------------*/
                if (token_stream[numberIndex].getValue() == 0) throw std::exception();
                
                tempRes = token_stream[iteration - 1].getValue() / token_stream[numberIndex].getValue();
            }
            
            if (op == '%')
            {
                if (floor(token_stream[iteration - 1].getValue()) == token_stream[iteration - 1].getValue() && floor(token_stream[numberIndex].getValue()) == token_stream[numberIndex].getValue())
                {
                    if (token_stream[numberIndex].getValue() == 0) tempRes = (int) token_stream[iteration - 1].getValue();
                    
                    else tempRes = (int) token_stream[iteration - 1].getValue() % (int) token_stream[numberIndex].getValue();
                }
                /**--------------------------------------------
                 Error '%' only works with whole numbers
                 ----------------------------------------------*/
                else throw std::exception();
            }
        }
        
        if (negative % 2 == 1 && negative != 0)
        {
            if (op == '*')
            {
                tempRes = - token_stream[iteration - 1].getValue() * token_stream[numberIndex].getValue();
                negative = 0;
            }
            
            if (op == '/')
            {
                /**------------------------------
                 Error undefined division by 0
                 --------------------------------*/
                if (token_stream[numberIndex].getValue() == 0) throw std::exception();
                
                tempRes = - token_stream[iteration - 1].getValue() / token_stream[numberIndex].getValue();
                negative = 0;
            }
            
            if (op == '%')
            {
                if (floor(token_stream[iteration - 1].getValue()) == token_stream[iteration - 1].getValue() && floor(token_stream[numberIndex].getValue()) == token_stream[numberIndex].getValue())
                {
                    if (token_stream[numberIndex].getValue() == 0) tempRes = (int) token_stream[iteration - 1].getValue();
                    
                    else tempRes = (int) token_stream[iteration - 1].getValue() % (int) token_stream[numberIndex].getValue();
                    negative = 0;
                }
                
                /**--------------------------------------------
                 Error '%' only works with whole numbers
                 ----------------------------------------------*/
                else throw std::exception();
            }
        }
        
        if (isParentheses && (token_stream[iteration - 2].getKind() == '(' && token_stream[numberIndex + 1].getKind() == ')'))
        {
            remove(iteration - 2, (numberIndex + 4 - iteration));
            insert(iteration - 2, Token(tempRes, number));
            if ((iteration - 3 >= 0 && (token_stream[iteration - 3].getKind() == ')' || token_stream[iteration - 3].getKind() == number) && (token_stream[iteration - 1].getKind() == '(' || token_stream[iteration - 1].getKind() == number)))
            {
                insert(iteration - 2, Token('*'));
                insert(iteration, Token('*'));
                isTrigger = true;
            }
            if (!isTrigger && iteration - 2 >= 0 && (token_stream[iteration - 1].getKind() == number || token_stream[iteration - 1].getKind() == '('))
                insert(iteration - 1, Token('*'));

            if (!isTrigger && token_stream[iteration - 1].getKind() != '*' && iteration - 2 > 0 && (token_stream[iteration - 3].getKind() == number|| token_stream[iteration - 3].getKind() == ')'))
                insert(iteration - 2, Token('*'));
                
            cleanUp(2);
        }
        
        if (!isParentheses || !isTrigger)
        {
            remove(iteration - 1, numberIndex + 2 - iteration);
            insert(iteration - 1, Token(tempRes, number));
            cleanUp(3);
        }
        cleanUp(4);
    }
}

/**------------------------------------
 Calculates '+' and  '-' expressions
 --------------------------------------*/
void Calculate::MinusPlus(char op)
{
    if (isReady)
    {
        unsigned long length = token_stream.size() - 1;
        if (op == '-') negative++;
        
        while (1)
        {
            extraIterator++;
            /**----------------------------------------
             Error '+' or '-' missing number behind
             ------------------------------------------*/
            if (token_stream[iteration + extraIterator].getKind() == ')') throw std::exception();
            
            if (token_stream[iteration + extraIterator].getKind() == '-') negative++;
            
            if (token_stream[iteration + extraIterator].getKind() == number)
            {
                numberIndex = iteration + extraIterator;
                extraIterator = 0;
                break;
            }
            /**---
             Error
             ----*/
            if (iteration + extraIterator == length) throw std::exception();
        }
        
        if (negative == 0)
        {
            if (op == '-') tempRes = token_stream[iteration - 1].getValue() - token_stream[numberIndex].getValue();
            
            if (op == '+') tempRes = token_stream[iteration - 1].getValue() + token_stream[numberIndex].getValue();
        }
        
        if (negative % 2 == 1 && negative != 0) tempRes = token_stream[iteration - 1].getValue() - token_stream[numberIndex].getValue();
        
        if (negative % 2 == 0 && negative != 0) tempRes = token_stream[iteration - 1].getValue() + token_stream[numberIndex].getValue();
        
        if (isParentheses && (token_stream[iteration - 2].getKind() == '(' && token_stream[numberIndex + 1].getKind() == ')'))
        {
            remove(iteration - 2, numberIndex + 4 - iteration);
            insert(iteration - 2, Token(tempRes, number));
            if (iteration - 3 >= 0 && (token_stream[iteration - 1].getKind() == '(' || token_stream[iteration - 1].getKind() == number) && (token_stream[iteration - 3].getKind() == ')' || token_stream[iteration - 3].getKind() == number))
            {
                insert(iteration - 2, Token('*'));
                insert(iteration, Token('*'));
                isTrigger = true;
            }
            if (!isTrigger && iteration - 2 >= 0 && (token_stream[iteration - 1].getKind() == number || token_stream[iteration - 1].getKind() == '('))
                insert(iteration - 1, Token('*'));
            
            if (!isTrigger && token_stream[iteration - 2].getKind() != '*' && iteration - 2 > 0 && (token_stream[iteration - 3].getKind() == number|| token_stream[iteration - 3].getKind() == ')'))
                insert(iteration - 2, Token('*'));
            
            cleanUp(5);
            return;
        }
        
        if (!isParentheses || !isTrigger)
        {
            remove(iteration - 1, numberIndex + 2 - iteration);
            
            if (iteration == 0) insert(iteration, Token(tempRes, number));
            
            if (iteration > 0) insert(iteration - 1, Token(tempRes, number));
        }
        cleanUp(4);
    }
}

/**---------------------------------------------------------------
Found '+' or '-' before a number e.g. - number or + number
------------------------------------------------------------------*/
void Calculate::foundOperatorFirst(char op)
{
    if (isReady)
    {
        if (op == '-') negative++;
        
        if (!isParentheses)
        {
            while (token_stream[iteration + extraIterator].getKind() != number)
            {
                extraIterator++;
                if (token_stream[iteration + extraIterator].getKind() == '-') negative++;
            }
        }
        
        if (isParentheses)
        {
            while (token_stream[iteration + extraIterator].getKind() != number && token_stream[iteration + extraIterator].getKind() != ')')
            {
                extraIterator++;
                if (token_stream[iteration + extraIterator].getKind() == '-') negative++;
            }
        }
        
        numberIndex = iteration + extraIterator;
        
        /**----------------------------------------
         Error '+' or '-' missing number behind
         ------------------------------------------*/
        if (token_stream[numberIndex].getKind() != number) throw std::exception();
        
        if (negative % 2 == 1) tempRes = - token_stream[numberIndex].getValue();
        
        if (negative % 2 == 0) tempRes = token_stream[numberIndex].getValue();
        
        if (!isParentheses)
        {
            if (iteration == 0) remove(0, numberIndex + 1);
            else remove(iteration, numberIndex + 1 - iteration);
        }
        
        if (isParentheses && secondParenthesesIndex - (iteration - 1) == 3) remove(iteration - 1, numberIndex + 3 - iteration);
        
        if (isParentheses && secondParenthesesIndex - (iteration - 1) != 3) remove(iteration, numberIndex + 1 - iteration);
        
        if (!isParentheses)
        {
            if (iteration == 0) insert(0, Token(tempRes, number));
            else insert(iteration, Token(tempRes, number));
        }
        
        if (isParentheses && secondParenthesesIndex - (iteration - 1) == 3)
        {
            insert(iteration - 1, Token(tempRes, number));
            
            if (token_stream[iteration].getKind() == number || token_stream[iteration].getKind() == '(') insert(iteration, Token('*'));
            
            if (token_stream[iteration - 2].getKind() == number) insert(iteration - 1, Token('*'));
            
            isParentheses = false;
            firstParenthesesIndex = - 1;
            secondParenthesesIndex = - 2;
            iteration = - 1;
            runTime = 0;
        }
        
        if (isParentheses && secondParenthesesIndex - (iteration - 1) != 3)
        {
            insert(iteration, Token(tempRes, number));
            firstParenthesesIndex = - 1;
            secondParenthesesIndex = - 2;
        }
        cleanUp(4);
    }
}

/**------------------------------
 Inserts tokens into the vector
 --------------------------------*/
void Calculate::insert(int index, Token token)
{
    token_stream.insert(token_stream.begin() + index, token);
}

/**------------------------------
 Removes tokens from the vector
 --------------------------------*/
void Calculate::remove(int removeIndex, int removeTimes)
{
    for (int i = 0; i < removeTimes; i++) token_stream.erase(token_stream.begin() + removeIndex);
}

/**---------------------------------------
 Resets variables depending on input
 -----------------------------------------*/
void Calculate::cleanUp(int whichFunction)
{
    switch (whichFunction)
    {
        case 0:
            token_stream.clear();
            runTime = 0;
            firstParenthesesIndex = 0;
            secondParenthesesIndex = 0;
            iteration = 0;
            isParentheses = false;
            isTrigger = false;
            extraIterator = 0;
            numberIndex = 0;
            negative = 0;
            tempRes = 0;
            parenthesesPair = 0;
            isReady = true;
            isVariable = false;
            isPow = false;
            break;
        case 1:
            iteration = 0;
            isParentheses = false;
            isTrigger = false;
            firstParenthesesIndex = -1;
            secondParenthesesIndex = -1;
            break;
        case 2:
            isTrigger = true;
            secondParenthesesIndex = - 1;
            firstParenthesesIndex = - 1;
            break;
        case 3:
            firstParenthesesIndex = - 1;
            secondParenthesesIndex = - 1;
            break;
        case  4:
            extraIterator = 0;
            isTrigger = false;
            numberIndex = 0;
            negative = 0;
            tempRes = 0;
            break;
        case 5:
            extraIterator = 0;
            numberIndex = 0;
            negative = 0;
            tempRes = 0;
            isTrigger = false;
            firstParenthesesIndex = - 1;
            secondParenthesesIndex = - 1;
            isParentheses = false;
            runTime = 0;
            break;
        default:
            break;
    }
}

/**----------------------------
Checks if char is an operator
-------------------------------*/
bool Calculate::isOperator(char op)
{
    return (op == '(' || op == ')' || op == '*' || op == '/' || op == '+' || op == '-' || op == '%' || op == '^');
}

/**------------------------------------------------------------
Function to remove spaces, found on geeksforgeeks.org
---------------------------------------------------------------*/
std::string Calculate::removeSpaces(std::string str)
{
    std::stringstream ss;
    std::string temp;
    ss << str;
    str = "";
    while (!ss.eof())
    {
        ss >> temp;
        str = str + temp;
    }
    return str;
}

std::string Calculate::setDecimalPoint(long double value)
{
    std::ostringstream streamObj1;
    std::string ans;
    /**-----------------------------------
     Can use: std::fixed, std::scientific
     ------------------------------------*/
    streamObj1 << std::defaultfloat;
    
    /**----------------------
     Add double to stream
     -----------------------*/
    streamObj1 << value;
    ans = streamObj1.str();
    return ans;
}

