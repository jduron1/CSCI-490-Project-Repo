%code requires {
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symbol_table.h"
}

%{
    #include "semantics.h"
    #include "ast.h"
    #include "code_gen.h"

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    extern FILE *yyout;

    extern int yylex();
    void yyerror();

    ASTNode *root;
    ASTFuncDecl *temp_decl;

    void addToNames(StorageNode *);
    StorageNode **names;
    int name_count = 0;

    void addToVals(ValueType);
    ValueType *vals;
    int val_count = 0;

    void addElseIf(ASTNode *);
    ASTNode **else_ifs;
    int else_if_count = 0;
%}

%union {
    ValueType value;
    Argument arg;
    ASTNode *node;
    StorageNode *item;

    int data_type;
    int const_type;
    char *array_size;
}

%token <value> CARACTER BOOLEANO ENTERO REAL CADENA VACIO
%token <value> FUNCION CIERTO FALSO SI SINO
%token <value> POR MIENTRAS PARAR CONTINUAR REGRESAR
%token <value> ADD SUB MUL DIV MOD EXP ADD_ASSIGN SUB_ASSIGN
%token <value> MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN
%token <value> OR AND NOT EQ NE LESS LE GREAT GE ARROW LPAREN RPAREN
%token <value> LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA
%token <value> ASSIGN AMPERSAND ELLIPSIS EN
%token <item> IDENTIFIER
%token <value> INTEGER
%token <value> FLOAT
%token <value> CHARACTER
%token <value> STRING

%left COMMA
%right ASSIGN
%right ADD_ASSIGN SUB_ASSIGN
%right MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%left OR
%left AND
%left EQ NE
%left LESS LE GREAT GE
%left ADD SUB
%left MUL DIV MOD
%left EXP
%right NOT AMPERSAND NEGATIVE
%left LPAREN RPAREN LBRACKET RBRACKET

%type <node> parts function global
%type <node> body declaration statement
%type <node> assignment
%type <node> if else_if else
%type <node> for while
%type <node> call
%type <node> return
%type <node> arguments parameter argument
%type <item> variable init
%type <node> expression
%type <node> constant
%type <data_type> type

%start program

%%

program: parts
        {
            root = (ASTNode *)$1;
        }
       ;

parts: parts global
        {
            $$ = $2;
        }
     | parts function
        {
            ASTFuncDecls *temp = (ASTFuncDecls *)$1;
            $$ = newASTFuncDeclsNode(temp -> func_declarations, temp -> func_declaration_count, $2);
        }
     | global
        {
            $$ = $1;
        }
     | function
        {
            $$ = newASTFuncDeclsNode(NULL, 0, $1);
        }
     ;

global: declaration { $$ = $1; }
      | assignment { $$ = $1; }
      ;

function: { incrScope(); } head tail
            {
                revisit(temp_decl -> entry -> storage_name);
                hideScope();
                $$ = (ASTNode *)temp_decl;
            }
        ;

head: FUNCION { function_declared = 1; } IDENTIFIER LPAREN arguments RPAREN ARROW type
        {
            function_declared = 0;

            ASTReturnType *temp = (ASTReturnType *)$8;
            temp_decl = (ASTFuncDecl *)newASTFuncDeclNode(temp -> ret_type, temp -> pointer, $3);

            temp_decl -> entry -> storage_type = FUNCTION_TYPE;
            temp_decl -> entry -> inferred_type = temp -> ret_type;

            if ($5 != NULL) {
                ASTDeclArgs *temp2 = (ASTDeclArgs *)$5;

                temp_decl -> entry -> args = temp2 -> args;
                temp_decl -> entry -> arg_count = temp2 -> arg_count;
            } else {
                temp_decl -> entry -> args = NULL;
                temp_decl -> entry -> arg_count = 0;
            }
        }
    ;

tail: LBRACE body RBRACE

arguments: arguments COMMA parameter
            {
                ASTDeclArgs *temp = (ASTDeclArgs *)$1;
                $$ = newASTDeclArgsNode(temp -> args, temp -> arg_count, $3);
            }
         | arguments COMMA argument
            {
                ASTCallArgs *temp = (ASTCallArgs *)$1;
                $$ = newASTCallArgsNode(temp -> args, temp -> arg_count, $3);
            }
         | parameter
            {
                $$ = newASTDeclArgsNode(NULL, 0, $1);
            }
         | argument
            {
                $$ = newASTCallArgsNode(NULL, 0, $1);
            }
         ;

