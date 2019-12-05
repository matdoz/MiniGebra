//
//  Calculate.cpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#include "Calculate.hpp"

/* Mottar mattestykket i en string og parser stringen til en vektor med tokens */
const char* calculate::parsing(const char* input, Variable& va)
{
    va.setCreated(0);
    std::string theNum = "";
    token = removeSpaces(input);
    
    /* Error ingenting ble skrevet inn. */
    if (token == "")
        throw std::exception();
    
    for (int i = 0; i < token.length(); i++)
    {
        /* Sjekker om det som er skrevet inn er et tall, ett '.' eller en variabel, og i tilfelle variabel at det ikke er en variabel deklarasjon. */
        if (isdigit(token.at(i)) || token.at(i) == '.' || (va.isVariable(token.at(i)) && i + 1 < token.length() && token.at(i + 1) != '=') || (va.isVariable(token.at(i)) && token.length() <= i + 1))
        {
            if (va.isVariable(token.at(i)))
            {
                bool isInserted = false;
                /* Sjekker om variabelen har tall eller variable på begge eller én av sidene sine. */
                if ((i + 1 < token.length() &&
                    (token.at(i + 1) == '.' ||
                     token.at(i + 1) == '(' ||
                     isdigit(token.at(i + 1))) &&
                    i > 0 &&
                    (token.at(i - 1) == '.' ||
                    token.at(i - 1) == ')' ||
                     isdigit(token.at(i - 1)) ||
                     va.isVariable(token.at(i - 1)))) && (!isOperator(token_stream[i].getKind()) && !isOperator(token_stream[i - 1].getKind()) && !isOperator(token_stream[i + 1].getKind())))
                {
                    token_stream.push_back(Token('*'));
                    token_stream.push_back(Token(va.getValue(token.at(i)), number));
                    token_stream.push_back(Token('*'));
                    isInserted = true;
                }
                
                if (!isInserted && i + 1 < token.length() && (token.at(i + 1) == '.' || token.at(i + 1) == '(' || isdigit(token.at(i + 1)) || va.isVariable(token.at(i + 1))))
                {
                    if (token_stream[token_stream.size() - 1].getKind() == number && token_stream.size() > 0)
                        token_stream.push_back(Token('*'));
                    
                    token_stream.push_back(Token(va.getValue(token.at(i)), number));
                    token_stream.push_back(Token('*'));
                    isInserted = true;
                }
                
                if (!isInserted && i > 0 && (token.at(i - 1) == '.' || token.at(i - 1) == ')' || isdigit(token.at(i - 1))))
                    token_stream.push_back(Token('*'));
                
                if (!isInserted)
                    token_stream.push_back(Token(va.getValue(token.at(i)), number));
            }
            
            else
            {
                bool isDecimal = false;
                for (int j = i; j < token.length(); j++)
                {
                    if (isdigit(token.at(j)) || token.at(j) == '.')
                    {
                        if (isDecimal) decimalPrecision++;

                        /* Error flere enn ett '.'. */
                        if (token.at(j) == '.' && isDecimal)
                            throw std::exception();
                        
                        theNum += token.at(j);
                        i++;
                        if (token.at(j) == '.') isDecimal = true;
                    }
                    
                    else break;
                }
                
                /* Error bare '.' ble skrevet inn. */
                if (theNum == ".")
                    throw std::exception();

                /* stod konverteter string til double. */
                long double num = std::stold(theNum);
                token_stream.push_back(Token(num,number));
                theNum = "";
                i--;
            }
        }
        
        else
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
                            va.setCreated(1);
                            theNum = "";
                            i--;
                            token = "";
                            isReady = false;
                        }
                        isVariable = false;
                    }
                    /* Ugyldig input */
                    else throw std::exception();
                }
                /* Ugyldig input */
                else throw std::exception();
            }
        }
    }
    if (isReady) result();
    if (!isVariable) cleanUp(0);
    return answer.c_str();
}

/* Opererer på vektoren med tokens frem til det kun er ett tall token igjen i vektoren */
void calculate::result()
{
    while (token_stream.size() > 1 && isReady)
    {
        if (runTime == 0) checkForParenthesesPower();
        
        if (runTime == 1) checkForMultiplicationDivisionModulo();
        
        if (runTime == 2) checkForMinusPlus();
    }
    if (isReady)
    {
        long double result = token_stream[0].getValue();
        if (result == -0 || result == +0) result = 0;
        answer = setDecimalPoint(result);
    }
}

