[![Compilation Check](https://github.com/jduron1/CSCI-490-Project-Repo/actions/workflows/compiler.yml/badge.svg)](https://github.com/jduron1/CSCI-490-Project-Repo/actions/workflows/compiler.yml)

# CSCI-490-Project-Repo
Implementation of a small compiler for my language.

# Installing Flex and Bison
* Installing Flex: sudo apt-get install flex
* Installing Bison: sudo apt-get install bison

# Compiling the Flex and Bison Files
1. flex scanner.l
2. bison -d parser.y
3. gcc -lfl lex.yy.c parser.tab.c symbol_table.c semantics.c ast.c code_gen.c

# Running the Executable
./a.out [file name]

# Compiling the C++ File and Running the Example Programs
g++ -o [name] output.cpp\
./[name]