parameter: { declared = 1; } type variable
            {
                declared = 0;
                
                switch ($3 -> storage_type) {
                    case UNDEF:
                        setDataType($3 -> storage_name, $2, UNDEF);
                        break;
                    case POINTER_TYPE:
                        setDataType($3 -> storage_name, POINTER_TYPE, $2);
                        break;
                    case ARRAY_TYPE:
                        setDataType($3 -> storage_name, ARRAY_TYPE, $2);
                        break;
                    default:
                        break;
                }

                $$ = defArg($2, type, $3 -> storage_name, 0);
            }
         ;

argument: expression { $$ = $1; }
        ;

type: BOOLEANO { $$ = BOOL_TYPE; }
    | CARACTER { $$ = CHAR_TYPE; }
    | ENTERO { $$ = INT_TYPE; }
    | REAL { $$ = REAL_TYPE; }
    | CADENA { $$ = STRING_TYPE; }
    | VACIO { $$ = VOID_TYPE; }
    ;

variable: IDENTIFIER
            {
                $$ = $1;

                if ($$ -> storage_type == ARRAY_TYPE) {
                    if ($$ -> indices != NULL) {
                        for (int i = 0; i < $$ -> index_count; i++) {
                            free($$ -> indices[i]);
                            $$ -> indices[i] = NULL;
                        }

                        free($$ -> indices);
                        $$ -> indices = NULL;
                    }
                }
            }
        | pointer IDENTIFIER
            {
                $2 -> storage_type = POINTER_TYPE;
                $$ = $2;
            }
        | reference IDENTIFIER
        | IDENTIFIER LBRACKET RBRACKET
        | pointer IDENTIFIER LBRACKET RBRACKET
        | AMPERSAND IDENTIFIER LBRACKET RBRACKET
        | IDENTIFIER LBRACKET expression RBRACKET
        | pointer IDENTIFIER LBRACKET expression RBRACKET
        | AMPERSAND IDENTIFIER LBRACKET expression RBRACKET
        ;

pointer: MUL
       ;

reference: AMPERSAND
         ;

body: body declaration
        {
            $$ = newASTDeclsNode(((ASTDecls *)$1) -> declarations, ((ASTDecls *)$1) -> declaration_count, $2);
        }
    | declaration
        {
            $$ = newASTDeclsNode(NULL, 0, $1);
        }
    | body statement
    | statement
    ;

declaration: type { declared = 1; } names { declared = 0; } SEMICOLON
            {
                $$ = newASTDeclNode($1, names, name_count);
                name_count = 0;
                ASTDecl* temp = (ASTDecl *)$$;

                for (int i = 0; i < temp -> names_count; i++) {
                    switch (temp -> names[i] -> storage_type) {
                        case UNDEF:
                            setDataType(temp -> names[i] -> storage_name, temp -> data_type, UNDEF);
                            break;
                        case POINTER_TYPE:
                            setDataType(temp -> names[i] -> storage_name, POINTER_TYPE, temp -> data_type);
                            break;
                        case ARRAY_TYPE:
                            setDataType(temp -> names[i] -> storage_name, ARRAY_TYPE, temp -> data_type);
                            break;
                        default:
                            break;
                    }
                }
            }
           ;

names: names COMMA variable { addToNames($3); }
     | names COMMA init { addToNames($3); }
     | variable { addToNames($1); }
     | init { addToNames($1); }
     ;

init: IDENTIFIER ASSIGN expression
        {
            $1 -> assigned = (ASTNode *)$3;
            $$ = $1;
        }
    | IDENTIFIER LBRACKET RBRACKET ASSIGN LBRACE values RBRACE
    ;

values: values COMMA constant
        {
            ASTConst *temp = (ASTConst *)$3;
            addToVals(temp -> val);
        }
      | constant
        {
            ASTConst *temp = (ASTConst *)$1;
            addToVals(temp -> val);
        }
      ;

statement: if { $$ = $1; }
         | while { $$ = $1; }
         | for { $$ = $1; }
         | return { $$ = $1; }
         | break { $$ = newASTSimpleNode(0); }
         | continue { $$ = newASTSimpleNode(1); }
         | assignment { $$ = $1; }
         | call SEMICOLON { $$ = $1; }
         ;

if: SI expression LBRACE body RBRACE else_if else
  | SI expression LBRACE body RBRACE else
  ;

else_if: else_if SINO SI expression LBRACE body RBRACE
       | SINO SI expression LBRACE body RBRACE
       ;

else: SINO LBRACE body RBRACE
    | %empty
    ;

while: MIENTRAS expression LBRACE body RBRACE
     ;

