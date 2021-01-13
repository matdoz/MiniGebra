//
//  C++_Wrapper.m
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#import "C++_Wrapper.h"
@implementation TS
Calculate* c = new Calculate();
Variable* v = new Variable();
-(NSString*) tokens:(NSString*)input
{
    
    const char* ptr = [input cStringUsingEncoding:NSUTF8StringEncoding];
    NSString* myNSString;
    try
    {
        myNSString = [NSString stringWithCString:c->parsing(ptr, *v) encoding:NSASCIIStringEncoding];
    }
    catch (std::exception& e)
    {
        myNSString = @"Syntax Error";
        c->cleanUp(0);
    }
    return myNSString;
}

-(void) clear
{
    v->clear();
}

-(bool) isVariable:(char)c
{
    return v->isVariable(c);
}
@end
