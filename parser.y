%{
    #include "semantics.c"
    #include "symbol_table.c"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

    extern char* yytext;
    extern int yylineno;
    extern FILE* yyin;
    extern FILE* yyout;

    extern int yylex();
    void yyerror();    
%}

%union {
    char character;
    int integer;
    double real;
    char* string;
    Node* item;
}

%token <integer> CARACTER BOOLEANO ENTERO REAL CADENA VACIO
%token <integer> FUNCION CIERTO FALSO SI SINO
%token <integer> POR MIENTRAS ESPERA CONTINUA REGRESA
%token <integer> ADD SUB MUL DIV MOD EXP ADD_ASSIGN SUB_ASSIGN
%token <integer> MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN
%token <integer> OR AND NOT EQ REL ARROW LPAREN RPAREN
%token <integer> LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA
%token <integer> ASSIGN AMPERSAND ELLIPSIS EN
%token <item> IDENTIFIER
%token <integer> INTEGER
%token <real> DECIMAL
%token <character> CHARACTER
%token <string> STRING

%left LPAREN RPAREN LBRACKET RBRACKET
%right NOT AMPERSAND
%left EXP EXP_ASSIGN
%left MUL DIV MOD MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%left ADD SUB ADD_ASSIGN SUB_ASSIGN
%left REL
%left EQ
%left OR
%left AND
%right ASSIGN
%left COMMA

%start program

%%

program: declarations statements REGRESA SEMICOLON optional_functions;

declarations: declarations declaration
            | declaration
            ;

declaration: { declared = 1; } type names { declared = 0; } SEMICOLON;

type: ENTERO
    | REAL
    | CARACTER
    | BOOLEANO
    | CADENA
    | VACIO
    ;

names: names COMMA variable
     | names COMMA init
     | variable
     | init
     ;

variable: IDENTIFIER
        | pointer IDENTIFIER
        | IDENTIFIER array
        ;

pointer: pointer MUL
       | MUL
       ;

array: array LBRACKET expression RBRACKET
     | LBRACKET expression RBRACKET
     ;

init: var_init
    | array_init
    ;

var_init: IDENTIFIER ASSIGN expression;

array_init: IDENTIFIER array ASSIGN LBRACE values RBRACE;

values: values COMMA expression
      | expression
      ;

statements: statements statement
          | statement
          ;

statement: if
         | for
         | while
         | assignment SEMICOLON
         | CONTINUA SEMICOLON
         | ESPERA SEMICOLON
         | call SEMICOLON
         | REGRESA expression SEMICOLON
         ;

if: SI expression tail else_if else
  | SI expression tail else
  ;

else_if: else_if SINO SI expression tail
       | SINO SI expression tail
       ;

else: SINO tail
    | %empty
    ;

for: POR IDENTIFIER EN INTEGER ELLIPSIS INTEGER tail;

while: MIENTRAS expression tail;

tail: LBRACE statements RBRACE;

expression: expression ADD expression
          | expression SUB expression
          | expression MUL expression
          | expression DIV expression
          | expression MOD expression
          | expression EXP expression
          | expression OR expression
          | expression AND expression
          | NOT expression
          | expression EQ expression
          | expression REL expression
          | LPAREN expression RPAREN
          | var_ref
          | sign constant
          | call
          ;

sign: ADD
    | SUB
    | %empty
    ;

constant: INTEGER
        | DECIMAL
        | CHARACTER
        | STRING
        | CIERTO
        | FALSO
        ;

assignment: var_ref ASSIGN expression;
          | var_ref ADD_ASSIGN expression
          | var_ref SUB_ASSIGN expression
          | var_ref MUL_ASSIGN expression
          | var_ref DIV_ASSIGN expression
          | var_ref MOD_ASSIGN expression
          | var_ref EXP_ASSIGN expression
          ;

var_ref: variable
       | AMPERSAND variable
       ;

call: IDENTIFIER LPAREN call_arguments RPAREN;

call_arguments: call_argument
              | %empty
              ;

call_argument: call_argument COMMA expression
             | expression
             ;

optional_functions: functions
                  | %empty
                  ;

functions: functions function
         | function
         ;

function: { incrScope(); } f_head f_tail { hideScope(); };

f_head: FUNCION { declared = 1; } IDENTIFIER LPAREN optional_arguments RPAREN ARROW return_type { declared = 0; };

return_type: type
           | type pointer
           ;

optional_arguments: arguments
                  | %empty
                  ;

arguments: arguments COMMA argument
         | argument
         ;

argument: { declared = 1; } type variable { declared = 0; };

f_tail: LBRACE optional_declarations optional_statements RBRACE;

optional_declarations: declarations
                     | %empty
                     ;

optional_statements: statements
                     | %empty
                     ;

%%

void yyerror() {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, yytext); 
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    initTable();

    queue = NULL;

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Error: Unable to open file\n");
        return 1;
    }

    int flag = yyparse();

    fclose(yyin);

    printf("Syntax check: %s\n", flag == 0 ? "successful." : "failed.");

    if (queue != NULL) {
        printf("Unchecked item in the revisit queue.\n");
    }

    yyout = fopen("table.out", "w");
    if (yyout == NULL) {
        printf("Error: Unable to open file\n");
        return 1;
    }

    printTable(yyout);
    fclose(yyout);

    yyout = fopen("queue.out", "w");
    if (yyout == NULL) {
        printf("Error: Unable to open file\n");
        return 1;
    }

    printQueue(yyout);
    fclose(yyout);

    freeTable();

    return flag;
}