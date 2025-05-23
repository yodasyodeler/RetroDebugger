#Readme

Simple Interpreter used for Conditional breakpoints.
All Conditions are Boolean expressions, meaning they all result in a 'true' or 'false' value.

This interpreter attempts to follow 'C' like truth deduction.
Meaning a '0' will mean 'false', and any numeric value greater will mean 'true'.

The comma operator works similar to C in that the last element is the one that is actually used.
Intended use of comma operator is not defined, may be considered for future deprecation.

TODO: Ternary operator precedence with comma operator. Does current implementations work as expected with each other.

TODO: this is out of date, also provide examples
| Name              | Evaluation                                                                        |
| ----------------- | --------------------------------------------------------------------------------- |
| BooleanExpression | Ternary;                                                                          |
| Ternary           | Comma "?" Comma ":" Comma \| Comma;                                               |
| Comma             | Logic_OR ("," Logic_OR)*;                                                         |
| Logic_OR          | Logic_AND ( "\|\|" Logic_AND)*;                                                   |
| Logic_AND         | Bitwise_OR ( "&&" Bitwise_OR)*;                                                   |
| Bitwise_OR        | Bitwise_XOR ("\|" Bitwise_XOR)*;                                                  |
| Bitwise_XOR       | Bitwise_AND ("^" Bitwise_AND)*;                                                   |
| Bitwise_AND       | Equality ("&" Equality)*;                                                         |
| Equality          | Comparison (("!=" \| "==") Comparison)*;                                          |
| Comparison        | Term ((">" \| ">=" \| "<" \| "<=") Term)*;                                        |
| Term              | Factor (("-" \| "+") Factor)*;                                                    |
| Unary             | ("!" \| "-" \| "*") Unary \| Primary;                                             |
| Primary           | Number \| Number":"Number \| Register \| "true" \| "false" \| "(" expression ")"; |

The prefixed `*` operator can be used to dereference an address.  
`*0x1000` will attempt to read the value at address `0x1000`.  
The prefixed `*` operator can be chained similar to C language to dereference like a pointer.  

Supported `Register`s are determined by the available RegSet values populated with the callback configured with the `SetGetRegSetCallback` API.