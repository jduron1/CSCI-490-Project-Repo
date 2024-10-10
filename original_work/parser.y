%{

/* C declarations */

#include <stdio.h>
#include <string.h>

extern FILE *yyin;
extern int yylex();
void yyerror(const char *s);

%}

/* Definitions */

%union {
    char* string;
    double real;
    int integer;
    char character;
}

%token <string> SEA ENTERO FLOTANTE BOOLEANO VACIO
        CADENA CARACTER FUNCION CLASE CIERTO FALSO SI SINO
        Y O POR MIENTRAS ESTA EN REGRESA
        PLUS_EQUAL MINUS_EQUAL TIMES_EQUAL
        DIVIDE_EQUAL MOD_EQUAL
        EQUAL NOT_EQUAL LE GE
        ARROW ELLIPSIS

%token <string> IDENTIFIER
%token <character> CHARACTER
%token <string> STRING
%token <integer> INTEGER
%token <real> REAL

%start program
%type <string> statements
%type <string> statement
%type <string> class
%type <string> declaration
%type <string> assignment
%type <string> if
%type <string> while
%type <string> for
%type <string> return
%type <string> function
%type <string> call
%type <string> arguments
%type <string> argument
%type <integer> expressions
%type <integer> expression
%type type
%type literal

%nonassoc IDENTIFIER
%left '+' '-'
%left '*' '/' '%' '^'
%left PLUS_EQUAL MINUS_EQUAL TIMES_EQUAL DIVIDE_EQUAL MOD_EQUAL
%nonassoc '<' LE '>' GE '=' EQUAL NOT_EQUAL
%left Y O '!'
%left ESTA EN

%%

/* Productions */

program: statements { printf("Program\n"); };

statements: statement statements { printf("Multiple statements\n"); }
          | %empty { printf("No statements\n"); } 

statement: declaration ';' { printf("Declaration\n"); }
         | assignment ';' { printf("Assignment\n"); }
         | function { printf("Function definition\n"); }
         | class { printf("Class\n"); }
         | expression ';' { printf("Expression\n"); }
         | if { printf("If statement\n"); }
         | while { printf("While statement\n"); }
         | for { printf("For statement\n"); }
         | return ';' { printf("Return statement\n"); }
         ;

class: CLASE IDENTIFIER '{' statements '}' { printf("Class: %s\n", $1); }

declaration: SEA IDENTIFIER { printf("Declaration: %s %s\n", $1, $2); }
           | SEA assignment { printf("Declaration with assignment: %s\n", $1); }
           ;

assignment: IDENTIFIER '=' expression { printf("Assignment: %s\n", $1); }

function: FUNCION IDENTIFIER '(' arguments ')' ARROW type '{' statements '}' { printf("Function: %s\n", $1); };

call: IDENTIFIER '(' expressions ')' { printf("Function call: %s\n", $1); }
    ;

arguments: arguments ',' argument { printf("Multiple arguments\n"); }
         | argument { printf("One argument\n"); }
         | %empty { printf("No arguments\n"); }
         ;

argument: IDENTIFIER { printf("Argument: %s\n", $1); };

if: SI expression '{' statements '}' { printf("If statement: %s\n", $1); }
  | SI expression '{' statements '}' SINO '{' statements '}' { printf("If-else statement: %s\n", $1); }
  ;

while: MIENTRAS expression '{' statements '}' { printf("While statement: %s\n", $1); }

for: POR IDENTIFIER EN INTEGER ELLIPSIS INTEGER '{' statements '}' { printf("For statement with range: %s %s %s %d %s %d\n", $1, $2, $3, $4, $5, $6); }
   | POR IDENTIFIER EN IDENTIFIER '{' statements '}' { printf("For statement w/ 2 identifiers: %s %s %s %s\n", $1, $2, $3, $4); }

return: REGRESA expression { printf("Return statement\n"); }

expressions: expressions ',' expression { printf("Multiple expressions\n"); }
           | expression { printf("One expression\n"); }
           | %empty { printf("No expressions\n"); }
           ;

expression: literal { printf("Literal\n"); }
          | expression '+' expression { printf("Addition: %d + %d\n", $1, $3); $$ = $1 + $3; }
          /* | expression '+' expression {
                if ($1.type == REAL || $3.type == REAL) {
                    printf("Addition: %f + %f\n", $1, $3);
                    $$ = (double)$1 + (double)$3;
                } else {
                    printf("Addition: %d + %d\n", $1, $3);
                    $$ = $1 + $3;
                }
            } */
          | expression '-' expression { printf("Subtraction: %d + %d\n", $1, $3); $$ = $1 - $3; }
          | expression '*' expression { printf("Multiplication: %d * %d\n", $1, $3); $$ = $1 * $3; }
          | expression '/' expression { printf("Division: %d / %d\n", $1, $3); $$ = $1 / $3; }
          | expression '^' expression { printf("Exponentiation: %d ^ %d\n", $1, $3); for (int i = 1; i < $3; i++) { $$ *= $1; } }
          | expression '%' expression { printf("Modulus: %d mod %d\n", $1, $3); $$ = $1 % $3; }
          | expression PLUS_EQUAL expression { printf("Plus-equal\n"); }
          | expression MINUS_EQUAL expression { printf("Minus-equal\n"); }
          | expression TIMES_EQUAL expression { printf("Times-equal\n"); }
          | expression DIVIDE_EQUAL expression { printf("Divide-equal\n"); }
          | expression MOD_EQUAL expression { printf("Mod-equal\n"); }
          | expression '=' expression { printf("Assignment\n"); }
          | expression EQUAL expression { printf("Equality\n"); }
          | expression NOT_EQUAL expression { printf("Inequality\n"); }
          | expression LE expression { printf("Less than or equal\n"); }
          | expression GE expression { printf("Greater than or equal\n"); }
          | expression '<' expression { printf("Less than\n"); }
          | expression '>' expression { printf("Greater than\n"); }
          | expression Y expression { printf("And\n"); }
          | expression O expression { printf("Or\n"); }
          | '!' expression { printf("Not\n"); }
          | '-' expression %prec '-' { printf("Negation: %d\n", -$2); $$ = -$2; }
          | expression ESTA EN expression { printf("In\n"); }
          | '(' expression ')' { printf("Parentheses\n"); $$ = $2; }
          | call { printf("Function call\n"); }
          | IDENTIFIER { printf("Identifier: %s\n", $1); }
          ;

type: CARACTER { printf("Character\n"); }
    | ENTERO { printf("Integer\n"); }
    | FLOTANTE { printf("Float\n"); }
    | CADENA { printf("String\n"); }
    | BOOLEANO { printf("Boolean\n"); }
    | VACIO { printf("Void\n"); }
    ;

literal: CHARACTER { printf("Character: %c\n", $1); }
      | STRING { printf("String: %s\n", $1); }
      | INTEGER { printf("Integer: %d\n", $1); }
      | REAL { printf("Float: %f\n", $1); }
      | CIERTO { printf("True\n"); }
      | FALSO { printf("False\n"); }
      ;

%%

/* C code */

void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Error: Unable to open file\n");
        return 1;
    }

    yyparse();
    fclose(yyin);

    return 0;
}