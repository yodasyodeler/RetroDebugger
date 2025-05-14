#Readme

Simple Interpreter used for Conditional breakpoints.


TODO: this is out of date, also provide examples
| Name              | Evaluation                                                    |
| ----------------- | ------------------------------------------------------------- |
| BooleanExpression | Equality;                                                     |
| Equality          | Comparison (("!=" \| "==") Comparison)*;                      |
| Comparison        | Term ((">" \| ">=" \| "<" \| "<=") Term)*;                    |
| Term              | Factor (("-" \| "+") Factor)*;                                |
| Unary             | ("!" \| "-") Unary \| Primary;                                |
| Primary           | Number \| Register \| "true" \| "false" \| "(" expression ")" |
