%{
    #include "semantics.c"
	#include "symbol_table.c"
    #include "code_generation.c"
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

    ASTNode* root;
    void freeAST(ASTNode*);

    void addToNames(StorageNode*);
    StorageNode** names;
    int name_count = 0;

    void addToVals(ValueType);
    ValueType* vals;
    int val_count = 0;

    void addElseIf(ASTNode*);
    ASTNode** else_ifs;
    int else_if_count = 0;

    ASTFuncDecl* temp_decl;

    int check_size = 0;
%}

%union {
    StorageNode* item;
    ASTNode* node;
    ValueType val;
    Argument arg;

    int data_type;
    int const_type;
    char* array_size;
}

%token <val> CARACTER BOOLEANO ENTERO REAL CADENA VACIO
%token <val> FUNCION CIERTO FALSO SI SINO
%token <val> POR MIENTRAS PARAR CONTINUAR REGRESAR
%token <val> ADD SUB MUL DIV MOD EXP ADD_ASSIGN SUB_ASSIGN
%token <val> MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN EXP_ASSIGN
%token <val> OR AND NOT EQ NE LESS LE GREAT GE ARROW LPAREN RPAREN
%token <val> LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA
%token <val> ASSIGN AMPERSAND ELLIPSIS EN
%token <item> IDENTIFIER
%token <val> INTEGER
%token <val> DECIMAL
%token <val> CHARACTER
%token <val> STRING

%left COMMA
%right ASSIGN
%right ADD_ASSIGN SUB_ASSIGN
%right MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%right EXP_ASSIGN
%left OR
%left AND
%left EQ NE
%left LESS LE GREAT GE
%left ADD SUB
%left MUL DIV MOD
%left EXP
%right NOT AMPERSAND NEGATIVE
%left LPAREN RPAREN LBRACKET RBRACKET

%type <node> program
%type <node> declarations declaration
%type <data_type> type
%type <item> variable
%type <array_size> array
%type <item> init var_init array_init
%type <node> constant
%type <node> expression var_ref
%type <node> statement assignment
%type <node> statements tail
%type <node> if else_if else
%type <node> for while
%type <node> program_functions functions function
%type <node> call call_arguments call_argument
%type <node> function_arguments arguments
%type <arg> argument
%type <node> return_type

%start program

%%

program: program_functions
            {
                root = (ASTNode*)$1;
                traverseAST(root);

                ASTFuncDeclarations* temp = (ASTFuncDeclarations*)$$;

                for (int i = 0; i < temp -> func_declaration_count; i++) {
                    generateFuncDeclCode(of, temp -> func_declarations[i]);
                }
            }
       ;

declarations: declarations declaration
                {
                    $$ = newASTDeclarationsNode(((ASTDeclarations*)$1) -> declarations, ((ASTDeclarations*)$1) -> declaration_count, $2);
                }
            | declaration
                {
                    $$ = newASTDeclarationsNode(NULL, 0, $1);
                }
            ;

