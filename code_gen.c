#include <stdlib.h>
#include <string.h>
#include "code_gen.h"
#include "semantics.h"

static int for_flag = 0;
static int assign_flag = 0;
static int out_flag = 0;
static int in_flag = 0;
static int func_level = 0;
static int cur_idx = 0;

void generateSimpleCode(FILE *of, ASTNode *node) {
    ASTSimple *simple_node = (ASTSimple *)node;

    switch (simple_node -> statement_type) {
        case 0:
            fprintf(of, "continue;\n");
            break;
        case 1:
            fprintf(of, "break;\n");
            break;
        default:
            break;
    }
}

void generateConstCode(FILE *of, ASTNode *node) {
    ASTConst *const_node = (ASTConst *)node;

    switch (const_node -> const_type) {
        case INT_TYPE:
            fprintf(of, "%lld", const_node -> value.integer);
            break;
        case FLOAT_TYPE:
            fprintf(of, "%f", const_node -> value.real);
            break;
        case CHAR_TYPE:
            fprintf(of, "%c", const_node -> value.character);
            break;
        case STRING_TYPE:
            if (strcmp(const_node -> value.string, "\"\\n\"") == 0) {
                fprintf(of, "\"\\n\"");
            } else {
                fprintf(of, "%s", const_node -> value.string);
            }
            break;
        case BOOL_TYPE:
            fprintf(of, "%s", const_node -> value.boolean ? "true" : "false");
            break;
        default:
            break;
    }
}

void generateRefCode(FILE *of, ASTNode *node) {
    ASTRef *ref_node = (ASTRef *)node;

    if (ref_node -> ref == 1) {
        fprintf(of, "&");
    } else if (ref_node -> entry -> storage_type == POINTER_TYPE) {
        fprintf(of, " *");
    }

    if (ref_node -> entry -> storage_type == ARRAY_TYPE && ref_node -> entry -> indices != NULL) {
        fprintf(of, "%s[%s]", ref_node -> entry -> storage_name, ref_node -> entry -> indices[ref_node -> entry -> cur_idx]);
        ref_node -> entry -> cur_idx++;
    } else {
        fprintf(of, "%s", ref_node -> entry -> storage_name);
    }
}

void generateArithCode(FILE *of, ASTNode *node) {
    ASTArith *arith_node = (ASTArith *)node;

    switch (arith_node -> op) {
        case OP_ADD:
            findNodeType(of, arith_node -> left);
            fprintf(of, " + ");
            findNodeType(of, arith_node -> right);
            break;
        case OP_SUB:
            findNodeType(of, arith_node -> left);
            fprintf(of, " - ");
            findNodeType(of, arith_node -> right);
            break;
        case OP_MUL:
            findNodeType(of, arith_node -> left);
            fprintf(of, "  *");
            findNodeType(of, arith_node -> right);
            break;
        case OP_DIV:
            findNodeType(of, arith_node -> left);
            fprintf(of, " / ");
            findNodeType(of, arith_node -> right);
            break;
        case OP_MOD:
            findNodeType(of, arith_node -> left);
            fprintf(of, " %% ");
            findNodeType(of, arith_node -> right);
            break;
        case OP_EXP:
            findNodeType(of, arith_node -> left);
            fprintf(of, " ^ ");
            findNodeType(of, arith_node -> right);
            break;
        default:
            break;
    }
}

void generateIncrCode(FILE *of, ASTNode *node) {
    ASTIncr *incr_node = (ASTIncr *)node;

    fprintf(of, "%s", incr_node -> entry -> storage_name);

    if (incr_node -> incr_type == 0) {
        fprintf(of, "--");
    } else {
        fprintf(of, "++");
    }
}

void generateBoolCode(FILE *of, ASTNode *node) {
    ASTBool *bool_node = (ASTBool *)node;

    if (bool_node -> op == OP_NOT) {
        fprintf(of, "!");
        findNodeType(of, bool_node -> left);
    } else {
        switch (bool_node -> op) {
            case OP_OR:
                findNodeType(of, bool_node -> left);
                fprintf(of, " || ");
                findNodeType(of, bool_node -> right);
                break;
            case OP_AND:
                findNodeType(of, bool_node -> left);
                fprintf(of, " && ");
                findNodeType(of, bool_node -> right);
                break;
            default:
                break;
        }
    }
}

