#ifndef AST_H
#define AST_H

typedef enum NodeTypeEnum {
    BASIC_NODE,
    DECLARATIONS,
    DECL_NODE,
    CONST_NODE,
    STATEMENTS,
    IF_NODE,
    ELSEIF_NODE,
    FOR_NODE,
    WHILE_NODE,
    ASSIGN_NODE,
    ARITH_ASSIGN_NODE,
    SIMPLE_NODE,
    INCR_NODE,
    FUNC_CALL,
    CALL_ARGS,
    ARITH_NODE,
    BOOL_NODE,
    REL_NODE,
    EQU_NODE,
    REF_NODE,
    FUNC_DECLS,
    FUNC_DECL,
    RET_TYPE,
    DECL_ARGS,
    RETURN_NODE
} NodeType;

typedef enum ArithOpEnum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EXP,
    OP_INC,
    OP_DEC
} ArithOp;

typedef enum ArithAssignEnum {
    OP_ADD_ASSIGN,
    OP_SUB_ASSIGN,
    OP_MUL_ASSIGN,
    OP_DIV_ASSIGN,
    OP_MOD_ASSIGN,
    OP_EXP_ASSIGN
} ArithAssign;

typedef enum BoolOpEnum {
    OP_OR,
    OP_AND,
    OP_NOT
} BoolOp;

typedef enum RelOpEnum {
    OP_GREAT,
    OP_LESS,
    OP_GE,
    OP_LE
} RelOp;

typedef enum EquOpEnum {
    OP_EQUAL,
    OP_NOT_EQUAL
} EquOp;

typedef struct ASTNodeStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct *left;
    struct ASTNodeStruct *right;
} ASTNode;

typedef struct ASTDeclarations {
    enum NodeTypeEnum type;
    struct ASTNodeStruct** declarations;
    int declaration_count;
} ASTDeclarations;

typedef struct ASTDeclStruct {
    enum NodeTypeEnum type;
    int data_type;
    StorageNode** names;
    int names_count;
} ASTDecl;

typedef struct ASTConst {
    enum NodeTypeEnum type;
    int const_type;
    ValueType val;
} ASTConst;

typedef struct ASTStatements {
    enum NodeTypeEnum type;
    struct ASTNodeStruct** statements;
    int statement_count;
} ASTStatements;

typedef struct ASTIfStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct* condition;
    struct ASTNodeStruct* if_branch;
    struct ASTNodeStruct** elseif_branches;
    int elseif_count;
    struct ASTNodeStruct* else_branch;
} ASTIf;

typedef struct ASTElseIfStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct* condition;
    struct ASTNodeStruct* elseif_branch;
} ASTElseIf;

typedef struct ASTForStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct* initialize;
    struct ASTNodeStruct* condition;
    struct ASTNodeStruct* increment;
    struct ASTNodeStruct* for_branch;
    StorageNode* counter;
} ASTFor;

typedef struct ASTWhileStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct* condition;
    struct ASTNodeStruct* while_branch;
} ASTWhile;

typedef struct ASTAssignStruct {
    enum NodeTypeEnum type;
    StorageNode* entry;
    int ref;
    struct ASTNodeStruct* assign_val;
} ASTAssign;

typedef struct ASTArithAssignStruct {
    enum NodeTypeEnum type;
    StorageNode* entry;
    int ref;
    struct ASTNodeStruct* assign_val;
    enum ArithAssignEnum op;
    int data_type;
} ASTArithAssign;

typedef struct ASTSimpleStruct {
    enum NodeTypeEnum type;
    int statement_type;
} ASTSimple;

typedef struct ASTIncrStruct {
    enum NodeTypeEnum type;
    StorageNode* entry;
    int incr_type;
    int fix;
} ASTIncr;

typedef struct ASTFuncCallStruct {
    enum NodeTypeEnum type;
    StorageNode* entry;
    struct ASTNodeStruct** args;
    int arg_count;
} ASTFuncCall;

typedef struct ASTCallArgsStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct** args;
    int arg_count;
} ASTCallArgs;

