//
//  C++_Wrapper.m
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#import "C++_Wrapper.h"
calculate c;
Variable va;

@implementation TS

-(NSString*) tokens:(NSString*)input
{
    
    const char* ptr = [input cStringUsingEncoding:NSUTF8StringEncoding];
    NSString* myNSString;
    try
    {
        myNSString = [NSString stringWithCString:c.parsing(ptr, va) encoding:NSASCIIStringEncoding];
    }
    catch (std::exception& e)
    {
        myNSString = @"Syntax Error";
        c.cleanUp(0);
    }
    return myNSString;
}

-(void) clear
{
    va.clear();
}

-(bool) isVarCreated
{
    return va.getCreated();
}

@end
