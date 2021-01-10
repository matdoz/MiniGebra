//
//  File.swift
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 09/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

import Foundation

@objc(Variable)
class Variable : NSObject
{
    @objc var variable: String
    var nameOfVar: Character
    init(variable: String)
    {
        self.nameOfVar = variable.first!
        self.variable = variable
    }
}