void generateRelCode(FILE *of, ASTNode *node) {
    ASTRel *rel_node = (ASTRel *)node;

    switch (rel_node -> op) {
        case OP_GREAT:
            findNodeType(of, rel_node -> left);
            fprintf(of, " > ");
            findNodeType(of, rel_node -> right);
            break;
        case OP_LESS:
            findNodeType(of, rel_node -> left);
            fprintf(of, " < ");
            findNodeType(of, rel_node -> right);
            break;
        case OP_GE:
            findNodeType(of, rel_node -> left);
            fprintf(of, " >= ");
            findNodeType(of, rel_node -> right);
            break;
        case OP_LE:
            findNodeType(of, rel_node -> left);
            fprintf(of, " <= ");
            findNodeType(of, rel_node -> right);
            break;
        default:
            break;
    }
}

void generateEquCode(FILE *of, ASTNode *node) {
    ASTEqu *equ_node = (ASTEqu *)node;

    switch (equ_node -> op) {
        case OP_EQUAL:
            findNodeType(of, equ_node -> left);
            fprintf(of, " == ");
            findNodeType(of, equ_node -> right);
            break;
        case OP_NOT_EQUAL:
            findNodeType(of, equ_node -> left);
            fprintf(of, " != ");
            findNodeType(of, equ_node -> right);
            break;
        default:
            break;
    }
}

void generateDeclCode(FILE *of, ASTNode *node) {
    ASTDecl *decl_node = (ASTDecl *)node;

    switch (decl_node -> data_type) {
        case INT_TYPE:
            fprintf(of, "int ");
            break;
        case FLOAT_TYPE:
            fprintf(of, "double ");
            break;
        case CHAR_TYPE:
            fprintf(of, "char ");
            break;
        case STRING_TYPE:
            fprintf(of, "string ");
            break;
        case BOOL_TYPE:
            fprintf(of, "bool ");
            break;
        case VOID_TYPE:
            fprintf(of, "void ");
            break;
        default:
            break;
    }

    for (int i = 0; i < decl_node -> names_count; i++) {
        if (decl_node -> entries[i] -> storage_type == POINTER_TYPE) {
            fprintf(of, " *");
        }

        fprintf(of, "%s", decl_node -> entries[i] -> storage_name);

        if (decl_node -> entries[i] -> array_size != NULL && atoi(decl_node -> entries[i] -> array_size) > 0) {
            fprintf(of, "[] = {");

            for (int j = 0; j < atoi(decl_node -> entries[i] -> array_size); j++) {
                switch (decl_node -> data_type) {
                    case INT_TYPE:
                        fprintf(of, "%lld", decl_node -> entries[i] -> vals[j].integer);
                        break;
                    case FLOAT_TYPE:
                        fprintf(of, "%f", decl_node -> entries[i] -> vals[j].real);
                        break;
                    case CHAR_TYPE:
                        fprintf(of, "%c", decl_node -> entries[i] -> vals[j].character);
                        break;
                    case STRING_TYPE:
                        fprintf(of, "%s", decl_node -> entries[i] -> vals[j].string);
                        break;
                    case BOOL_TYPE:
                        fprintf(of, "%s", decl_node -> entries[i] -> vals[j].boolean ? "true" : "false");
                        break;
                    default:
                        break;
                }

                if (j < atoi(decl_node -> entries[i] -> array_size) - 1) {
                    fprintf(of, ", ");
                }
            }

            fprintf(of, "}");
        } else if (decl_node -> entries[i] -> storage_type == ARRAY_TYPE && decl_node -> entries[i] -> indices != NULL) {
            fprintf(of, "[%s]", decl_node -> entries[i] -> indices[decl_node -> entries[i] -> cur_idx]);
            decl_node -> entries[i] -> cur_idx++;
        }

        if (decl_node -> entries[i] -> assigned != NULL) {
            fprintf(of, " = ");
            findNodeType(of, decl_node -> entries[i] -> assigned);
        }

        if (i < decl_node -> names_count - 1) {
            fprintf(of, ", ");
        }
    }

    if (for_flag) {
        fprintf(of, "; ");
    } else {
        fprintf(of, ";\n");
    }
}

void generateStatementCode(FILE *of, ASTNode *node) {
    switch (node -> type) {
        case DECL_NODE:
            generateDeclCode(of, node);
            break;
        case ASSIGN_NODE:
            generateAssignCode(of, node);
            break;
        case ARITH_ASSIGN_NODE:
            generateArithAssignCode(of, node);
            break;
        case IF_NODE:
            generateIfCode(of, node);
            break;
        case FOR_NODE:
            generateForCode(of, node);
            break;
        case FOR_EACH_NODE:
            generateForEachCode(of, node);
            break;
        case WHILE_NODE:
            generateWhileCode(of, node);
            break;
        case SIMPLE_NODE:
            generateSimpleCode(of, node);
            break;
        case INCR_NODE:
            generateIncrCode(of, node);
            break;
        case FUNC_CALL_NODE:
            generateFuncCallCode(of, node);
            break;
        case RETURN_NODE:
            generateReturnCode(of, node);
            break;
        case STMTS_NODE: {
            ASTStmts *statements_node = (ASTStmts *)node;

            for (int i = 0; i < statements_node -> statement_count; i++) {
                generateStatementCode(of, statements_node -> statements[i]);
            }

            break;
        }
    }
}