for: POR IDENTIFIER EN expression ELLIPSIS expression LBRACE body RBRACE
   | POR type IDENTIFIER EN expression ELLIPSIS expression LBRACE body RBRACE
   ;

return: REGRESAR SEMICOLON
        {
            $$ = newASTReturnNode(temp_decl -> ret_type, NULL);
        }
      | REGRESAR expression SEMICOLON
        {
            $$ = newASTReturnNode(temp_decl -> ret_type, $2);
        }
      ;

break: PARAR SEMICOLON
     ;

continue: CONTINUAR SEMICOLON
        ;

assignment: variable ASSIGN expression SEMICOLON
            {
                ASTRef *temp = (ASTRef *)$1;
                $$ = newASTAssignNode(temp -> entry, temp -> ref, $3);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue *q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void **)malloc(sizeof(void *));
                    } else {
                        q -> nodes = (void **)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void *));
                    }

                    q -> nodes[q -> assign_count] = (void *)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, NONE);
                }
            }
          | variable ADD_ASSIGN expression SEMICOLON
          | variable SUB_ASSIGN expression SEMICOLON
          | variable MUL_ASSIGN expression SEMICOLON
          | variable DIV_ASSIGN expression SEMICOLON
          | variable MOD_ASSIGN expression SEMICOLON
          | variable EXP_ASSIGN expression SEMICOLON
          ;

call: IDENTIFIER LPAREN arguments RPAREN
        {
            ASTCallArgs *temp = (ASTCallArgs *)$3;
            $$ = newASTFuncCallNode($1, temp -> args, temp -> arg_count);

            RevisitQueue *q = searchQueue($1 -> storage_name);

            if (q != NULL) {
                if (q -> call_count == 0) {
                    q -> arg_types = (int **)malloc(sizeof(int *));
                    q -> arg_count = (int *)malloc(sizeof(int));
                } else {
                    q -> arg_types = (int **)realloc(q -> arg_types, (q -> call_count + 1) * sizeof(int *));
                    q -> arg_count = (int *)realloc(q -> arg_count, (q -> call_count + 1) * sizeof(int));
                }

                q -> arg_count[q -> call_count] = temp -> arg_count;
                q -> arg_types[q -> call_count] = (int *)malloc(temp -> arg_count * sizeof(int));

                for (int i = 0; i < temp -> arg_count; i++) {
                    q -> arg_types[q -> call_count][i] = getExpressionType(temp -> args[i]);
                }

                q -> call_count++;
            } else {
                if ($1 -> storage_type == FUNCTION_TYPE) {
                    if ($1 -> arg_count != temp -> arg_count) {
                        fprintf(stderr, "Error at line %d: incorrect number of arguments in function call.\n", yylineno);
                        exit(1);
                    }

                    for (int i = 0; i < temp -> arg_count; i++) {
                        int type_1 = getExpressionType(temp -> args[i]);
                        int type_2 = $1 -> args[i].arg_type;

                        getResultType(type_1, type_2, NONE);
                    }
                }
            }
        }
    ;

expression: expression ADD expression
            {
                $$ = newASTArithNode(OP_ADD, $1, $3);
            }
          | expression SUB expression
          | expression MUL expression
          | expression DIV expression
          | expression MOD expression
          | expression EXP expression
          | expression OR expression
          | expression AND expression
          | expression EQ expression
          | expression NE expression
          | expression LESS expression
          | expression LE expression
          | expression GREAT expression
          | expression GE expression
          | NOT expression
          | LPAREN expression RPAREN
          | variable
          | constant
            {
                $$ = $1;
            }
          | ADD constant %prec NEGATIVE
          | SUB constant %prec NEGATIVE
          | call
          ;

constant: CHARACTER { $$ = newASTConstNode(CHAR_TYPE, $1); }
        | CIERTO { $$ = newASTConstNode(BOOL_TYPE, $1); }
        | FALSO { $$ = newASTConstNode(BOOL_TYPE, $1); }
        | INTEGER { $$ = newASTConstNode(INT_TYPE, $1); }
        | FLOAT { $$ = newASTConstNode(FLOAT_TYPE, $1); }
        | STRING { $$ = newASTConstNode(STRING_TYPE, $1); }
        ;

%%

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Error: Cannot open file %s.\n", argv[1]);
        return 1;
    }

    printf("Syntax check: %s\n", yyparse() == 0 ? "Success" : "Failure");

    return 0;
}

void yyerror() {
    fprintf(stderr, "Syntax error at line %d.\n", yylineno); 
    exit(1);
}