/* Leter etter parentes og potens */
void calculate::checkForParenthesesPower()
{
    if (isReady) {
        unsigned long length = token_stream.size() - 1;
        switch (token_stream[iteration].getKind())
        {
            case ')':
                /* Error fant ')' før '(' */
                throw std::exception();
            case '(':
                for (int j = iteration; j < token_stream.size(); j++)
                {
                    if (token_stream[j].getKind() == '^' && !isParentheses)
                        isPow = true;
                    
                    if (token_stream[j].getKind() == '(' && secondParenthesesIndex <= 0) firstParenthesesIndex = j;
                    
                    if (token_stream[j].getKind() == '(')
                        parenthesesPair++;
                    
                    if (token_stream[j].getKind() == ')' && secondParenthesesIndex <= 0)
                    {
                        secondParenthesesIndex = j;
                        isParentheses = true;
                    }
                    
                    if (token_stream[j].getKind() == ')')
                        parenthesesPair--;
                }
                
                /* Error åpen parentes */
                if (parenthesesPair != 0)
                    throw std::exception();
                
                break;
            case '^':
                if (isPow)
                {
                    isPow = false;
                    firstParenthesesIndex = - 1;
                    secondParenthesesIndex = - 1;
                    isParentheses = false;
                }
                
                if (token_stream[iteration - 1].getKind() == number && token_stream[iteration + 1].getKind() == number)
                {
                    if (token_stream[iteration + 2].getKind() == '^')
                    {
                        iteration += 2;
                        checkForParenthesesPower();
                        iteration = - 1;
                        break;
                    }
                    else
                    {
                        tempRes = pow(token_stream[iteration - 1].getValue(), token_stream[iteration + 1].getValue());
                        remove(iteration - 1, 3);
                        insert(iteration - 1, Token(tempRes, number));
                        iteration = - 1;
                        break;
                    }
                }
                
                if (token_stream[iteration - 1].getKind() == number && token_stream[iteration + 1].getKind() == '(')
                {
                    int wait = 0;
                    for (int i = iteration; i < length; i++)
                    {
                        if (token_stream[i].getKind() == '(')
                            wait++;
                        
                        if (token_stream[i].getKind() == ')' && wait == 0)
                        {
                            if (token_stream[i + 1].getKind() == '^')
                            {
                                iteration = i + 1;
                                checkForParenthesesPower();
                                iteration = - 1;
                                break;
                            }
                        }
                        if (token_stream[i].getKind() == ')')
                            wait--;
                    }
                    iteration++;
                    checkForParenthesesPower();

                    while (isPow)
                        checkForParenthesesPower();
                    
                    iteration = - 1;
                    break;
                }
                if ((token_stream[iteration + 1].getKind() == '+' || token_stream[iteration + 1].getKind() == '-') && token_stream[iteration + 2].getKind() == '(' && token_stream[iteration - 1].getKind() == number)
                {
                    iteration += 2;
                    checkForParenthesesPower();
                    iteration = - 1;
                    break;
                }
                if ((token_stream[iteration - 1].getKind() == number && token_stream[iteration + 2].getKind() == number) && (token_stream[iteration + 1].getKind() == '+' || token_stream[iteration + 1].getKind() == '-'))
                {
                    iteration++;
                    foundOperatorFirst(token_stream[iteration].getKind());
                    iteration = - 1;
                    break;
                }
                if (token_stream[iteration - 1].getKind() == ')' && (token_stream[iteration + 1].getKind() == '+' || token_stream[iteration + 1].getKind() == '-' || token_stream[iteration + 1].getKind() == number || token_stream[iteration + 1].getKind() == '('))
                {
                    int wait = 0;
                    for (int i = iteration; i > 0; i--)
                    {
                        if (token_stream[i].getKind() == '(')
                            wait++;
                        
                        if (token_stream[i].getKind() == '(')
                        {
                            iteration = i;
                            while (token_stream[i].getKind() == '(' && wait == 0)
                            {
                                if (runTime == 0) checkForParenthesesPower();
                                if (runTime == 1) checkForMultiplicationDivisionModulo();
                                if (runTime == 2) checkForMinusPlus();
                            }
                            if (token_stream[i + 1].getKind() == '^')
                            {
                                iteration = i + 1;
                                checkForParenthesesPower();
                                
                                while (isPow)
                                    checkForParenthesesPower();
                                
                                iteration = - 1;
                                break;
                            }
                            break;
                        }
                        
                        if (token_stream[i].getKind() == ')')
                            wait--;
                    }
                    break;
                }
                /* Error ugyldig input til '^' */
                throw std::exception();
        }
        
        /* Error tom parentes */
        if (secondParenthesesIndex-firstParenthesesIndex == 1)
            throw std::exception();
        
        /* Error tom parentes operator inni */
        if (secondParenthesesIndex-firstParenthesesIndex == 2 && isOperator(token_stream[firstParenthesesIndex + 1].getKind()))
            throw std::exception();
        
        
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

/* Leter etter gange, dele og modulo */
void calculate::checkForMultiplicationDivisionModulo()
{
    if (isReady) {
        unsigned long length = token_stream.size() - 1;
        
        /* Error '*', '/' eller '%' uten tall foran */
        if (token_stream[0].getKind() == '*' || token_stream[0].getKind() == '/' || token_stream[0].getKind() == '%')
            throw std::exception();
        
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

/* Leter etter pluss og minus */
void calculate::checkForMinusPlus()
{
    if (isReady) {
        unsigned long length = token_stream.size() - 1;
        
        /* Error '+' eller '-' uten tall bak */
        if (token_stream[length].getKind() == '+' || token_stream[length].getKind() == '-')
            throw std::exception();
        
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

/* Funnet gange, dele eller modulo, utfører regneoperasjon */
void calculate::MultiplicationDivisionModulo(char op)
{
    if (isReady) {
        unsigned long length = token_stream.size() - 1;
        while(1)
        {
            /* Error '*', '/' eller '%' etterfulgt av en operator av samme prioritet */
            if (extraIterator > 0 && (token_stream[iteration + extraIterator].getKind() == '*' || token_stream[iteration + extraIterator].getKind() == '/' || token_stream[iteration + extraIterator].getKind() == '%'))
                throw std::exception();
            
            if (token_stream[iteration + extraIterator].getKind() == number)
            {
                numberIndex = iteration + extraIterator;
                extraIterator = 0;
                break;
            }
            
            /* Error '*', '/' eller '%' uten tall bak */
            if (isParentheses)
                if (iteration + extraIterator == secondParenthesesIndex)
                    throw std::exception();
            
            /* Error '*', '/' eller '%' uten tall bak */
            if (!isParentheses)
                if (iteration + extraIterator == length)
                    throw std::exception();
            
            if (token_stream[iteration + extraIterator].getKind() == '-') negative++;
            
            extraIterator++;
        }
        
        /* Error '*', '/' eller '%' uten tall foran */
        if (isOperator(token_stream[iteration - 1].getKind()) && token_stream[iteration - 1].getKind() != ')')
            throw std::exception();
        
        if (negative == 0 || (negative % 2 == 0 && negative != 0))
        {
            if (op == '*') tempRes = token_stream[iteration - 1].getValue() * token_stream[numberIndex].getValue();
            
            if (op == '/')
            {
                /* Error udefinert å dele på 0 */
                if (token_stream[numberIndex].getValue() == 0)
                    throw std::exception();
                
                tempRes = token_stream[iteration - 1].getValue() / token_stream[numberIndex].getValue();
            }
            if (op == '%')
            {
                if (floor(token_stream[iteration - 1].getValue()) == token_stream[iteration - 1].getValue() && floor(token_stream[numberIndex].getValue()) == token_stream[numberIndex].getValue())
                {
                    if (token_stream[numberIndex].getValue() == 0) tempRes = (int) token_stream[iteration - 1].getValue();
                    
                    else tempRes = (int) token_stream[iteration - 1].getValue() % (int) token_stream[numberIndex].getValue();
                }
                /* Error % fungerer kun på heltall */
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
                /* Error udefinert å dele på 0 */
                if (token_stream[numberIndex].getValue() == 0)
                    throw std::exception();
                
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
                
                /* Error '%' fungerer kun på heltall */
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

/* Funnet pluss eller minus, utfører regneoperasjon */
void calculate::MinusPlus(char op)
{
    if (isReady) {
        unsigned long length = token_stream.size() - 1;
        if (op == '-') negative++;
        
        while (1)
        {
            extraIterator++;
            
            /* Error '+' eller '-' uten etterfølgende tall */
            if (token_stream[iteration + extraIterator].getKind() == ')')
                throw std::exception();
            
            if (token_stream[iteration + extraIterator].getKind() == '-') negative++;
            
            if (token_stream[iteration + extraIterator].getKind() == number)
            {
                numberIndex = iteration + extraIterator;
                extraIterator = 0;
                break;
            }
            
            /* Error */
            if (iteration + extraIterator == length)
                throw std::exception();
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

/* Funnet operator */
void calculate::foundOperatorFirst(char op)
{
    if (isReady) {
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
        
        /* Error '+' eller '-' uten etterfølgende tall */
        if (token_stream[numberIndex].getKind() != number)
            throw std::exception();
        
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

/* Setter inn tokens i vektoren */
void calculate::insert(int index, Token token)
{
    token_stream.insert(token_stream.begin() + index, token);
}

/* Sletter tokens fra vektoren */
void calculate::remove(int removeIndex, int removeTimes)
{
    for (int i = 0; i < removeTimes; i++) token_stream.erase(token_stream.begin() + removeIndex);
}

/* Rydder opp i variablene */
void calculate::cleanUp(int whichFunction)
{
    switch (whichFunction)
    {
        case 0:
            decimalPrecision = 0;
            token = "";
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

/* Sjekker om det er en operator */
bool calculate::isOperator(char op)
{
    return (op == '(' || op == ')' || op == '*' || op == '/' || op == '+' || op == '-' || op == '%' || op == '^');
}

/* Function to remove spaces, funnet på nettet geeksforgeeks.org */
std::string calculate::removeSpaces(std::string str)
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

std::string calculate::setDecimalPoint(long double value)
{
    std::ostringstream streamObj1;
    std::string ans;
    /* Kan endre til std::fixed, std::scientific */
    streamObj1 << std::defaultfloat;
    
    /* Add double to stream */
    streamObj1 << value;
    ans = streamObj1.str();
    return ans;
}

