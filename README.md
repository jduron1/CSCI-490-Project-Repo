[![Compilation Check](https://github.com/jduron1/CSCI-490-Project-Repo/actions/workflows/compiler.yml/badge.svg)](https://github.com/jduron1/CSCI-490-Project-Repo/actions/workflows/compiler.yml)

# CSCI-490-Project-Repo
Implementation of a small transpiler for my language called Axl.

## Installing Flex and Bison
* Installing Flex: `sudo apt-get install flex`
* Installing Bison: `sudo apt-get install bison`

## Compiling the Flex, Bison, and Source Files
```bash
flex scanner.l
bison -d parser.y
gcc -o axlc -lfl lex.yy.c parser.tab.c symbol_table.c semantics.c ast.c code_gen.c
```

## Running the Executable to Convert Source Code to C++
```bash
./axlc [file name]
```

## Compiling the C++ File and Running the Program
```bash
g++ -o [name] output.cpp
./[name]
```