void generateIfCode(FILE *of, ASTNode *node) {
    ASTIf *if_node = (ASTIf *)node;

    fprintf(of, "if (");
    
    switch (if_node -> condition -> type) {
        case BOOL_NODE:
            generateBoolCode(of, if_node -> condition);
            break;
        case REL_NODE:
            generateRelCode(of, if_node -> condition);
            break;
        case EQU_NODE:
            generateEquCode(of, if_node -> condition);
            break;
        default:
            break;
    }

    fprintf(of, ") {\n");

    generateStatementCode(of, if_node -> if_branch);

    fprintf(of, "}");

    if (if_node -> else_if_count > 0) {
        for (int i = 0; i < if_node -> else_if_count; i++) {
            ASTElseIf *elseif_node = (ASTElseIf *)if_node -> else_if_branches[i];

            fprintf(of, "else if (");
            generateBoolCode(of, elseif_node -> condition);
            fprintf(of, ") {\n");
            generateStatementCode(of, elseif_node -> else_if_branch);
            fprintf(of, "}\n");
        }
    }

    if (if_node -> else_branch != NULL) {
        fprintf(of, "else {\n");
        generateStatementCode(of, if_node -> else_branch);
        fprintf(of, "}\n");
    }
}

void generateForCode(FILE *of, ASTNode *node) {
    ASTFor *for_node = (ASTFor *)node;
    for_flag = 1;

    fprintf(of, "for (");

    if (for_node -> init != NULL) {
        switch (for_node -> init -> type) {
            case DECLS_NODE:
                generateDeclCode(of, for_node -> init);
                break;
            case ASSIGN_NODE:
                generateAssignCode(of, for_node -> init);
                break;
            default:
                break;
        }
    }

    if (for_node -> condition != NULL) {
        switch (for_node -> condition -> type) {
            case BOOL_NODE:
                generateBoolCode(of, for_node -> condition);
                break;
            case REL_NODE:
                generateRelCode(of, for_node -> condition);
                break;
            case EQU_NODE:
                generateEquCode(of, for_node -> condition);
                break;
            default:
                break;
        }
    }

    fprintf(of, "; ");

    if (for_node -> increment != NULL) {
        switch (for_node -> increment -> type) {
            case INCR_NODE:
                generateIncrCode(of, for_node -> increment);
                break;
            case ARITH_ASSIGN_NODE:
                break;
            default:
                break;
        }
    }

    fprintf(of, ") {\n");

    generateStatementCode(of, for_node -> for_branch);

    fprintf(of, "\n}\n");

    for_flag = 0;
}

void generateForEachCode(FILE *of, ASTNode *node) {
    ASTForEach *for_each_node = (ASTForEach *)node;

    fprintf(of, "for (");

    switch (for_each_node -> element -> storage_type) {
        case INT_TYPE:
            fprintf(of, "int ");
            break;
        case FLOAT_TYPE:
            fprintf(of, "double ");
            break;
        case CHAR_TYPE:
            fprintf(of, "char ");
            break;
        case STRING_TYPE:
            fprintf(of, "string ");
            break;
        case BOOL_TYPE:
            fprintf(of, "bool ");
            break;
        case VOID_TYPE:
            fprintf(of, "void ");
            break;
        default:
            break;
    }

    fprintf(of, "%s : %s) {\n", for_each_node -> element -> storage_name, for_each_node -> array -> storage_name);

    generateStatementCode(of, for_each_node -> for_branch);

    fprintf(of, "}\n");
}

void generateWhileCode(FILE *of, ASTNode *node) {
    ASTWhile *while_node = (ASTWhile *)node;

    fprintf(of, "while (");

    switch (while_node -> condition -> type) {
        case BOOL_NODE:
            generateBoolCode(of, while_node -> condition);
            break;
        case REL_NODE:
            generateRelCode(of, while_node -> condition);
            break;
        case EQU_NODE:
            generateEquCode(of, while_node -> condition);
            break;
        case CONST_NODE:
            generateConstCode(of, while_node -> condition);
            break;
        default:
            break;
    }

    fprintf(of, ") {\n");

    generateStatementCode(of, while_node -> while_branch);

    fprintf(of, "}\n");
}

