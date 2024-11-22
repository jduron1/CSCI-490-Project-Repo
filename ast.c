#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
#include "code_gen.h"

extern int yylineno;

ASTNode *newASTNode(NodeType type, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));

    node -> type = type;
    node -> left = left;
    node -> right = right;

    return node;
}

ASTNode *newASTDeclsNode(ASTNode **declarations, int declaration_count, ASTNode *declaration) {
    ASTDecls *node = (ASTDecls *)malloc(sizeof(ASTDecls));

    node -> type = DECLS_NODE;

    if (declarations == NULL) {
        declarations = (ASTNode **)malloc(sizeof(ASTNode *));
        declarations[0] = declaration;
        declaration_count = 1;
    } else {
        declarations = (ASTNode **)realloc(declarations, (declaration_count + 1) * sizeof(ASTNode *));
        declarations[declaration_count] = declaration;
        declaration_count++;
    }

    node -> declarations = declarations;
    node -> declaration_count = declaration_count;

    return (ASTNode *)node;
}

ASTNode *newASTDeclNode(int data_type, StorageNode **entries, int names_count) {
    ASTDecl *node = (ASTDecl *)malloc(sizeof(ASTDecl));

    node -> type = DECL_NODE;
    node -> data_type = data_type;
    node -> entries = entries;
    node -> names_count = names_count;

    return (ASTNode *)node;
}

ASTNode *newASTConstNode(int const_type, ValueType value) {
    ASTConst *node = (ASTConst *)malloc(sizeof(ASTConst));

    node -> type = CONST_NODE;
    node -> const_type = const_type;
    node -> value = value;

    return (ASTNode *)node;
}

ASTNode *newASTStmtsNode(ASTNode **statements, int statement_count, ASTNode *statement) {
    ASTStmts *node = (ASTStmts *)malloc(sizeof(ASTStmts));

    node -> type = STMTS_NODE;

    if (statements == NULL) {
        statements = (ASTNode **)malloc(sizeof(ASTNode *));
        statements[0] = statement;
        statement_count = 1;
    } else {
        statements = (ASTNode **)realloc(statements, (statement_count + 1) * sizeof(ASTNode *));
        statements[statement_count] = statement;
        statement_count++;
    }

    node -> statements = statements;
    node -> statement_count = statement_count;

    return (ASTNode *)node;
}

ASTNode *newASTIfNode(ASTNode *condition, ASTNode *if_branch, ASTNode **else_if_branches, int else_if_count, ASTNode *else_branch) {
    ASTIf *node = (ASTIf *)malloc(sizeof(ASTIf));

    node -> type = IF_NODE;
    node -> condition = condition;
    node -> if_branch = if_branch;
    node -> else_if_branches = else_if_branches;
    node -> else_if_count = else_if_count;
    node -> else_branch = else_branch;

    return (ASTNode *)node;
}

ASTNode *newASTElseIfNode(ASTNode *condition, ASTNode *else_if_branch) {
    ASTElseIf *node = (ASTElseIf *)malloc(sizeof(ASTElseIf));

    node -> type = ELSE_IF_NODE;
    node -> condition = condition;
    node -> else_if_branch = else_if_branch;

    return (ASTNode *)node;
}

ASTNode *newASTForNode(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *for_branch) {
    ASTFor *node = (ASTFor *)malloc(sizeof(ASTFor));

    node -> type = FOR_NODE;
    node -> init = init;
    node -> condition = condition;
    node -> increment = increment;
    node -> for_branch = for_branch;

    return (ASTNode *)node;
}

void setLoopCounter(ASTNode *node) {
    ASTFor *for_node = (ASTFor *)node;
    ASTAssign *assign_node = (ASTAssign *)for_node -> init;
    ASTIncr *incr_node = (ASTIncr *)for_node -> increment;

    if (strncmp(incr_node -> entry -> storage_name, assign_node -> entry -> storage_name, strlen(incr_node -> entry -> storage_name)) != 0) {
        fprintf(stderr, "Loop counter must be the same as the loop increment.\n");
        exit(1);
    }

    for_node -> iterator = assign_node -> entry;
}

ASTNode *newASTWhileNode(ASTNode *condition, ASTNode *while_branch) {
    ASTWhile *node = (ASTWhile *)malloc(sizeof(ASTWhile));

    node -> type = WHILE_NODE;
    node -> condition = condition;
    node -> while_branch = while_branch;

    return (ASTNode *)node;
}

ASTNode *newASTAssignNode(StorageNode *entry, int ref, ASTNode *value) {
    ASTAssign *node = (ASTAssign *)malloc(sizeof(ASTAssign));

    node -> type = ASSIGN_NODE;
    node -> entry = entry;
    node -> ref = ref;
    node -> value = value;

    return (ASTNode *)node;
}

