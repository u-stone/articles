[toc]

Windbg的 `js` 脚本本身只能依赖 `js` 的可编程能力来整合一些已有的调试能力；有别于 Windbg 扩展开发之`扩展`。

# js 基础
- [Writing JavaScript Code](https://learn.microsoft.com/en-us/scripting/javascript/writing-javascript-code)
- [JScript Language Tour](https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/t895bwkh(v=vs.100))
- [Mozilla JavaScript Reference](https://developer.mozilla.org/docs/Web/JavaScript)
- [WinJS: The Windows library for JavaScript](https://github.com/winjs/winjs)
- [ECMAScript 6 — New Features: Overview & Comparison](http://es6-features.org/)

# Ref
- [Debugger data model, Javascript & x64 exception handling](https://doar-e.github.io/blog/2017/12/01/debugger-data-model/)
- [Break On Process Creation in WinDbg](https://shakreiner.com/posts/break-on-process-windbg/)

# Tips

1. 打开Windbg，然后输入命令：`dx -r2 Debugger` ，可以看到清晰的宿主对象结构层次，可以在js脚本中通过 `host.xx.yy` 的形式按级别访问. 

    ```
    0:000> dx -r2 Debugger
    Debugger                
        Sessions        
            [0x0]            : Full memory user mini dump: E:\jira\MMCTS-54450\test.dmp
        Settings        
            Debug           
            Display         
            EngineInitialization
            Extensions      
            Input           
            Plugins         
            Sources         
            Symbols         
            AutoSaveSettings : false
        State           
            DebuggerInformation
            DebuggerVariables
            FunctionAliases 
            PseudoRegisters 
            Scripts         
            UserVariables   
            ExtensionGallery
        Utility         
            Collections     
            Control         
            Objects         
            DbgDocumentation
            Events          
            Extensions      
            FileSystem      
            Code            
            Analysis  
    ```
    
    下面是 `-r3` 递归3层的一个输出.
    
    ```
    0:000> dx -r3 Debugger
    Debugger                
        Sessions        
            [0x0]            : Full memory user mini dump: E:\jira\MMCTS-54450\test.dmp
                Processes       
                Id               : 0
                Diagnostics     
                Devices         
                Attributes      
        Settings        
            Debug           
                Advanced        
            Display         
                Disassembly     
                PreferDMLOutput  : true
                StackTraceDepth  : 0x100
                UnicodeStrings   : false
                DefaultRecursionDepth : 0x1
                DefaultDisplayRadix : 0x10
            ...省略多行...
    ```
2. 可以参考微软官方的 Windbg js脚本示例代码来帮助入手脚本编程: [microsoft/WinDbg-Samples](https://github.com/Microsoft/WinDbg-Samples)。

# 宿主 host 中的信息

## 宿主对象级别

- host.diagnostics
    - 只有一个方法 `debugLog(object...)`, 比如： `host.diagnostics.debugLog("***> Displaying command output \n");`
- host.memory
    - readMemoryValues
    - readString
    - readWideString

## 根级别

- createPointerObject `createPointerObject(address, moduleName, typeName, [contextInheritor])`
- createTypedObject `createTypedObject(location, moduleName, typeName, [contextInheritor])`
- currentProcess 属性
- currentSession 属性
- currentThread 属性
- evaluateExpression `evaluateExpression(expression, [contextInheritor])`
- evaluateExpressionInContext `evaluateExpressionInContext(context, expression)`
- getModuleSymbol `getModuleSymbol(moduleName, symbolName, [contextInheritor])`
- getNamedModel `getNamedModel(modelName)`
- indexedValue `new indexedValue(value, indicies)`
- ...

## js 中的数据模型

|Concept|	Native Interface|	JavaScript Equivalent|
|--|--|--|
|String Conversion|	IStringDisplayableConcept|	standard: toString(...){...}|
|Iterability	|IIterableConcept|	standard: [Symbol.iterator](){...}|
|Indexability|	IIndexableConcept|	protocol: getDimensionality(...) / getValueAt(...) / setValueAt(...)|
|Runtime Type Conversion|	IPreferredRuntimeTypeConcept|	protocol: getPreferredRuntimeTypedObject(...)|

# js调用windbg命令

```
var ctl = host.namespace.Debugger.Utility.Control;   
var outputLines = ctl.ExecuteCommand("u");
```

# 输出信息

```
host.diagnostics.debugLog("***> Exiting RunCommands Function \n");
```

# Windbg 调用

```
.scriptload D:/scripts/Hello.js
dx @$hw = Debugger.State.Scritps.Hello.Contents
dx @$hw.AnyFunctinInFile()
.scriptsunload D:/scripts/Hello.js
```

