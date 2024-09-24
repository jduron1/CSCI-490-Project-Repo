#ifndef AST_H
#define AST_H

#include "symbol_table.h"

typedef enum NodeTypeEnum {
    BASIC_NODE,
    DECL_NODE,
    CONST_NODE,
    IF_NODE,
    ELSEIF_NODE,
    FOR_NODE,
    WHILE_NODE,
    ASSIGN_NODE,
    ASSIGN_OP_NODE,
    SIMPLE_NODE,
    FUNC_CALL,
    ARITH_NODE,
    BOOL_NODE,
    REL_NODE,
    EQ_NODE,
    FUNC_DECL,
    RETURN_NODE
} NodeType;

typedef enum ArithOpEnum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EXP
} ArithOp;

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
    EQUAL,
    NOT_EQUAL
} EquOp;

typedef struct ASTNodeStruct {
    NodeType type;
    struct ASTNodeStruct* left;
    struct ASTNodeStruct* right;
} ASTNode;

typedef struct ASTDeclStruct {
    NodeType type;
    int decl_type;
    StorageNode** names;
    int num_names;
} ASTDecl;

typedef struct ASTConstStruct {
    NodeType type;
    int const_type;
    ValueType var;
} ASTConst;

typedef struct ASTIfStruct {
    NodeType type;
    ASTNode* condition;
    ASTNode* if_branch;
    ASTNode** elseif_branch;
    ASTNode* else_branch;
    int num_elseif;
} ASTIf;

typedef struct ASTElseIfStruct {
    NodeType type;
    ASTNode* condition;
    ASTNode* elseif_branch;
} ASTElseIf;

typedef struct ASTForStruct {
    NodeType type;
    ASTNode* initialize;
    ASTNode* condition;
    ASTNode* increment;
    ASTNode* for_branch;
} ASTFor;

typedef struct ASTWhileStruct {
    NodeType type;
    ASTNode* condition;
    ASTNode* while_branch;
} ASTWhile;

typedef struct ASTAssignStruct {
    NodeType type;
    StorageNode* entry;
    ASTNode* assign_val;
} ASTAssign;

typedef struct ASTOpAssignStruct {
    NodeType type;
    StorageNode* entry;
    ArithOp op;
    ASTNode* assign_val;
} ASTOpAssign;

typedef struct ASTSimpleStruct {
    NodeType type;
    int statement_type;
} ASTSimple;

typedef struct ASTFuncCallStruct {
    NodeType type;
    StorageNode* entry;
    ASTNode** args;
    int num_args;
} ASTFuncCall;

typedef struct ASTArithStruct {
    NodeType type;
    ArithOp op;
    ASTNode* left;
    ASTNode* right;
} ASTArith;

typedef struct ASTBoolStruct {
    NodeType type;
    BoolOp op;
    ASTNode* left;
    ASTNode* right;
} ASTBool;

typedef struct ASTRelStruct {
    NodeType type;
    RelOp op;
    ASTNode* left;
    ASTNode* right;
} ASTRel;

typedef struct ASTEqStruct {
    NodeType type;
    EquOp op;
    ASTNode* left;
    ASTNode* right;
} ASTEq;

typedef struct ASTFuncDeclStruct {
    NodeType type;
    int ret_type;
    StorageNode* entry;
} ASTFuncDecl;

typedef struct ASTReturnStruct {
    NodeType type;
    int ret_type;
    ASTNode* ret_val;
} ASTReturn;

ASTNode* newASTNode(NodeType, ASTNode*, ASTNode*);
ASTNode* newASTDeclNode(int, StorageNode**, int table_size);
ASTNode* newASTConstNode(int, ValueType);
ASTNode* newASTIfNode(ASTNode*, ASTNode*, ASTNode*, ASTNode**, int);
ASTNode* newASTElseIfNode(ASTNode*, ASTNode*);
ASTNode* newASTForNode(ASTNode*, ASTNode*, ASTNode*, ASTNode*);
ASTNode* newASTWhileNode(ASTNode*, ASTNode*);
ASTNode* newASTAssignNode(StorageNode*, ASTNode*);
ASTNode* newASTOpAssignNode(StorageNode*, ArithOp, ASTNode*);
ASTNode* newASTSimpleNode(int);
ASTNode* newASTFuncCallNode(StorageNode*, ASTNode**, int);
void printAST(ASTNode*);
void traverseAST(ASTNode*);

#endif