ASTNode *newASTArithAssignNode(StorageNode *entry, int ref, ASTNode *value, ASTNode *left, ASTNode *right, ArithAssign op) {
    ASTArithAssign *node = (ASTArithAssign *)malloc(sizeof(ASTArithAssign));

    node -> type = ARITH_ASSIGN_NODE;
    node -> entry = entry;
    node -> ref = ref;
    node -> value = value;
    node -> op = op;
    node -> data_type = getResultType(getExpressionType(left), getExpressionType(right), ARITH_ASSIGN_OP);

    return (ASTNode *)node;
}

ASTNode *newASTSimpleNode(int statement_type) {
    ASTSimple *node = (ASTSimple *)malloc(sizeof(ASTSimple));

    node -> node_type = SIMPLE_NODE;
    node -> statement_type = statement_type;

    return (ASTNode *)node;
}

ASTNode *newASTIncrNode(StorageNode *entry, int incr_type, int fix) {
    ASTIncr *node = (ASTIncr *)malloc(sizeof(ASTIncr));

    node -> type = INCR_NODE;
    node -> entry = entry;
    node -> incr_type = incr_type;
    node -> fix = fix;

    return (ASTNode *)node;
}

ASTNode *newASTFuncCallNode(StorageNode *entry, ASTNode **args, int arg_count) {
    ASTFuncCall *node = (ASTFuncCall *)malloc(sizeof(ASTFuncCall));

    node -> node_type = FUNC_CALL_NODE;
    node -> entry = entry;
    node -> args = args;
    node -> arg_count = arg_count;

    return (ASTNode *)node;
}

ASTNode *newASTCallArgsNode(ASTNode **args, int arg_count, ASTNode *arg) {
    ASTCallArgs *node = (ASTCallArgs *)malloc(sizeof(ASTCallArgs));

    node -> type = CALL_ARGS_NODE;

    if (args == NULL) {
        args = (ASTNode **)malloc(sizeof(ASTNode *));
        args[0] = arg;
        arg_count = 1;
    } else {
        args = (ASTNode **)realloc(args, (arg_count + 1) * sizeof(ASTNode *));
        args[arg_count] = arg;
        arg_count++;
    }

    node -> args = args;
    node -> arg_count = arg_count;

    return (ASTNode *)node;
}

ASTNode *newASTArithNode(ArithOp op, ASTNode *left, ASTNode *right) {
    ASTArith *node = (ASTArith *)malloc(sizeof(ASTArith));

    node -> type = ARITH_NODE;
    node -> op = op;
    node -> left = left;
    node -> right = right;
    node -> data_type = getResultType(getExpressionType(left), getExpressionType(right), ARITH_OP);

    return (ASTNode *)node;
}

ASTNode *newASTBoolNode(BoolOp op, ASTNode *left, ASTNode *right) {
    ASTBool *node = (ASTBool *)malloc(sizeof(ASTBool));

    node -> type = BOOL_NODE;
    node -> op = op;
    node -> left = left;
    node -> right = right;
    
    if (op != OP_NOT) {
        node -> data_type = getResultType(getExpressionType(left), getExpressionType(right), BOOL_OP);
    } else {
        node -> data_type = getResultType(getExpressionType(left), UNDEF, NOT_OP);
    }

    return (ASTNode *)node;
}

ASTNode *newASTRelNode(RelOp op, ASTNode *left, ASTNode *right) {
    ASTRel *node = (ASTRel *)malloc(sizeof(ASTRel));

    node -> type = REL_NODE;
    node -> op = op;
    node -> left = left;
    node -> right = right;
    node -> data_type = getResultType(getExpressionType(left), getExpressionType(right), REL_OP);

    return (ASTNode *)node;
}

ASTNode *newASTEquNode(EquOp op, ASTNode *left, ASTNode *right) {
    ASTEqu *node = (ASTEqu *)malloc(sizeof(ASTEqu));

    node -> type = EQU_NODE;
    node -> op = op;
    node -> left = left;
    node -> right = right;
    node -> data_type = getResultType(getExpressionType(left), getExpressionType(right), EQU_OP);

    return (ASTNode *)node;
}

ASTNode *newASTRefNode(StorageNode *entry, int ref) {
    ASTRef *node = (ASTRef *)malloc(sizeof(ASTRef));

    node -> node_type = REF_NODE;
    node -> entry = entry;
    node -> ref = ref;

    return (ASTNode *)node;
}

ASTNode *newASTFuncDeclsNode(ASTNode **func_declarations, int func_declaration_count, ASTNode *func_declaration) {
    ASTFuncDecls *node = (ASTFuncDecls *)malloc(sizeof(ASTFuncDecls));

    node -> type = FUNC_DECLS_NODE;

    if (func_declarations == NULL) {
        func_declarations = (ASTNode **)malloc(sizeof(ASTNode *));
        func_declarations[0] = func_declaration;
        func_declaration_count = 1;
    } else {
        func_declarations = (ASTNode **)realloc(func_declarations, (func_declaration_count + 1) * sizeof(ASTNode *));
        func_declarations[func_declaration_count] = func_declaration;
        func_declaration_count++;
    }

    node -> func_declarations = func_declarations;
    node -> func_declaration_count = func_declaration_count;

    return (ASTNode *)node;
}

