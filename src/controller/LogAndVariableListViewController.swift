//
//  LogAndVariableListViewController.swift
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

var lavvc = LogAndVariableListViewController()
import Cocoa

class LogAndVariableListViewController: NSViewController, NSTableViewDataSource, NSTableViewDelegate
{
    @IBOutlet var VariableAC: NSArrayController!
    @IBOutlet var LogAC: NSArrayController!
    @IBOutlet var logTable: NSTableColumn!
    @IBOutlet var varTable: NSTableColumn!
    @IBOutlet var logTab: NSTabViewItem!
    @IBOutlet var varTab: NSTabViewItem!
    var width = CGFloat(0)
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        lavvc = self
        width = lavvc.logTable.width
    }
    
    override var representedObject: Any?
    {
        didSet {}
    }
    
    func addLog(log: String)
    {
        if (log.count * 10) > Int(lavvc.logTable.width)
        {
            lavvc.logTable.width = CGFloat(log.count) * 10
        }
        let item = Log(log: log)
        lavvc.LogAC.addObject(item)
    }
    
    func addVariable(variable: String)
    {
        if (variable.count * 10) > Int(lavvc.varTable.width)
        {
            lavvc.varTable.width = CGFloat(variable.count) * 10
        }
        let item = Variable(variable: variable)
        for element in lavvc.VariableAC.arrangedObjects as! [Variable]
        {
            if element.nameOfVar == variable.first!
            {
                lavvc.VariableAC.removeObject(element)
            }
        }
        lavvc.VariableAC.addObject(item)
    }
    
    @IBAction func clear(_ sender: Any)
    {
        
        if logTab.tabState == .selectedTab
        {
            clearLog()
        }
        if varTab.tabState == .selectedTab
        {
            clearVar()
        }
    }
    
    func clearLog()
    {
        lavvc.logTable.width = width
        for _ in lavvc.LogAC.arrangedObjects as! [AnyObject]
        {
            lavvc.LogAC.remove(self)
        }
    }
    
    func clearVar()
    {
        TS().clear()
        lavvc.varTable.width = width
        for _ in lavvc.VariableAC.arrangedObjects as! [AnyObject]
        {
            lavvc.VariableAC.remove(self)
        }
    }
    
    func clearLogAndVar()
    {
        lavvc.logTable.width = width
        lavvc.varTable.width = width
        for _ in lavvc.LogAC.arrangedObjects as! [AnyObject]
        {
            lavvc.LogAC.remove(self)
        }
        for _ in lavvc.VariableAC.arrangedObjects as! [AnyObject]
        {
            lavvc.VariableAC.remove(self)
        }
    }
}