void generateAssignCode(FILE *of, ASTNode *node) {
    ASTAssign *assign_node = (ASTAssign *)node;

    assign_flag = 1;

    if (assign_node -> entry -> storage_type == ARRAY_TYPE && assign_node -> entry -> indices != NULL) {
        fprintf(of, "%s[%s] = ", assign_node -> entry -> storage_name, assign_node -> entry -> indices[assign_node -> entry -> cur_idx]);
        assign_node -> entry -> cur_idx++;
    } else {
        if (assign_node -> ref == 1) {
            fprintf(of, "&");
        } else if (assign_node -> entry -> storage_type == POINTER_TYPE) {
            fprintf(of, " *");
        }

        fprintf(of, "%s = ", assign_node -> entry -> storage_name);
    }

    switch (assign_node -> value -> type) {
        case CONST_NODE:
            generateConstCode(of, assign_node -> value);
            break;
        case ARITH_NODE:
            generateArithCode(of, assign_node -> value);
            break;
        case BOOL_NODE:
            generateBoolCode(of, assign_node -> value);
            break;
        case REL_NODE:
            generateRelCode(of, assign_node -> value);
            break;
        case EQU_NODE:
            generateEquCode(of, assign_node -> value);
            break;
        case REF_NODE:
            generateRefCode(of, assign_node -> value);
            break;
        case FUNC_CALL_NODE:
            generateFuncCallCode(of, assign_node -> value);
            break;
        default:
            break;
    }

    if (for_flag) {
        fprintf(of, "; ");
    } else {
        fprintf(of, ";\n");
    }

    assign_flag = 0;
}

void generateArithAssignCode(FILE *of, ASTNode *node) {
    ASTArithAssign *arith_assign_node = (ASTArithAssign *)node;

    assign_flag = 1;

    if (arith_assign_node -> entry -> storage_type == ARRAY_TYPE && arith_assign_node -> entry -> indices != NULL) {
        fprintf(of, "%s[%s] ", arith_assign_node -> entry -> storage_name, arith_assign_node -> entry -> indices[arith_assign_node -> entry -> cur_idx]);
        arith_assign_node -> entry -> cur_idx++;
    } else {
        if (arith_assign_node -> ref == 1) {
            fprintf(of, "&");
        } else if (arith_assign_node -> entry -> storage_type == POINTER_TYPE) {
            fprintf(of, " *");
        }

        fprintf(of, "%s ", arith_assign_node -> entry -> storage_name);
    }

    switch (arith_assign_node -> op) {
        case OP_ADD_ASSIGN:
            fprintf(of, "+= ");
            break;
        case OP_SUB_ASSIGN:
            fprintf(of, "-= ");
            break;
        case OP_MUL_ASSIGN:
            fprintf(of, " *= ");
            break;
        case OP_DIV_ASSIGN:
            fprintf(of, "/= ");
            break;
        default:
            break;
    }

    findNodeType(of, arith_assign_node -> value);

    if (for_flag) {
        fprintf(of, "; ");
    } else {
        fprintf(of, ";\n");
    }

    assign_flag = 0;
}

