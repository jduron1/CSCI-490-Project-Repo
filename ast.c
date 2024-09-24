#include <stdlib.h>
#include "ast.h"

ASTNode* newASTNode(NodeType type, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));

    node -> type = type;
    node -> left = left;
    node -> right = right;

    return node;
}

ASTNode* newASTDeclNode(int type, StorageNode** names, int table_size) {
    ASTDecl* node = (ASTDecl*)malloc(sizeof(ASTDecl));

    node -> type = DECL_NODE;
    node -> decl_type = type;
    node -> names = names;
    node -> num_names = table_size;

    return (ASTNode*)node;
}

ASTNode* newASTConstNode(int type, ValueType var) {
    ASTConst* node = (ASTConst*)malloc(sizeof(ASTConst));

    node -> type = CONST_NODE;
    node -> const_type = type;
    node -> var = var;

    return (ASTNode*)node;
}

ASTNode* newASTIfNode(ASTNode* condition, ASTNode* if_branch, ASTNode* else_branch, ASTNode** elseif_branch, int num_elseif) {
    ASTIf* node = (ASTIf*)malloc(sizeof(ASTIf));

    node -> type = IF_NODE;
    node -> condition = condition;
    node -> if_branch = if_branch;
    node -> else_branch = else_branch;
    node -> elseif_branch = elseif_branch;
    node -> num_elseif = num_elseif;

    return (ASTNode*)node;
}

ASTNode* newASTElseIfNode(ASTNode* condition, ASTNode* elseif_branch) {
    ASTElseIf* node = (ASTElseIf*)malloc(sizeof(ASTElseIf));

    node -> type = ELSEIF_NODE;
    node -> condition = condition;
    node -> elseif_branch = elseif_branch;

    return (ASTNode*)node;
}

ASTNode* newASTForNode(ASTNode* initialize, ASTNode* condition, ASTNode* increment, ASTNode* for_branch) {
    ASTFor* node = (ASTFor*)malloc(sizeof(ASTFor));

    node -> type = FOR_NODE;
    node -> initialize = initialize;
    node -> condition = condition;
    node -> increment = increment;
    node -> for_branch = for_branch;

    return (ASTNode*)node;
}

ASTNode* newASTWhileNode(ASTNode* condition, ASTNode* while_branch) {
    ASTWhile* node = (ASTWhile*)malloc(sizeof(ASTWhile));

    node -> type = WHILE_NODE;
    node -> condition = condition;
    node -> while_branch = while_branch;

    return (ASTNode*)node;
}

ASTNode* newASTAssignNode(StorageNode* entry, ASTNode* assign_val) {
    ASTAssign* node = (ASTAssign*)malloc(sizeof(ASTAssign));

    node -> type = ASSIGN_NODE;
    node -> entry = entry;
    node -> assign_val = assign_val;

    return (ASTNode*)node;
}

ASTNode* newASTOpAssignNode(StorageNode* entry, ArithOp op, ASTNode* assign_val) {
    ASTOpAssign* node = (ASTOpAssign*)malloc(sizeof(ASTOpAssign));

    node -> type = ASSIGN_OP_NODE;
    node -> entry = entry;
    node -> op = op;
    node -> assign_val = assign_val;

    return (ASTNode*)node;
}

ASTNode* newASTSimpleNode(int type) {
    ASTSimple* node = (ASTSimple*)malloc(sizeof(ASTSimple));

    node -> type = SIMPLE_NODE;
    node -> statement_type = type;

    return (ASTNode*)node;
}

ASTNode* newASTFuncCallNode(StorageNode* entry, ASTNode** args, int num_args) {
    ASTFuncCall* node = (ASTFuncCall*)malloc(sizeof(ASTFuncCall));

    node -> type = FUNC_CALL;
    node -> entry = entry;
    node -> args = args;
    node -> num_args = num_args;

    return (ASTNode*)node;
}

