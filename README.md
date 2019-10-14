# calculator
编译原理课程作业-- 计算器， 内含表达式的计算（中缀转后缀）

### 计算器接受四则运算表达式为输入。如果表达式语法正确，则输出计算结果，否则报错，指出错误位置及原因。 
#### 测试用例：
1.正常运行
``` C
float a; 
int b; 
a = (10.44*356+1.28) / 2 + 1024 * 1.6;
b = a * 2 - a/2;
write(b);
write(a).
```

2. Error( line 4 ): undefined identifier
``` C
float a; 
int b; 
a = (10.44*356+1.28) / 2 + 1024 * 1.6;
b = a * 2 - c/2;
write(b).
```

3. Error( line 2 ): syntax error
``` C
float a; 
int ; 
a = (10.44*356+1.28) / 2 + 1024 * 1.6;
b = a * 2 - a/2;
write(b);
write(a).
```

4. Error( line 4 ): divide by zero
``` C
float a; 
float b; 
a = 10.44;
b = a/0;
write(b);
write(a).
```

5. Error( line 4 ): type cast : turn int into float
``` C
int a; 
float b; 
a = 10.44;
b = a;
write(b);
write(a).
```

6. Error( line 3 ): uninitialized identifier
``` C
float a; 
int b; 
b = a * 2 - a/2;
write(b);
```

### 语法描述
```
<程序> ->
(<语句><行结束符号>)*(<语句><全文结束符号>)

<语句> -> 
<声明语句>|<赋值语句>|<输出语句>

<声明语句> -> 
<声明符号><空格> <变量名>

<赋值语句> -> 
<变量名><等号><表达式>

<输出语句> -> 
<输出函数>(<变量名>)

<声明符号> -> float | int

Digit -> [0-9];
Letter ->[a-zA-Z]
<变量名> -> (_|Letter)(Letter | Digit|_)*

<行结束符号> -> ;

<全文结束符号> ->.

<等号> -> =

<输出函数> -> write

<空格> -> ( )+

<表达式> ->(<运算符号>|<整数>|<浮点数>)*

<运算符号> ->[+-*/()]

<整数> -> Digit*

<浮点数> -> (Digit+).(Digit*)

```

