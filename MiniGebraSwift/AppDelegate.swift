//
//  AppDelegate.swift
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 21/08/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate
{
    func applicationDidFinishLaunching(_ aNotification: Notification)
    {}

    func applicationWillTerminate(_ aNotification: Notification)
    {}
    
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool
    {
        return true
    }
}

