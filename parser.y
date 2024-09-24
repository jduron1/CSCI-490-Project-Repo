%{
    #include "semantics.c"
	#include "symbol_table.c"
	#include "ast.h"
	#include "ast.c"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

    extern char* yytext;
    extern int yylineno;
    extern FILE* yyin;
    extern FILE* yyout;

    extern int yylex();
    void yyerror();

    void addToNames(StorageNode*);
    StorageNode** names;
    int name_count = 0;

    void addToVars(ValueType);
    ValueType* vars;
    int var_count = 0;
%}

%union {
    StorageNode* item;
    ASTNode* node;
    ValueType var;

    int data_type;
    int const_type;
    int array_size;
}

%token <var> CARACTER BOOLEANO ENTERO REAL CADENA VACIO
%token <var> FUNCION CIERTO FALSO SI SINO
%token <var> POR MIENTRAS ESPERA CONTINUA REGRESA
%token <var> ADD SUB MUL DIV MOD EXP ADD_ASSIGN SUB_ASSIGN
%token <var> MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN
%token <var> OR AND NOT EQ REL ARROW LPAREN RPAREN
%token <var> LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA
%token <var> ASSIGN AMPERSAND ELLIPSIS EN
%token <item> IDENTIFIER
%token <var> INTEGER
%token <var> DECIMAL
%token <var> CHARACTER
%token <var> STRING

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

%type <node> program
%type <node> declarations declaration
%type <data_type> type
%type <item> variable
%type <array_size> array
%type <item> init var_init array_init
%type <node> constant

%start program

%%

program: declarations statements REGRESA SEMICOLON optional_functions
       ;

declarations: declarations declaration
            | declaration
            ;

declaration: type { declared = 1; } names { declared = 0; } SEMICOLON
                {
                    $$ = newASTDeclNode($1, names, name_count);
                    
                    name_count = 0;

                    ASTDecl* temp = (ASTDecl*)$$;

                    for (int i = 0; i < temp -> num_names; i++) {
                        switch (temp -> names[i] -> storage_type) {
                            case UNDEF:
                                setType(temp -> names[i] -> storage_name, temp -> decl_type, UNDEF);
                                break;
                            case PTR_TYPE:
                                setType(temp -> names[i] -> storage_name, PTR_TYPE, temp -> decl_type);
                                break;
                            case ARR_TYPE:
                                setType(temp -> names[i] -> storage_name, ARR_TYPE, temp -> decl_type);
                                break;
                            default:
                                break;
                        }
                    }

                    traverseAST($$);
                }
           ;

type: ENTERO { $$ = INT_TYPE; }
    | REAL { $$ = REAL_TYPE; }
    | CARACTER { $$ = CHAR_TYPE; }
    | BOOLEANO { $$ = BOOL_TYPE; }
    | CADENA { $$ = STR_TYPE; }
    | VACIO { $$ = VOID_TYPE; }
    ;

names: names COMMA variable { addToNames($3); }
     | names COMMA init { addToNames($3); }
     | variable { addToNames($1); }
     | init { addToNames($1); }
     ;

variable: IDENTIFIER { $$ = $1; }
        | pointer IDENTIFIER
            {
                $2 -> storage_type = PTR_TYPE;
                $$ = $2;
            }
        | IDENTIFIER array
            {
                $1 -> storage_type = ARR_TYPE;
                $1 -> array_size = $2;
                $$ = $1;
            }
        ;

pointer: pointer MUL
       | MUL
       ;

array: array LBRACKET expression RBRACKET
        {
            if (declared) {
                fprintf(stderr, "Error at line %d: array size must be an integer.\n", yylineno);
            }
        }
     | LBRACKET expression RBRACKET
        {
            if (declared) {
                fprintf(stderr, "Error at line %d: array size must be an integer.\n", yylineno);
            }
        }
     | LBRACKET INTEGER RBRACKET
        {
            $$ = $2.integer;
        }
     ;

init: var_init { $$ = $1; }
    | array_init { $$ = $1; }
    ;

var_init: IDENTIFIER ASSIGN constant
            {
                ASTConst* temp = (ASTConst*)$$;

                $1 -> val = temp -> var;
                $1 -> storage_type = temp -> const_type;
                $$ = $1;
            }
        ;

array_init: IDENTIFIER array ASSIGN LBRACE values RBRACE
            {
                if ($1 -> array_size != var_count) {
                    fprintf(stderr, "Error at line %d: incorrect number of elements in array.\n", yylineno);
                }

                $1 -> vals = vars;
                $1 -> array_size = $2;
                $$ = $1;

                var_count = 0;
            }
          ;

values: values COMMA constant
        {
            ASTConst* temp = (ASTConst*)$3;
            addToVars(temp -> var);
        }
      | constant
        {
            ASTConst* temp = (ASTConst*)$1;
            addToVars(temp -> var);
        }
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

constant: INTEGER { $$ = newASTConstNode(INT_TYPE, $1); }
        | DECIMAL { $$ = newASTConstNode(REAL_TYPE, $1); }
        | CHARACTER { $$ = newASTConstNode(CHAR_TYPE, $1); }
        | STRING { $$ = newASTConstNode(STR_TYPE, $1); }
        | CIERTO { $$ = newASTConstNode(BOOL_TYPE, $1); }
        | FALSO { $$ = newASTConstNode(BOOL_TYPE, $1); }
        ;

assignment: var_ref ASSIGN expression
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    initSymbolTable();

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

    printSymbolTable(yyout);

    yyout = fopen("queue.out", "w");
    if (yyout == NULL) {
        printf("Error: Unable to open file\n");
        return 1;
    }

    printRevisitQueue(yyout);
    fclose(yyout);

    return flag;
}

void addToNames(StorageNode* node) {
    if (name_count == 0) {
        names = (StorageNode**)malloc(sizeof(StorageNode*));
        names[0] = node;
        name_count++;
    } else {
        name_count++;
        names = (StorageNode**)realloc(names, name_count * sizeof(StorageNode*));
        names[name_count - 1] = node;
    }
}

void addToVars(ValueType var) {
    if (var_count == 0) {
        vars = (ValueType*)malloc(sizeof(ValueType));
        vars[0] = var;
        var_count++;
    } else {
        var_count++;
        vars = (ValueType*)realloc(vars, var_count * sizeof(ValueType));
        vars[var_count - 1] = var;
    }
}

void yyerror() {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, yytext); 
    exit(1);
}