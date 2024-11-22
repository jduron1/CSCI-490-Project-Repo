#ifndef AST_H
#define AST_H

#include "value_type.h"

typedef struct ArgumentStruct Argument;
typedef struct StorageNodeStruct StorageNode;

typedef enum NodeTypeEnum {
    BASIC_NODE,
    DECLS_NODE,
    DECL_NODE,
    CONST_NODE,
    STMTS_NODE,
    IF_NODE,
    ELSE_IF_NODE,
    FOR_NODE,
    WHILE_NODE,
    ASSIGN_NODE,
    ARITH_ASSIGN_NODE,
    SIMPLE_NODE,
    INCR_NODE,
    FUNC_CALL_NODE,
    CALL_ARGS_NODE,
    ARITH_NODE,
    BOOL_NODE,
    REL_NODE,
    EQU_NODE,
    REF_NODE,
    FUNC_DECLS_NODE,
    FUNC_DECL_NODE,
    RET_TYPE_NODE,
    DECL_ARGS_NODE,
    RETURN_NODE,
    PAREN_NODE
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
    NodeType type;
    struct ASTNodeStruct *left;
    struct ASTNodeStruct *right;
} ASTNode;

typedef struct ASTDeclsStruct {
    NodeType type;
    ASTNode **declarations;
    int declaration_count;
} ASTDecls;

typedef struct ASTDeclStruct {
    NodeType type;
    int data_type;
    StorageNode **entries;
    int names_count;
} ASTDecl;

typedef struct ASTConstStruct {
    NodeType type;
    int const_type;
    ValueType value;
} ASTConst;

typedef struct ASTStmtsStruct {
    NodeType type;
    ASTNode **statements;
    int statement_count;
} ASTStmts;

typedef struct ASTIfStruct {
    NodeType type;
    ASTNode *condition;
    ASTNode *if_branch;
    ASTNode **else_if_branches;
    int else_if_count;
    ASTNode *else_branch;
} ASTIf;

typedef struct ASTElseIfStruct {
    NodeType type;
    ASTNode *condition;
    ASTNode *else_if_branch;
} ASTElseIf;

typedef struct ASTForStruct {
    NodeType type;
    ASTNode *init;
    ASTNode *condition;
    ASTNode *increment;
    ASTNode *for_branch;
    StorageNode *iterator;
} ASTFor;

typedef struct ASTWhileStruct {
    NodeType type;
    ASTNode *condition;
    ASTNode *while_branch;
} ASTWhile;

typedef struct ASTAssignStruct {
    NodeType type;
    StorageNode *entry;
    int ref;
    ASTNode *value;
} ASTAssign;

typedef struct ASTArithAssignStruct {
    NodeType type;
    StorageNode *entry;
    int ref;
    ASTNode *value;
    ArithAssign op;
    int data_type;
} ASTArithAssign;

typedef struct ASTSimpleStruct {
    NodeType node_type;
    int statement_type;
} ASTSimple;

typedef struct ASTIncrStruct {
    NodeType type;
    StorageNode *entry;
    int incr_type;
    int fix;
} ASTIncr;

typedef struct ASTFuncCallStruct {
    NodeType node_type;
    StorageNode *entry;
    ASTNode **args;
    int arg_count;
} ASTFuncCall;

typedef struct ASTCallArgsStruct {
    NodeType type;
    ASTNode **args;
    int arg_count;
} ASTCallArgs;

typedef struct ASTArithStruct {
    NodeType type;
    ArithOp op;
    ASTNode *left;
    ASTNode *right;
    int data_type;
} ASTArith;

typedef struct ASTBoolStruct {
    NodeType type;
    BoolOp op;
    ASTNode *left;
    ASTNode *right;
    int data_type;
} ASTBool;

typedef struct ASTRelStruct {
    NodeType type;
    RelOp op;
    ASTNode *left;
    ASTNode *right;
    int data_type;
} ASTRel;

typedef struct ASTEquStruct {
    NodeType type;
    EquOp op;
    ASTNode *left;
    ASTNode *right;
    int data_type;
} ASTEqu;

typedef struct ASTRefStruct {
    NodeType node_type;
    StorageNode *entry;
    int ref;
} ASTRef;

typedef struct ASTFuncDeclsStruct {
    NodeType type;
    ASTNode **func_declarations;
    int func_declaration_count;
} ASTFuncDecls;

typedef struct ASTFuncDeclStruct {
    NodeType type;
    StorageNode *entry;
    ASTNode *declarations;
    ASTNode *statements;
    ASTNode *ret;
    int ret_type;
    int pointer;
} ASTFuncDecl;

typedef struct ASTReturnTypeStruct {
    NodeType node_type;
    int ret_type;
    int pointer;
} ASTReturnType;

typedef struct ASTDeclArgsStruct {
    NodeType type;
    Argument *args;
    int arg_count;
} ASTDeclArgs;

typedef struct ASTReturnStruct {
    NodeType type;
    ASTNode *value;
    int ret_type;
} ASTReturn;

typedef struct ASTParenStruct {
    NodeType type;
    ASTNode *node;
} ASTParen;

ASTNode *newASTNode(NodeType, ASTNode *, ASTNode *);
ASTNode *newASTDeclsNode(ASTNode **, int, ASTNode *);
ASTNode *newASTDeclNode(int, StorageNode **, int);
ASTNode *newASTConstNode(int, ValueType);
ASTNode *newASTStmtsNode(ASTNode **, int, ASTNode *);
ASTNode *newASTIfNode(ASTNode *, ASTNode *, ASTNode **, int, ASTNode *);
ASTNode *newASTElseIfNode(ASTNode *, ASTNode *);
ASTNode *newASTForNode(ASTNode *, ASTNode *, ASTNode *, ASTNode *);
void setLoopCounter(ASTNode *);
ASTNode *newASTWhileNode(ASTNode *, ASTNode *);
ASTNode *newASTAssignNode(StorageNode *, int, ASTNode *);
ASTNode *newASTArithAssignNode(StorageNode *, int, ASTNode *, ASTNode *, ASTNode *, ArithAssign);
ASTNode *newASTSimpleNode(int);
ASTNode *newASTIncrNode(StorageNode *, int, int);
ASTNode *newASTFuncCallNode(StorageNode *, ASTNode **, int);
ASTNode *newASTCallArgsNode(ASTNode **, int, ASTNode *);
ASTNode *newASTArithNode(ArithOp, ASTNode *, ASTNode *);
ASTNode *newASTBoolNode(BoolOp, ASTNode *, ASTNode *);
ASTNode *newASTRelNode(RelOp, ASTNode *, ASTNode *);
ASTNode *newASTEquNode(EquOp, ASTNode *, ASTNode *);
ASTNode *newASTRefNode(StorageNode *, int);
ASTNode *newASTFuncDeclsNode(ASTNode **, int, ASTNode *);
ASTNode *newASTFuncDeclNode(int, int, StorageNode *);
ASTNode *newASTReturnTypeNode(int, int);
ASTNode *newASTDeclArgsNode(Argument *, int, Argument);
ASTNode *newASTReturnNode(int, ASTNode *);
ASTNode *newASTParenNode(ASTNode *);
int getExpressionType(ASTNode *);
void traverseAST(ASTNode *);

#endif // AST_H