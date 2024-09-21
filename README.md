# CSCI-490-Project-Repo
Implementation of a small compiler for my language.

# Compiling the Flex and Bison files:

flex scanner.l
bison -d parser.y
gcc -lfl lex.yy.c parser.tab.c

# Running the executable:

./a.out [file name]
