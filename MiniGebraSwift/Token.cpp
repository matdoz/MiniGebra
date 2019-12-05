//
//  Token.cpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#include "Token.hpp"

Token::Token(long double val, char kind)
:value(val), kind(kind)
{}

Token::Token(char kind)
:kind(kind), value(0)
{}

long double Token::getValue()
{
    return value;
}

char Token::getKind()
{
    return kind;
}

Token::Token(){}

void Token::setValue(long double val)
{
    value = val;
}

void Token::setKind(char kin)
{
    kind = kin;
}