ASTNode *newASTFuncDeclNode(int ret_type, int pointer, StorageNode *entry) {
    ASTFuncDecl *node = (ASTFuncDecl *)malloc(sizeof(ASTFuncDecl));

    node -> type = FUNC_DECL_NODE;
    node -> ret_type = ret_type;
    node -> pointer = pointer;
    node -> entry = entry;

    return (ASTNode *)node;
}

ASTNode *newASTReturnTypeNode(int ret_type, int pointer) {
    ASTReturnType *node = (ASTReturnType *)malloc(sizeof(ASTReturnType));

    node -> node_type = RET_TYPE_NODE;
    node -> ret_type = ret_type;
    node -> pointer = pointer;

    return (ASTNode *)node;
}

ASTNode *newASTDeclArgsNode(Argument *args, int arg_count, Argument arg) {
    ASTDeclArgs *node = (ASTDeclArgs *)malloc(sizeof(ASTDeclArgs));

    node -> type = DECL_ARGS_NODE;

    if (args == NULL) {
        args = (Argument *)malloc(sizeof(Argument));
        args[0] = arg;
        arg_count = 1;
    } else {
        args = (Argument *)realloc(args, (arg_count + 1) * sizeof(Argument));
        args[arg_count] = arg;
        arg_count++;
    }

    node -> args = args;
    node -> arg_count = arg_count;

    return (ASTNode *)node;
}

ASTNode *newASTReturnNode(int ret_type, ASTNode *value) {
    ASTReturn *node = (ASTReturn *)malloc(sizeof(ASTReturn));

    node -> type = RETURN_NODE;
    node -> value = value;
    node -> ret_type = ret_type;

    return (ASTNode *)node;
}

ASTNode *newASTParenNode(ASTNode *node) {
    ASTParen *paren = (ASTParen *)malloc(sizeof(ASTParen));

    paren -> type = PAREN_NODE;
    paren -> node = node;

    return (ASTNode *)paren;
}

int getExpressionType(ASTNode *node) {
    switch (node -> type) {
        case ARITH_NODE: {
            ASTArith *temp_arith = (ASTArith *)node;

            temp_arith -> data_type = getResultType(getExpressionType(temp_arith -> left), getExpressionType(temp_arith -> right), ARITH_OP);

            return temp_arith -> data_type;
        }

        case INCR_NODE: {
            ASTIncr *temp_incr = (ASTIncr *)node;
            return temp_incr -> entry -> storage_type;
        }

        case BOOL_NODE: {
            ASTBool *temp_bool = (ASTBool *)node;

            if (temp_bool -> op != NOT_OP) {
                temp_bool -> data_type = getResultType(getExpressionType(temp_bool -> left), getExpressionType(temp_bool -> right), BOOL_OP);
            } else {
                temp_bool -> data_type = getResultType(getExpressionType(temp_bool -> left), UNDEF, NOT_OP);
            }

            return temp_bool -> data_type;
        }

        case REL_NODE: {
            ASTRel *temp_rel = (ASTRel *)node;

            temp_rel -> data_type = getResultType(getExpressionType(temp_rel -> left), getExpressionType(temp_rel -> right), REL_OP);

            return temp_rel -> data_type;
        }

        case EQU_NODE: {
            ASTEqu *temp_equ = (ASTEqu *)node;

            temp_equ -> data_type = getResultType(getExpressionType(temp_equ -> left), getExpressionType(temp_equ -> right), EQU_OP);

            return temp_equ -> data_type;
        }

        case REF_NODE: {
            ASTRef *temp_ref = (ASTRef *)node;

            if (temp_ref -> entry -> storage_type == INT_TYPE || temp_ref -> entry -> storage_type == FLOAT_TYPE || temp_ref -> entry -> storage_type == CHAR_TYPE || temp_ref -> entry -> storage_type == STRING_TYPE || temp_ref -> entry -> storage_type == BOOL_TYPE) {
                return temp_ref -> entry -> storage_type;
            } else if (temp_ref -> entry -> storage_type == ARRAY_TYPE) {
                return temp_ref -> entry -> inferred_type;
            } else if (temp_ref -> entry -> storage_type == POINTER_TYPE) {
                return INT_TYPE;
            }
        }

        case CONST_NODE: {
            ASTConst *temp_const = (ASTConst *)node;
            return temp_const -> const_type;
        }

        case FUNC_CALL_NODE: {
            ASTFuncCall *temp_func_call = (ASTFuncCall *)node;

            if (temp_func_call -> entry -> storage_type == UNDEF && temp_func_call -> entry -> inferred_type == UNDEF) {
                return INT_TYPE;
            }

            return temp_func_call -> entry -> inferred_type;
        }

        case PAREN_NODE: {
            ASTParen *temp_paren = (ASTParen *)node;
            return getExpressionType(temp_paren -> node);
        }

        default:
            fprintf(stderr, "Error: Invalid expression type.\n");
            exit(1);
    }
}