typedef struct ASTArithStruct {
    enum NodeTypeEnum type;
    enum ArithOpEnum op;
    struct ASTNodeStruct* left;
    struct ASTNodeStruct* right;
    int data_type;
} ASTArith;

typedef struct ASTBoolStruct {
    enum NodeTypeEnum type;
    enum BoolOpEnum op;
    struct ASTNodeStruct* left;
    struct ASTNodeStruct* right;
    int data_type;
} ASTBool;

typedef struct ASTRelStruct {
    enum NodeTypeEnum type;
    enum RelOpEnum op;
    struct ASTNodeStruct* left;
    struct ASTNodeStruct* right;
    int data_type;
} ASTRel;

typedef struct ASTEquStruct {
    enum NodeTypeEnum type;
    enum EquOpEnum op;
    struct ASTNodeStruct* left;
    struct ASTNodeStruct* right;
    int data_type;
} ASTEqu;

typedef struct ASTRefStruct {
    enum NodeTypeEnum type;
    StorageNode* entry;
    int ref;
} ASTRef;

typedef struct ASTFuncDeclarationsStruct {
    enum NodeTypeEnum type;
    struct ASTNodeStruct** func_declarations;
    int func_declaration_count;
} ASTFuncDeclarations;

typedef struct ASTFuncDeclStruct {
    enum NodeTypeEnum type;
    int ret_type;
    int pointer;
    StorageNode* entry;
    struct ASTNodeStruct* declarations;
    struct ASTNodeStruct* statements;
    struct ASTNodeStruct* return_node;
} ASTFuncDecl;

typedef struct ASTReturnTypeStruct {
    enum NodeTypeEnum type;
    int ret_type;
    int pointer;
} ASTReturnType;

typedef struct ASTDeclArgsStruct{
    enum NodeTypeEnum type;
    Argument* args;
    int arg_count;
} ASTDeclArgs;

typedef struct ASTReturnStruct {
    enum NodeTypeEnum type;
    int ret_type;
    struct ASTNodeStruct* ret_val;
} ASTReturn;

ASTNode* newASTNode(NodeType, ASTNode*, ASTNode*);
ASTNode* newASTDeclarationsNode(ASTNode**, int, ASTNode*);
ASTNode* newASTDeclNode(int, StorageNode**, int);
ASTNode* newASTConstNode(int, ValueType);
ASTNode* newStatementsNode(ASTNode**, int, ASTNode*);
ASTNode* newASTIfNode(ASTNode*, ASTNode*, ASTNode**, int, ASTNode*);
ASTNode* newASTElseIfNode(ASTNode*, ASTNode*);
ASTNode* newASTForNode(ASTNode*, ASTNode*, ASTNode*, ASTNode*);
void setLoopCounter(ASTNode*);
ASTNode* newASTWhileNode(ASTNode*, ASTNode*);
ASTNode* newASTAssignNode(StorageNode*, int, ASTNode*);
ASTNode* newASTArithAssignNode(StorageNode*, int, ASTNode*, ASTNode*, ASTNode*, ArithAssign);
ASTNode* newASTSimpleNode(int);
ASTNode* newASTIncrNode(StorageNode*, int, int);
ASTNode* newASTFuncCallNode(StorageNode*, ASTNode**, int);
ASTNode* newASTCallArgsNode(ASTNode**, int, ASTNode*);
ASTNode* newASTArithNode(enum ArithOpEnum, ASTNode*, ASTNode*);
ASTNode* newASTBoolNode(enum BoolOpEnum, ASTNode*, ASTNode*);
ASTNode* newASTRelNode(enum RelOpEnum, ASTNode*, ASTNode*);
ASTNode* newASTEquNode(enum EquOpEnum, ASTNode*, ASTNode*);
ASTNode* newASTRefNode(StorageNode*, int);
ASTNode* newASTFuncDeclarationsNode(ASTNode**, int, ASTNode*);
ASTNode* newASTFuncDeclNode(int, int, StorageNode*);
ASTNode* newASTReturnTypeNode(int, int);
ASTNode* newASTDeclArgsNode(Argument*, int, Argument);
ASTNode* newASTReturnNode(int, ASTNode*);
int getExpressionType(ASTNode*);
void printASTNode(ASTNode*);
void traverseAST(ASTNode*);

#endif