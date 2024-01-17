[toc]

# 语法

## 命令符号

- ; (命令分割符) `Command1 ; Command2 [; Command3 ...]` 从左向右执行
- {} (块分隔符) `Statements { Statements } Statements` 块内部的别名会重新计算，同名的别名，块内部修改后，不会影响块外部的值，但是块的内部嵌套块会受影响。
- ${} (别名解释器)

    ```
    Text ${Alias} Text 
    Text ${/d:Alias} Text  根据当前是否定义了别名，计算结果为一或零。如果定义了别名，则${/d: Alias }替换为1；如果未定义别名，${/d: Alias }将替换为 0。
    Text ${/f:Alias} Text 如果当前定义了别名，则计算为等效的别名。如果定义了别名，则${/f: Alias }被替换为等价的别名；如果未定义别名，${/f: Alias }将替换为空字符串。
    Text ${/n:Alias} Text 如果当前定义了别名，则计算别名。如果定义了别名，则${/n: Alias }被替换为别名；如果未定义别名，则${/n: Alias }不会被替换，但会保留其字面值${/n: Alias }。
    Text ${/v:Alias} Text 防止任何别名评估。无论Alias是否被定义，${/v: Alias }始终保留其字面值${/v: Alias }。
    ```
- $$（注释说明符）`$$ [any text]` 例如：`0:000> r eax; $$ some text; r ebx; * more text; r ecx`
- \* （注释行说明符）如果星号 ( * ) 字符位于命令的开头，则该行的其余部分将被视为注释，即使其后出现分号也是如此。`0:000> r eax; $$ some text; r ebx; * more text; r ecx`
- .block (块定义头) .block不执行任何操作；它仅用于引入语句块。`Commands ; .block { Commands } ; Commands`

## 控制流

- .break 类似C语言的 `break`
    ```
    .for (...) { ... ; .if (Condition) .break ; ...} 
    .while (...) { ... ; .if (Condition) .break ; ...} 
    .do { ... ; .if (Condition) .break ; ...} (...)
    ```
- .catch 发生错误时防止终止程序 `Commands ; .catch { Commands } ; Commands`
- .continue 类似C语言的 `continue`

    ```
    .for (...) { ... ; .if (Condition) .continue ; ... } 
    .while (...) { ... ; .if (Condition) .continue ; ... } 
    .do { ... ; .if (Condition) .continue ; ... } (...)
    ```
- .do 类似C语言的 do `.do { Commands } (Condition)`
- .else 类似C语言的 `else`

    ```
    .if (Condition) { Commands } .else { Commands } 
    .if (Condition) { Commands } .elsif (Condition) { Commands } .else { Commands }
    ```
- .elif 类似 C 语言的 `else if`
    ```
    .if (Condition) { Commands } .elsif (Condition) { Commands } 
    .if (Condition) { Commands } .elsif (Condition) { Commands } .else { Commands }
    ```
- .for 行为类似于C 中的for关键字，除了多个增量命令必须用分号而不是逗号分隔。`.for (InitialCommand ; Condition ; IncrementCommands) { Commands }`， 比如 :`0:000> .for (r eax=0; @eax < 7; r eax=@eax+1; r ebx=@ebx+1) { .... }`
- .foreach 解析一个或多个调试器命令的输出，并将此输出中的每个值用作一个或多个附加命令的输入。

    ```
    .foreach [Options] ( Variable  { InCommands } ) { OutCommands } 
    .foreach [Options] /s ( Variable  "InString" ) { OutCommands } 
    .foreach [Options] /f ( Variable  "InFile" ) { OutCommands }
    
    例如：
    0:000> .foreach /f ( place "g:\myfile.txt") { dds place }
    0:000> .foreach /pS 2 /ps 4 /f ( place "g:\myfile.txt") { dds place }
    ```
- .if 类似C语言的if 

    ```
    .if (Condition) { Commands } 
    .if (Condition) { Commands } .else { Commands } 
    .if (Condition) { Commands } .elsif (Condition) { Commands } 
    .if (Condition) { Commands } .elsif (Condition) { Commands } .else { Commands }
    ```

- .leave 用于退出.catch块。`.catch { ... ; .if (Condition) .leave ; ... }`
- .printf 类似于C 中的printf语句。`.printf [/D] [Option] "FormatString" [, Argument , ...]`
- .while `.while (Condition) { Commands }`

## 变量

### 别名

- 用户自定义：不包含空格的任意字符串
- 固定名称别名：$u0,...$u9 （等价于不包含回车符的任意字符串），不过这些名称没有什么含义，可以使用寄存器命令 (r)定义固定名称别名的等效别名：`需要在u前面加一个"."，后面跟一个 "="，即 (r $.u2=alianame)`
- 自动别名

#### 用于脚本

```
.foreach (value {dd 610000 L4})
{
   as /x ${/v:myAlias} value + 1
   .echo value myAlias
}

ad myAlias
```

第一次通过循环时，as, aS（设置别名）命令为 myAlias 赋值。分配给 myAlias 的值是 1 加上 610000（dd 命令的第一个输出）。但是，当执行.echo（Echo Comment）命令时，myAlias 尚未展开，因此我们看到的不是 610001，而是文本“myAlias”。

我们可以通过将. echo (Echo Comment)命令在一个新的块中

```
.foreach (value {dd 610000 L4}) 
{
   as /x ${/v:myAlias} value + 1
   .block{.echo value myAlias}
}

ad myAlias
```

#### .foreach中使用别名

```
r $t0 = 5
ad myAlias
.foreach /pS 2 /ps 2 (Token {?@$t0}) {as myAlias Token}
al
```
第一个命令将$t0伪寄存器的值设置为 5。第二个命令删除之前可能已分配给 myAlias 的任何值。第三个命令采用?@$t0命令的第三个标记，并尝试将该标记的值分配给 myAlias。第四个命令列出所有别名及其值。我们希望 myAlias 的值为 5，但该值是单词“Token”。

```
Alias            Value  
-------          ------- 
myAlias          Token
```

**问题是as命令位于.foreach循环主体中行的开头**。当一行以as命令开始时，该行中的别名和标记不会展开。如果我们在as命令之前放置一个分号或空格，那么任何已经有值的别名或标记都会被展开。

```
r $t0 = 5
ad myAlias
.foreach /pS 2 /ps 2 (Token {?@$t0}) {;as myAlias Token}
al
```
现在我们得到了预期的输出。
```
Alias            Value  
-------          ------- 
myAlias          5
```


#### 递归别名

您可以在任何别名的定义中使用固定名称别名。您还可以在固定名称别名的定义中使用用户命名的别名。但是，要在另一个用户命名的别名的定义中使用一个用户命名的别名，您必须在as或aS命令之前添加一个分号，否则该行不会发生别名替换。

```
0:000> r $.u2=2 
0:000> r $.u1=1+$u2 
0:000> r $.u2=6 
0:000> ? $u1 
Evaluate expression: 3 = 00000003
```

# 脚本

- 一个循环命令示例：`recx = poi(ecx); r ecx; z(ecx!=0)` ，z(n)是循环条件

# 参考

- [Using Debugger Commands](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/using-debugger-commands)
- [Debugger Command Program Examples](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-command-program-examples)
- [Using Aliases](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/using-aliases)
- [0vercl0k/windbg-scripts](https://github.com/0vercl0k/windbg-scripts)