void printAST(ASTNode* node) {
    ASTDecl* decl_node;
    ASTConst* const_node;
    ASTIf* if_node;
    ASTElseIf* elseif_node;
    ASTFor* for_node;
    ASTWhile* while_node;
    ASTAssign* assign_node;
    ASTSimple* simple_node;
    ASTFuncCall* call_node;
    ASTArith* arith_node;
    ASTBool* bool_node;
    ASTRel* rel_node;
    ASTEq* eq_node;
    ASTFuncDecl* func_decl_node;
    ASTReturn* return_node;

    switch(node->type){
        case BASIC_NODE:
            printf("Basic Node\n");
            break;
        case DECL_NODE:
            decl_node = (ASTDecl*)node;
            printf("Declaration Node of declaration type %d for %d names.\n",
                decl_node -> decl_type, decl_node -> num_names);
            break;
        case CONST_NODE:
            const_node = (ASTConst*)node;
            printf("Constant Node of constant type %d.\n", const_node -> const_type);
            break;
        case IF_NODE:
            if_node = (ASTIf*)node;
            printf("If Node with %d else-ifs\n", if_node -> num_elseif);
            break;
        case ELSEIF_NODE:
            printf("Else-if Node\n");
            break;
        case FOR_NODE:
            printf("For Node\n");
            break;
        case WHILE_NODE:
            printf("While Node\n");
            break;
        case ASSIGN_NODE:
            assign_node = (ASTAssign*)node;
            printf("Assign Node of entry %s.\n", assign_node -> entry -> storage_name);
            break;
        case SIMPLE_NODE:
            simple_node = (ASTSimple*)node;
            printf("Simple Node of statement %d.\n", simple_node -> statement_type);
            break;
        case FUNC_CALL:
            call_node = (ASTFuncCall*)node;
            printf("Function Call Node with %d parameters\n", call_node -> num_args);
            break;
        case ARITH_NODE:
            arith_node = (ASTArith*)node;
            printf("Arithmetic Node of operator %d.\n", arith_node -> op);
            break;
        case BOOL_NODE:
            bool_node = (ASTBool*)node;
            printf("Boolean Node of operator %d\n", bool_node -> op);
            break;
        case REL_NODE:
            rel_node = (ASTRel*)node;
            printf("Relational Node of operator %d.\n", rel_node -> op);
            break;
        case EQ_NODE:
            eq_node = (ASTEq*)node;
            printf("Equality Node of operator %d\n", eq_node -> op);
            break;
        case FUNC_DECL:
            func_decl_node = (ASTFuncDecl*)node;
            printf("Function Declaration Node of %s with return type %d.\n", func_decl_node -> entry -> storage_name, func_decl_node -> ret_type);
            break;
        case RETURN_NODE:
            return_node = (ASTReturn*)node;
            printf("Return Node of ret_type %d\n", return_node -> ret_type);
            break;
        default:
            fprintf(stderr, "Error in node selection.\n");
            exit(1);
            break;
    }
}

void traverseAST(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    ASTIf* if_node;
    ASTElseIf* elseif_node;
    ASTFor* for_node;
    ASTWhile* while_node;
    ASTAssign* assign_node;
    ASTFuncCall* call_node;
    ASTReturn* return_node;

    switch (node -> type) {
        case BASIC_NODE:
            traverseAST(node -> left);
            traverseAST(node -> right);
            printAST(node);
            break;
        case ARITH_NODE:
            traverseAST(node -> left);
            traverseAST(node -> right);
            printAST(node);
            break;
        case BOOL_NODE:
            traverseAST(node -> left);
            traverseAST(node -> right);
            printAST(node);
            break;
        case REL_NODE:
            traverseAST(node -> left);
            traverseAST(node -> right);
            printAST(node);
            break;
        case EQ_NODE:
            traverseAST(node -> left);
            traverseAST(node -> right);
            printAST(node);
            break;
        case IF_NODE:
            if_node = (ASTIf*)node;

            traverseAST(if_node -> condition);
            traverseAST(if_node -> if_branch);

            for (int i = 0; i < if_node -> num_elseif; i++) {
                traverseAST(if_node -> elseif_branch[i]);
            }

            traverseAST(if_node -> else_branch);
            printAST(node);
            break;
        case ELSEIF_NODE:
            elseif_node = (ASTElseIf*)node;

            traverseAST(elseif_node -> condition);
            traverseAST(elseif_node -> elseif_branch);
            printAST(node);
            break;
        case FOR_NODE:
            for_node = (ASTFor*)node;
            
            traverseAST(for_node -> condition);
            traverseAST(for_node -> for_branch);
            printAST(node);
            break;
        case WHILE_NODE:
            while_node = (ASTWhile*)node;

            traverseAST(while_node -> condition);
            traverseAST(while_node -> while_branch);
            printAST(node);
            break;
        case ASSIGN_NODE:
            assign_node = (ASTAssign*)node;

            traverseAST(assign_node -> assign_val);
            printAST(node);
            break;
        case FUNC_CALL:
            call_node = (ASTFuncCall*)node;

            for (int i = 0; i < call_node -> num_args; i++) {
                traverseAST(call_node -> args[i]);
            }

            printAST(node);
            break;
        case RETURN_NODE:
            return_node = (ASTReturn*)node;

            traverseAST(return_node -> ret_val);
            printAST(node);
            break;
        default:
            printAST(node);
            break;
    }
}