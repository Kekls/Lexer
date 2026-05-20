# Lexer0.0.1

## About

Lexer0.0.1 is my first attempt at building a lexical analyzer from scratch.

This project is part of my learning journey in compiler design and low-level programming concepts.

It processes input text and converts it into a sequence of tokens (lexemes).

---

## Features

- Multi-state lexer (NORMAL / EXPRESSION / STRING)
- Supports arithmetic expressions
- String parsing with quotes
- Token classification system

## Limitations

- No full parser yet
- No float support (or optional)
- No error handling for invalid syntax
- Experimental architecture (0.0.1)

---

## How it works

The lexer reads input character by character and classifies them into tokens such as:
- NUMBER
- STRING
- IDENTIFIER
- OPERATORS (+, -, *, /, %, etc.)
- PARENTHESES

---

## Example

### Input:
```
"ala ma kota o imieniu ' jarek ' ma on " 5 + 3 * ( 2 + 1 - 2 ) "lat"
```
### Output:
```
STRING("ala ma kota o imieniu ' jarek ' ma on ")
NUMBER(5)
PLUS
NUMBER(3)
MULT
LPAREN
NUMBER(2)
PLUS
NUMBER(1)
MINUS
NUMBER(2)
RPAREN
STRING("lat")
```

---

## Status

This is an early experimental version (0.0.1).
Code structure is not yet optimized and will be improved in future versions.

---

## Author

Kekls  
Contact: bartorgan@gmail.com
