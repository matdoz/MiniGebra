//
//  C++_Wrapper.h
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "Calculate.hpp"
#import "Token.hpp"
#import "Variable.hpp"

@interface TS : NSObject
{}
-(void) clear;
-(NSString*) tokens:(NSString*) input;
-(bool) isVarCreated;

@end