void generateFuncDeclCode(FILE *of, ASTNode *node) {
    ASTFuncDecl *func_decl_node = (ASTFuncDecl *)node;

    switch (func_decl_node -> ret_type) {
        case INT_TYPE:
            fprintf(of, "int ");
            break;
        case FLOAT_TYPE:
            fprintf(of, "double ");
            break;
        case CHAR_TYPE:
            fprintf(of, "char ");
            break;
        case STRING_TYPE:
            fprintf(of, "string ");
            break;
        case BOOL_TYPE:
            fprintf(of, "bool ");
            break;
        case VOID_TYPE:
            fprintf(of, "void ");
            break;
        default:
            break;
    }

    if (strcmp(func_decl_node -> entry -> storage_name, "principal") == 0) {
        fprintf(of, "main(");
    } else {
        fprintf(of, "%s(", func_decl_node -> entry -> storage_name);
    }

    for (int i = 0; i < func_decl_node -> entry -> arg_count; i++) {
        switch (func_decl_node -> entry -> args[i].arg_type) {
            case INT_TYPE:
                fprintf(of, "int ");
                break;
            case FLOAT_TYPE:
                fprintf(of, "double ");
                break;
            case CHAR_TYPE:
                fprintf(of, "char ");
                break;
            case STRING_TYPE:
                fprintf(of, "string ");
                break;
            case BOOL_TYPE:
                fprintf(of, "bool ");
                break;
            case VOID_TYPE:
                fprintf(of, "void ");
                break;
            default:
                break;
        }

        if (func_decl_node -> entry -> args[i].pass == 1) {
            fprintf(of, "&");
        } else if (func_decl_node -> entry -> args[i].storage_type == POINTER_TYPE) {
            fprintf(of, " *");
        }

        fprintf(of, "%s", func_decl_node -> entry -> args[i].arg_name);

        if (func_decl_node -> entry -> args[i].storage_type == ARRAY_TYPE) {
            fprintf(of, "[]");
        }

        if (i < func_decl_node -> entry -> arg_count - 1) {
            fprintf(of, ", ");
        }
    }

    fprintf(of, ") {\n");

    // if (func_decl_node -> declarations != NULL) {
    //     ASTDecls *declarations_node = (ASTDecls *)func_decl_node -> declarations;

    //     for (int i = 0; i < declarations_node -> declaration_count; i++) {
    //         generateDeclCode(of, declarations_node -> declarations[i]);
    //     }
    // }

    if (func_decl_node -> statements != NULL) {
        ASTStmts *statements_node = (ASTStmts *)func_decl_node -> statements;

        for (int i = 0; i < statements_node -> statement_count; i++) {
            generateStatementCode(of, statements_node -> statements[i]);
        }
    }

    if (func_decl_node -> ret != NULL) {
        ASTReturn *return_node = (ASTReturn *)func_decl_node -> ret;

        fprintf(of, "return ");
        findNodeType(of, return_node -> value);
        fprintf(of, ";\n");
    }

    fprintf(of, "}\n");
}

void generateFuncCallCode(FILE *of, ASTNode *node) {
    ASTFuncCall *func_call_node = (ASTFuncCall *)node;

    if (strcmp(func_call_node -> entry -> storage_name, "principal") == 0) {
        fprintf(of, "main(");
    } else if (strcmp(func_call_node -> entry -> storage_name, "imprimir") == 0) {
        fprintf(of, "cout << ");
        out_flag = 1;
    } else if (strcmp(func_call_node -> entry -> storage_name, "leer") == 0) {
        fprintf(of, "cin >> ");
        in_flag = 1;
    } else {
        fprintf(of, "%s(", func_call_node -> entry -> storage_name);
    }

    for (int i = 0; i < func_call_node -> arg_count; i++) {
        findNodeType(of, func_call_node -> args[i]);

        if (i < func_call_node -> arg_count - 1 && out_flag) {
            fprintf(of, " << ");
        } else if (i < func_call_node -> arg_count - 1 && in_flag) {
            fprintf(of, " >> ");
        } else if (i < func_call_node -> arg_count - 1) {
            fprintf(of, ", ");
        }
    }

    if (func_level > 0 || assign_flag) {
        fprintf(of, ")");
        func_level--;
    } else if (in_flag || out_flag) {
        fprintf(of, ";\n");
        in_flag = 0;
        out_flag = 0;
    } else {
        fprintf(of, ");\n");
    }
}

void generateReturnCode(FILE *of, ASTNode *node) {
    ASTReturn *return_node = (ASTReturn *)node;

    fprintf(of, "return ");
    findNodeType(of, return_node -> value);
    fprintf(of, ";\n");
}

void generateParenCode(FILE *of, ASTNode *node) {
    ASTParen *paren_node = (ASTParen *)node;

    fprintf(of, "(");
    findNodeType(of, paren_node -> node);
    fprintf(of, ")");
}

void findNodeType(FILE *of, ASTNode *node) {
    switch (node -> type) {
        case ARITH_NODE:
            generateArithCode(of, node);
            break;
        case INCR_NODE:
            generateIncrCode(of, node);
            break;
        case BOOL_NODE:
            generateBoolCode(of, node);
            break;
        case REL_NODE:
            generateRelCode(of, node);
            break;
        case EQU_NODE:
            generateEquCode(of, node);
            break;
        case REF_NODE:
            generateRefCode(of, node);
            break;
        case CONST_NODE:
            generateConstCode(of, node);
            break;
        case FUNC_CALL_NODE:
            func_level++;
            generateFuncCallCode(of, node);
            break;
        case FUNC_DECLS_NODE:
            for (int i = 0; i < ((ASTFuncDecls *)node) -> func_declaration_count; i++) {
                generateFuncDeclCode(of, ((ASTFuncDecls *)node) -> func_declarations[i]);
            }
            break;
        case PAREN_NODE:
            generateParenCode(of, node);
            break;
        default:
            break;
    }
}