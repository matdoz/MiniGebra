//
//  Token.hpp
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#ifdef __cplusplus
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <cmath>
#include <math.h>

extern "C"
{
    const char number = '8';
    
    class Token
    {
    private:
        long double value;
        char kind;
    public:
        Token(long double val, char kind);
        Token(char kind);
        Token();
        void setValue(long double val);
        void setKind(char kind);
        long double getValue();
        char getKind();
    };
}
#endif
