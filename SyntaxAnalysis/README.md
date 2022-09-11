## Syntax Analysis

Literals
Binary expressions. The infix arithmetic (+, -, *, /) and logic
operators (==, !=, <, <=, >, >=) 
Parentheses. A pair of ( and ) wrapped around an expression



E -> T+E | T-E | T
T -> F*T | F/T | F
F -> ID | Integer | E

example y = 3x(x+2)-5
y and 5 are  terms

expression is made of sum of terms
term is a product of factors
factor is a number or arethesized sub expression


TreeNode {
    left 
    right
    name
}

print