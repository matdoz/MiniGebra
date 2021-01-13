//
//  Variable.cpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#include "Variable.hpp"

Variable::Variable(char name, long double value)
:name(name), value(value)
{}

Variable::Variable()
{}

long double Variable::getValue(char name)
{
    for (int i = 0; i < variableTable.size(); i++)
    {
        if (name == variableTable[i].getName())
        {
            return variableTable[i].value;
        }
    }
    return 1;
}

char Variable::getName()
{
    return name;
}

void Variable::addVariable(Variable a)
{
    if (isVariable(a.getName()))
    {
        removeVariable(a.getName());
    }
    variableTable.push_back(a);
}

void Variable::removeVariable(char a)
{
    for (int i = 0; i < variableTable.size(); i++)
    {
        if (a == variableTable[i].getName())
        {
            variableTable.erase(variableTable.begin() + i);
            return;
        }
    }
    std::cerr << "Error variabel ikke funnet i listen.\n\n";
}

bool Variable::isVariable(char name)
{
    for (int i = 0; i < variableTable.size(); i++) if (name == variableTable[i].getName()) return true;
    return false;
}


void Variable::clear()
{
    variableTable.clear();
}

Variable* Variable::getVar(char name)
{
    for (int i = 0; i < variableTable.size(); i++)
    {
        if(name == variableTable[i].getName())
            return &variableTable[i];
    }
    throw std::exception();
}