declaration: type { declared = 1; } names { declared = 0; } SEMICOLON
                {
                    $$ = newASTDeclNode($1, names, name_count);
                    
                    name_count = 0;

                    ASTDecl* temp = (ASTDecl*)$$;

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

type: ENTERO { $$ = INT_TYPE; }
    | REAL { $$ = REAL_TYPE; }
    | CARACTER { $$ = CHAR_TYPE; }
    | CADENA { $$ = STRING_TYPE; }
    | BOOLEANO { $$ = BOOL_TYPE; }
    | VACIO { $$ = VOID_TYPE; }
    ;

names: names COMMA variable { addToNames($3); }
     | names COMMA init { addToNames($3); }
     | variable { addToNames($1); }
     | init { addToNames($1); }
     ;

variable: IDENTIFIER
            {
                $$ = $1;

                if ($1 -> storage_type == ARRAY_TYPE) {
                    if ($1 -> indices != NULL) {
                        for (int i = 0; i < $1 -> index_count; i++) {
                            free($1 -> indices[i]);
                        }

                        free($1 -> indices);
                    }

                    $1 -> indices = NULL;
                }
            }
        | pointer IDENTIFIER
            {
                $2 -> storage_type = POINTER_TYPE;
                $$ = $2;
            }
        | IDENTIFIER array
            {
                if (declared) {
                    char temp[32];

                    sprintf(temp, "%d", 0);

                    $1 -> storage_type = ARRAY_TYPE;
                    $1 -> vals = NULL;

                    $1 -> array_size = (char*)malloc(strlen(temp) + 1);
                    if ($1 -> array_size == NULL) {
                        fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                        exit(1);
                    }

                    strcpy($1 -> array_size, temp);

                    $$ = $1;
                }
            
                if ($2 != NULL) {
                    if ($1 -> indices == NULL) {
                        $1 -> indices = (char**)malloc(sizeof(char*));
                        $1 -> index_count = 1;
                    } else {
                        $1 -> indices = (char**)realloc($1 -> indices, ($1 -> index_count + 1) * sizeof(char*));
                        $1 -> index_count++;
                    }

                    int length = strlen($2);
                    $1 -> indices[$1 -> index_count - 1] = (char*)malloc((length + 1) * sizeof(char));
                    if ($1 -> indices[$1 -> index_count - 1] == NULL) {
                        fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                        exit(1);
                    }

                    strcpy($1 -> indices[$1 -> index_count - 1], $2);
                    $1 -> indices[$1 -> index_count - 1][length] = '\0';
                }
            }
        | pointer IDENTIFIER array
            {
                if (declared) {
                    char temp[32];

                    sprintf(temp, "%d", 0);

                    $2 -> storage_type = ARRAY_TYPE;
                    $2 -> vals = NULL;

                    $2 -> array_size = (char*)malloc(strlen(temp) + 1);
                    if ($2 -> array_size == NULL) {
                        fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                        exit(1);
                    }

                    strcpy($2 -> array_size, temp);

                    $$ = $2;
                }
            
                if ($3 != NULL) {
                    if ($2 -> indices == NULL) {
                        $2 -> indices = (char**)malloc(sizeof(char*));
                        $2 -> index_count = 1;
                    } else {
                        $2 -> indices = (char**)realloc($2 -> indices, ($2 -> index_count + 1) * sizeof(char*));
                        $2 -> index_count++;
                    }

                    int length = strlen($3);
                    $2 -> indices[$2 -> index_count - 1] = (char*)malloc((length + 1) * sizeof(char));
                    if ($2 -> indices[$2 -> index_count - 1] == NULL) {
                        fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                        exit(1);
                    }

                    strcpy($2 -> indices[$2 -> index_count - 1], $3);
                    $2 -> indices[$2 -> index_count - 1][length] = '\0';
                }
            }
        ;

pointer: MUL
       ;

array: LBRACKET INTEGER RBRACKET
        {
            char temp[32];
            sprintf(temp, "%lld", $2.integer);

            $$ = (char*)malloc(strlen(temp) + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s", temp);
            check_size = $2.integer;
        }
     | LBRACKET IDENTIFIER RBRACKET
        {
            char temp[strlen($2 -> storage_name) + 1];
            strcpy(temp, $2 -> storage_name);
            
            $$ = (char*)malloc(strlen(temp) + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            strcpy($$, temp);
            check_size = $2 -> val.integer;
        }
     | LBRACKET INTEGER ADD IDENTIFIER RBRACKET
        {
            char temp[32];
            sprintf(temp, "%lld", $2.integer);

            $$ = (char*)malloc(strlen(temp) + strlen($4 -> storage_name) + strlen(" + ") + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s + %s", temp, $4 -> storage_name);

            check_size = $2.integer + $4 -> val.integer;
        }
     | LBRACKET IDENTIFIER ADD INTEGER RBRACKET
        {
            char temp[32];
            sprintf(temp, "%lld", $4.integer);

            $$ = (char*)malloc(strlen($2 -> storage_name) + strlen(temp) + strlen(" + ") + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s + %s", $2 -> storage_name, temp);

            check_size = $2 -> val.integer + $4.integer;
        }
     | LBRACKET IDENTIFIER ADD IDENTIFIER RBRACKET
        {
            $$ = (char*)malloc(strlen($2 -> storage_name) + strlen($4 -> storage_name) + strlen(" + ") + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s + %s", $2 -> storage_name, $4 -> storage_name);

            check_size = $2 -> val.integer + $4 -> val.integer;
        }
     | LBRACKET INTEGER SUB IDENTIFIER RBRACKET
        {
            char temp[32];
            sprintf(temp, "%lld", $2.integer);

            $$ = (char*)malloc(strlen(temp) + strlen($4 -> storage_name) + strlen(" - ") + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s - %s", temp, $4 -> storage_name);

            check_size = $2.integer - $4 -> val.integer;
        }
     | LBRACKET IDENTIFIER SUB INTEGER RBRACKET
        {
            char temp[32];
            sprintf(temp, "%lld", $4.integer);

            $$ = (char*)malloc(strlen($2 -> storage_name) + strlen(temp) + strlen(" - ") + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s - %s", $2 -> storage_name, temp);

            check_size = $2 -> val.integer - $4.integer;
        }
     | LBRACKET IDENTIFIER SUB IDENTIFIER RBRACKET
        {
            $$ = (char*)malloc(strlen($2 -> storage_name) + strlen($4 -> storage_name) + strlen(" - ") + 1);
            if ($$ == NULL) {
                fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                exit(1);
            }

            sprintf($$, "%s - %s", $2 -> storage_name, $4 -> storage_name);

            check_size = $2 -> val.integer - $4 -> val.integer;
        }
      | LBRACKET RBRACKET
        {
            $$ = NULL;
        }
     ;

init: var_init { $$ = $1; }
    | array_init { $$ = $1; }
    ;

var_init: IDENTIFIER ASSIGN constant
            {
                ASTConst* temp = (ASTConst*)$3;

                $1 -> val = temp -> val;
                $1 -> storage_type = temp -> const_type;
                $$ = $1;
            }
        ;

array_init: IDENTIFIER LBRACKET RBRACKET ASSIGN LBRACE values RBRACE
            {
                if (val_count == 0) {
                    fprintf(stderr, "Error at line %d: no values in array initialization.\n", yylineno);
                    exit(1);
                }

                char temp[32];

                $1 -> storage_type = ARRAY_TYPE;
                $1 -> vals = vals;

                sprintf(temp, "%d", val_count);

                $1 -> array_size = (char*)malloc(strlen(temp) + 1);
                if ($1 -> array_size == NULL) {
                    fprintf(stderr, "Error at line %d: memory allocation failed.\n", yylineno);
                    exit(1);
                }

                strcpy($1 -> array_size, temp);

                $$ = $1;
            }
          ;

values: values COMMA constant
        {
            ASTConst* temp = (ASTConst*)$3;
            addToVals(temp -> val);
        }
      | constant
        {
            ASTConst* temp = (ASTConst*)$1;
            addToVals(temp -> val);
        }
      ;

statements: statements statement
            {
                $$ = newStatementsNode(((ASTStatements*)$1) -> statements, ((ASTStatements*)$1) -> statement_count, $2);
            }
          | statement
            {
                $$ = newStatementsNode(NULL, 0, $1);
            }
          ;

statement: if
            {
                $$ = $1;
            }
         | for
            {
                $$ = $1;
            }
         | while
            {
                $$ = $1;
            }
         | assignment SEMICOLON
            {
                $$ = $1;
            }
         | CONTINUAR SEMICOLON
            {
                $$ = newASTSimpleNode(0);
            }
         | PARAR SEMICOLON
            {
                $$ = newASTSimpleNode(1);
            }
         | call SEMICOLON
            {
                $$ = $1;
            }
         ;

if: SI expression tail else_if else
    {
        $$ = newASTIfNode($2, $3, else_ifs, else_if_count, $5);
        else_if_count = 0;
        else_ifs = NULL;
    }
  | SI expression tail else
    {
        $$ = newASTIfNode($2, $3, NULL, 0, $4);
    }
  ;

else_if: else_if SINO SI expression tail
        {
            ASTNode* temp = newASTElseIfNode($4, $5);
            addElseIf(temp);
        }
       | SINO SI expression tail
        {
            ASTNode* temp = newASTElseIfNode($3, $4);
            addElseIf(temp);
        }
       ;

else: SINO tail
        {
            $$ = $2;
        }
    | %empty
        {
            $$ = NULL;
        }
    ;

for: POR IDENTIFIER EN INTEGER ELLIPSIS INTEGER tail
        {
            ASTNode* temp;
            ASTNode* temp2;

            if ($4.integer < $6.integer) {
                temp = newASTIncrNode($2, 0, 0);
                temp2 = newASTRelNode(OP_GE, newASTRefNode($2, 0), newASTConstNode(INT_TYPE, $6));
            } else {
                temp = newASTIncrNode($2, 1, 0);
                temp2 = newASTRelNode(OP_LE, newASTRefNode($2, 0), newASTConstNode(INT_TYPE, $6));
            }

            ASTNode* temp3 = newASTAssignNode($2, 0, newASTConstNode(INT_TYPE, $4));

            $$ = newASTForNode(temp3, temp2, temp, $7);
            setLoopCounter($$);
        }
    | POR IDENTIFIER EN INTEGER ELLIPSIS IDENTIFIER tail
        {
            ASTNode* temp;
            ASTNode* temp2;

            if ($4.integer < $6 -> val.integer) {
                temp = newASTIncrNode($2, 0, 0);
                temp2 = newASTRelNode(OP_GE, newASTRefNode($2, 0), newASTRefNode($6, 0));
            } else {
                temp = newASTIncrNode($2, 1, 0);
                temp2 = newASTRelNode(OP_LE, newASTRefNode($2, 0), newASTRefNode($6, 0));
            }

            ASTNode* temp3 = newASTAssignNode($2, 0, newASTConstNode(INT_TYPE, $4));

            $$ = newASTForNode(temp3, temp2, temp, $7);
            setLoopCounter($$);
        }
    | POR IDENTIFIER EN INTEGER ELLIPSIS IDENTIFIER ADD INTEGER tail
        {
            ASTNode* temp;
            ASTNode* temp2;

            if ($4.integer < $6 -> val.integer + $8.integer) {
                temp = newASTIncrNode($2, 0, 0);
                temp2 = newASTRelNode(OP_GE, newASTRefNode($2, 0), newASTArithNode(OP_ADD, newASTRefNode($6, 0), newASTConstNode(INT_TYPE, $8)));
            } else {
                temp = newASTIncrNode($2, 1, 0);
                temp2 = newASTRelNode(OP_LE, newASTRefNode($2, 0), newASTArithNode(OP_ADD, newASTRefNode($6, 0), newASTConstNode(INT_TYPE, $8)));
            }

            ASTNode* temp3 = newASTAssignNode($2, 0, newASTConstNode(INT_TYPE, $4));

            $$ = newASTForNode(temp3, temp2, temp, $9);
            setLoopCounter($$);
        }
    | POR IDENTIFIER EN INTEGER ELLIPSIS IDENTIFIER SUB INTEGER tail
        {
            ASTNode* temp;
            ASTNode* temp2;

            if ($4.integer < $6 -> val.integer - $8.integer) {
                temp = newASTIncrNode($2, 0, 0);
                temp2 = newASTRelNode(OP_GE, newASTRefNode($2, 0), newASTArithNode(OP_SUB, newASTRefNode($6, 0), newASTConstNode(INT_TYPE, $8)));
            } else {
                temp = newASTIncrNode($2, 1, 0);
                temp2 = newASTRelNode(OP_LE, newASTRefNode($2, 0), newASTArithNode(OP_SUB, newASTRefNode($6, 0), newASTConstNode(INT_TYPE, $8)));
            }

            ASTNode* temp3 = newASTAssignNode($2, 0, newASTConstNode(INT_TYPE, $4));

            $$ = newASTForNode(temp3, temp2, temp, $9);
            setLoopCounter($$);
        }
   ;

while: MIENTRAS expression tail
        {
            $$ = newASTWhileNode($2, $3);
        }
     ;

tail: LBRACE statements RBRACE
        {
            $$ = $2;
        }
    ;

expression: expression ADD expression
            {
                $$ = newASTArithNode(OP_ADD, $1, $3);
            }
          | expression SUB expression
            {
                $$ = newASTArithNode(OP_SUB, $1, $3);
            }
          | expression MUL expression
            {
                $$ = newASTArithNode(OP_MUL, $1, $3);
            }
          | expression DIV expression
            {
                $$ = newASTArithNode(OP_DIV, $1, $3);
            }
          | expression MOD expression
            {
                $$ = newASTArithNode(OP_MOD, $1, $3);
            }
          | expression EXP expression
            {
                $$ = newASTArithNode(OP_EXP, $1, $3);
            }
          | expression OR expression
            {
                $$ = newASTBoolNode(OP_OR, $1, $3);
            }
          | expression AND expression
            {
                $$ = newASTBoolNode(OP_AND, $1, $3);
            }
          | NOT expression
            {
                $$ = newASTBoolNode(OP_NOT, $2, NULL);
            }
          | expression EQ expression
            {
                $$ = newASTEquNode(OP_EQUAL, $1, $3);
            }
          | expression NE expression
            {
                $$ = newASTEquNode(OP_NOT_EQUAL, $1, $3);
            }
          | expression LESS expression
            {
                $$ = newASTRelNode(OP_LESS, $1, $3);
            }
          | expression LE expression
            {
                $$ = newASTRelNode(OP_LE, $1, $3);
            }
          | expression GREAT expression
            {
                $$ = newASTRelNode(OP_GREAT, $1, $3);
            }
          | expression GE expression
            {
                $$ = newASTRelNode(OP_GE, $1, $3);
            }
          | LPAREN expression RPAREN
            {
                $$ = newASTParenNode($2);
            }
          | var_ref
            {
                $$ = $1;
            }
          | constant
            {
                $$ = $1;
            }
          | ADD constant %prec NEGATIVE
            {
                fprintf(stderr, "Error at line %d: sign before constant can not be \'+\'.\n", yylineno);
                exit(1);
            }
          | SUB constant %prec NEGATIVE
            {
                ASTConst* temp = (ASTConst*)$2;

                switch (temp -> const_type) {
                    case INT_TYPE:
                        temp -> val.integer *= -1;
                        break;
                    case REAL_TYPE:
                        temp -> val.real *= -1;
                        break;
                    case CHAR_TYPE:
                        fprintf(stderr, "Error at line %d: sign before char constant.\n", yylineno);
                        exit(1);
                        break;
                    case STRING_TYPE:
                        fprintf(stderr, "Error at line %d: sign before string constant.\n", yylineno);
                        exit(1);
                        break;
                    case BOOL_TYPE:
                        fprintf(stderr, "Error at line %d: sign before boolean constant.\n", yylineno);
                        exit(1);
                        break;
                    default:
                        break;
                }

                $$ = (ASTNode*)temp;
            }
          | call
            {
                $$ = $1;
            }
          ;

constant: INTEGER { $$ = newASTConstNode(INT_TYPE, $1); }
        | DECIMAL { $$ = newASTConstNode(REAL_TYPE, $1); }
        | CHARACTER { $$ = newASTConstNode(CHAR_TYPE, $1); }
        | STRING { $$ = newASTConstNode(STRING_TYPE, $1); }
        | CIERTO { $$ = newASTConstNode(BOOL_TYPE, $1); }
        | FALSO { $$ = newASTConstNode(BOOL_TYPE, $1); }
        ;

assignment: var_ref ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTAssignNode(temp -> entry, temp -> ref, $3);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, NONE);
                }
            }
          | var_ref ADD_ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTArithAssignNode(temp -> entry, temp -> ref, $3, $1, $3, OP_ADD_ASSIGN);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, ARITH_ASSIGN_OP);
                }
            }
          | var_ref SUB_ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTArithAssignNode(temp -> entry, temp -> ref, $3, $1, $3, OP_SUB_ASSIGN);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, ARITH_ASSIGN_OP);
                }
            }
          | var_ref MUL_ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTArithAssignNode(temp -> entry, temp -> ref, $3, $1, $3, OP_MUL_ASSIGN);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, ARITH_ASSIGN_OP);
                }
            }
          | var_ref DIV_ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTArithAssignNode(temp -> entry, temp -> ref, $3, $1, $3, OP_DIV_ASSIGN);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, ARITH_ASSIGN_OP);
                }
            }
          | var_ref MOD_ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTArithAssignNode(temp -> entry, temp -> ref, $3, $1, $3, OP_MOD_ASSIGN);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, ARITH_ASSIGN_OP);
                }
            }
          | var_ref EXP_ASSIGN expression
            {
                ASTRef* temp = (ASTRef*)$1;
                $$ = newASTArithAssignNode(temp -> entry, temp -> ref, $3, $1, $3, OP_EXP_ASSIGN);
                int type_1 = getDataType(temp -> entry -> storage_name);
                int type_2 = getExpressionType($3);

                if (contains_revisit) {
                    RevisitQueue* q = searchQueue(temp -> entry -> storage_name);

                    if (q == NULL) {
                        pushToQueue(temp -> entry, temp -> entry -> storage_name, ASSIGN_CHECK);
                        q = searchQueue(temp -> entry -> storage_name);
                    }

                    if (q -> assign_count == 0) {
                        q -> nodes = (void**)malloc(sizeof(void*));
                    } else {
                        q -> nodes = (void**)realloc(q -> nodes, (q -> assign_count + 1) * sizeof(void*));
                    }

                    q -> nodes[q -> assign_count] = (void*)$3;
                    q -> assign_count++;
                    contains_revisit = 0;

                    printf("Assignment revisit for %s at line %d.\n", temp -> entry -> storage_name, yylineno);
                } else {
                    getResultType(type_1, type_2, ARITH_ASSIGN_OP);
                }
            }
          ;

