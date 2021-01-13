//
//  Variable.hpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#ifdef __cplusplus
#include <stdio.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

extern "C"
{
    class Variable
    {
    private:
        long double value;
        char name;
        std::vector<Variable> variableTable {};
    public:
        Variable(char name, long double value);
        Variable();
        Variable* getVar(char name);
        void addVariable(Variable a);
        void removeVariable(char a);
        void clear();
        long double getValue(char name);
        char getName();
        bool isVariable(char name);
    };
}
#endif

