//
//  CalculatorViewController.swift
//  MiniGebraSwift
//
//  Created by Mathias Lunner Dozsa on 04/09/2019.
//  Copyright © 2019 Mathias Lunner Dozsa. All rights reserved.
//

import Cocoa

class CalculatorViewController: NSViewController
{
    @IBOutlet weak var label: NSTextField!
    @IBOutlet weak var textField: NSTextField!
    var ans = ""
    var isDark = false
    
    /*override func viewDidLoad()
    {
        super.viewDidLoad()
    }*/
    
    @IBAction func darkMode(_ sender: Any)
    {
        let window = NSApplication.shared.windows
        if isDark {
            isDark = false
            for window in window
            {
                window.appearance = NSAppearance(named: .aqua)
            }
        }
        else
        {
            isDark = true
            for window in window
            {
                window.appearance = NSAppearance(named: .darkAqua)
            }
        }
    }
    
    @IBAction func B1(_ sender: Any)
    {
        textField.currentEditor()?.insertText("1")
    }
    
    @IBAction func B2(_ sender: Any)
    {
        textField.currentEditor()?.insertText("2")
    }
    
    @IBAction func B3(_ sender: Any)
    {
        textField.currentEditor()?.insertText("3")
    }
    
    @IBAction func B4(_ sender: Any)
    {
        textField.currentEditor()?.insertText("4")
    }
    
    @IBAction func B5(_ sender: Any)
    {
        textField.currentEditor()?.insertText("5")
    }
    
    @IBAction func B6(_ sender: Any)
    {
       textField.currentEditor()?.insertText("6")
    }
    
    @IBAction func B7(_ sender: Any)
    {
        textField.currentEditor()?.insertText("7")
    }
    
    @IBAction func B8(_ sender: Any)
    {
       textField.currentEditor()?.insertText("8")
    }
    
    @IBAction func B9(_ sender: Any)
    {
        textField.currentEditor()?.insertText("9")
    }
    
    @IBAction func B0(_ sender: Any)
    {
        textField.currentEditor()?.insertText("0")
    }
    
    @IBAction func BPoint(_ sender: Any)
    {
        textField.currentEditor()?.insertText(".")
    }
    
    
    @IBAction func BEqual(_ sender: Any)
    {
        Clear.title = "C"
        label.stringValue = TS().tokens(textField.stringValue)
        let log = textField.stringValue + " = " + label.stringValue
        if (!TS().isVarCreated() && label.stringValue != "Syntax Error")
        {
            LogAndVariableListViewController().addLog(log: log)
            ans = label.stringValue
        }
        else if (TS().isVarCreated() && (label.stringValue != "Syntax Error"))
        {
            LogAndVariableListViewController().addVariable(variable: label.stringValue)
        }
    }
    
    @IBAction func BDivision(_ sender: Any)
    {
        textField.currentEditor()?.insertText("/")
    }
    
    @IBAction func BMultiplication(_ sender: Any)
    {
        textField.currentEditor()?.insertText("*")
    }
    
    @IBAction func BMinus(_ sender: Any)
    {
        textField.currentEditor()?.insertText("-")
    }
    
    @IBAction func BPlus(_ sender: Any)
    {
        textField.currentEditor()?.insertText("+")
    }
    
    @IBOutlet weak var Clear: NSButton!
    @IBAction func BClear(_ sender: Any)
    {
        if Clear.title == "AC"
        {
            label.stringValue = ""
            textField.stringValue = ""
            TS().clear();
            ans = ""
            LogAndVariableListViewController().clearLogAndVar()
            Clear.title = "C"
        }
        else
        {
        label.stringValue = ""
        textField.stringValue = ""
        Clear.title = "AC"
        }
    }
    
    @IBAction func BParentheses(_ sender: Any)
    {
        textField.currentEditor()?.insertText("()")
        textField.currentEditor()?.moveBackward(self)
    }
    
    @IBAction func BPower(_ sender: Any)
    {
        textField.currentEditor()?.insertText("^")
    }
    
    @IBAction func Bans(_ sender: Any)
    {
        if let i = ans.firstIndex(of: "e") {
            ans.remove(at: i)
            ans.insert(contentsOf: "(", at: ans.startIndex)
            ans.insert(contentsOf: "*10^", at: i)
            ans.insert(contentsOf: ")", at: ans.endIndex)
        }
        textField.currentEditor()?.insertText(ans)
    }
}
