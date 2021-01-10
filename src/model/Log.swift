//
//  File.swift
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 09/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

import Foundation

@objc(Log)
class Log : NSObject
{
    @objc var log: String
    init(log: String)
    {
        self.log = log
    }
}
