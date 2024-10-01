[![Compilation Check](https://github.com/jduron1/CSCI-490-Project-Repo/actions/workflows/compiler.yml/badge.svg)](https://github.com/jduron1/CSCI-490-Project-Repo/actions/workflows/compiler.yml)

# CSCI-490-Project-Repo
Implementation of a small compiler for my language.

# Compiling the Flex and Bison Files

1. flex scanner.l
2. bison -d parser.y
3. gcc -lfl lex.yy.c parser.tab.c

# Running the Executable

./a.out [file name]