var_ref: variable
            {
                $$ = newASTRefNode($1, 0);
            }
       | AMPERSAND variable
            {
                $$ = newASTRefNode($2, 1);
            }
       ;

call: IDENTIFIER LPAREN call_arguments RPAREN
        {
            ASTCallArgs* temp = (ASTCallArgs*)$3;
            $$ = newASTFuncCallNode($1, temp -> args, temp -> arg_count);

            RevisitQueue* q = searchQueue($1 -> storage_name);

            if (q != NULL) {
                if (q -> call_count == 0) {
                    q -> arg_types = (int**)malloc(sizeof(int*));
                    q -> arg_count = (int*)malloc(sizeof(int));
                } else {
                    q -> arg_types = (int**)realloc(q -> arg_types, (q -> call_count + 1) * sizeof(int*));
                    q -> arg_count = (int*)realloc(q -> arg_count, (q -> call_count + 1) * sizeof(int));
                }

                q -> arg_count[q -> call_count] = temp -> arg_count;
                q -> arg_types[q -> call_count] = (int*)malloc(temp -> arg_count * sizeof(int));

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

call_arguments: call_argument
                {
                    $$ = $1;
                }
              | %empty
                {
                    ASTCallArgs* temp = (ASTCallArgs*)malloc(sizeof(ASTCallArgs));

                    temp -> type = CALL_ARGS;
                    temp -> args = NULL;
                    temp -> arg_count = 0;

                    $$ = (ASTNode*)temp;
                }
              ;

call_argument: call_argument COMMA expression
                {
                    ASTCallArgs* temp = (ASTCallArgs*)$1;
                    $$ = newASTCallArgsNode(temp -> args, temp -> arg_count, $3);
                }
             | expression
                {
                    $$ = newASTCallArgsNode(NULL, 0, $1);
                }
             ;

program_functions: functions
                    {
                        $$ = $1;
                    }
                  | %empty
                    {
                        $$ = NULL;
                    }
                  ;

functions: functions function
            {
                ASTFuncDeclarations* temp = (ASTFuncDeclarations*)$1;
                $$ = newASTFuncDeclarationsNode(temp -> func_declarations, temp -> func_declaration_count, $2);
            }
         | function
            {
                $$ = newASTFuncDeclarationsNode(NULL, 0, $1);
            }
         ;

function: { incrScope(); } f_head f_tail
            {
                revisit(temp_decl -> entry -> storage_name);

                hideScope();
                
                $$ = (ASTNode*)temp_decl;
            }
        ;

f_head: FUNCION { function_declared = 1; } IDENTIFIER LPAREN function_arguments RPAREN ARROW return_type
        {
            function_declared = 0;

            ASTReturnType* temp = (ASTReturnType*)$8;
            temp_decl = (ASTFuncDecl*)newASTFuncDeclNode(temp -> ret_type, temp -> pointer, $3);

            temp_decl -> entry -> storage_type = FUNCTION_TYPE;
            temp_decl -> entry -> inferred_type = temp -> ret_type;

            if ($5 != NULL) {
                ASTDeclArgs* temp2 = (ASTDeclArgs*)$5;

                temp_decl -> entry -> args = temp2 -> args;
                temp_decl -> entry -> arg_count = temp2 -> arg_count;
            } else {
                temp_decl -> entry -> args = NULL;
                temp_decl -> entry -> arg_count = 0;
            }
        }
      ;

return_type: type
            {
                $$ = newASTReturnTypeNode($1, 0);
            }
           | type pointer
            {
                $$ = newASTReturnTypeNode($1, 1);
            }
           ;

function_arguments: arguments
                    {
                        $$ = $1;
                    }
                  | %empty
                    {
                        $$ = NULL;
                    }
                  ;

arguments: arguments COMMA argument
            {
                ASTDeclArgs* temp = (ASTDeclArgs*)$1;
                $$ = newASTDeclArgsNode(temp -> args, temp -> arg_count, $3);
            }
         | argument
            {
                $$ = newASTDeclArgsNode(NULL, 0, $1);
            }
         ;

argument: { declared = 1; } type variable
            {
                declared = 0;

                int type = $3 -> storage_type;

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

f_tail: LBRACE function_declarations function_statements function_return RBRACE
    ;

function_declarations: declarations
                        {
                            temp_decl -> declarations = $1;
                        }
                     | %empty
                        {
                            temp_decl -> declarations = NULL;
                        }
                     ;

function_statements: statements
                        {
                            temp_decl -> statements = $1;
                        }
                     | %empty
                        {
                            temp_decl -> statements = NULL;
                        }
                     ;

function_return: REGRESAR expression SEMICOLON
        {
            temp_decl -> return_node = newASTReturnNode(temp_decl -> ret_type, $2);
        }
      | %empty
        {
            temp_decl -> return_node = NULL;
        }
      ;

%%

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    int length = strlen(argv[1]) + 1;

    of = fopen("output.cpp", "w");
    if (of == NULL) {
        fprintf(stderr, "Error opening output file.\n");
        exit(1);
    }

    fprintf(of, "#include <bits/stdc++.h>\n");
    fprintf(of, "using namespace std;\n\n");

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

    RevisitQueue* q = searchPrevQueue("imprimir");
    if (q == NULL) {
        if (queue != NULL) {
            queue = queue -> next;
        }
    } else {
        if (q -> next != NULL) {
            q -> next = q -> next -> next;
        }
    }

    q = searchPrevQueue("leer");
    if (q == NULL) {
        if (queue != NULL) {
            queue = queue -> next;
        }
    } else {
        if (q -> next != NULL) {
            q -> next = q -> next -> next;
        }
    }

    if (queue != NULL) {
        RevisitQueue* cur = queue;

        while (cur != NULL) {
            if (cur -> revisit_type == ASSIGN_CHECK) {
                revisit(cur -> storage_name);
            }

            cur = cur -> next;
        }
    }

    if (queue != NULL) {
        printf("Unchecked item in the revisit queue.\n");
    }

    funcDeclaration("imprimir", VOID_TYPE, 1, NULL);
    funcDeclaration("leer", VOID_TYPE, 1, NULL);

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
        name_count = 1;
    } else {
        name_count++;
        names = (StorageNode**)realloc(names, name_count * sizeof(StorageNode*));
        names[name_count - 1] = node;
    }
}

void addToVals(ValueType val) {
    if (val_count == 0) {
        vals = (ValueType*)malloc(sizeof(ValueType));
        vals[0] = val;
        val_count = 1;
    } else {
        val_count++;
        vals = (ValueType*)realloc(vals, val_count * sizeof(ValueType));
        vals[val_count - 1] = val;
    }
}

void addElseIf(ASTNode* node) {
    if (else_if_count == 0) {
        else_ifs = (ASTNode**)malloc(sizeof(ASTNode*));
        else_ifs[0] = node;
        else_if_count = 1;
    } else {
        else_if_count++;
        else_ifs = (ASTNode**)realloc(else_ifs, else_if_count * sizeof(ASTNode*));
        else_ifs[else_if_count - 1] = node;
    }
}

void yyerror() {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, yytext); 
    exit(1);
}