#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "ast.h"
#include "semantics.h"

extern int yylineno;

void pushToQueue(StorageNode* entry, char* name, int type) {
    RevisitQueue* q;

    if(queue == NULL) {
        q = (RevisitQueue*)malloc(sizeof(RevisitQueue));

        q -> entry = entry;
        q -> storage_name = name;
        q -> revisit_type = type;
        q -> next = NULL;

        if (type == ARG_CHECK) {
            q -> call_count = 0;
        } else if (type == ASSIGN_CHECK) {
            q -> assign_count = 0;
        }

        queue = q;
    } else {
        q = queue;

        while (q -> next != NULL) {
            q = q -> next;
        }

        q -> next = (RevisitQueue*)malloc(sizeof(RevisitQueue));

        q -> next -> entry = entry;
        q -> next -> storage_name = name;
        q -> next -> revisit_type = type;
        q -> next -> next = NULL;

        if (type == ARG_CHECK) {
            q -> next -> call_count = 0;
        } else if (type == ASSIGN_CHECK) {
            q -> next -> assign_count = 0;
        }
    }		
}

RevisitQueue* searchQueue(const char* name) {
    RevisitQueue* q = queue;

    while ((q != NULL) && (strcmp(q -> storage_name, name) != 0)) {
        q = q -> next;
    }

    return q;
}

RevisitQueue* searchPrevQueue(const char* name) {
    if (queue == NULL) {
        return NULL;
    }

    if (strcmp(queue -> storage_name, name) == 0) {
        return NULL;
    }

    RevisitQueue* q = queue;

    while ((q != NULL) && (strcmp(q -> storage_name, name) != 0)) {
        q = q -> next;
    }

    return q;
}

int revisit(const char* name) {
    RevisitQueue* q = searchQueue(name);

    if (q == NULL) {
        return -1;
    }

    switch (q -> revisit_type) {
        case ARG_CHECK: {
            if (q -> call_count == 0) {
                printf("Function %s defined at line %d.\n", name, q -> entry -> lines -> line_no);
            } else if (!funcArgCheck(name, q -> call_count, q -> arg_types, q -> arg_count)) {
                printf("Function %s called with correct number of arguments.\n", name);
            }

            RevisitQueue* prev = searchPrevQueue(name);

            if (prev == NULL) {
                queue = queue -> next;
            } else {
                RevisitQueue* temp = prev -> next;
                prev -> next = prev -> next -> next;
                free(temp);
            }

            break;
        }

        case ASSIGN_CHECK: {
            int type_1 = getDataType(q -> entry -> storage_name);

            for (int i = 0; i < q -> assign_count; i++) {
                int type_2 = getExpressionType(q -> nodes[i]);

                getResultType(type_1, type_2, NONE);
            }

            RevisitQueue* prev = searchPrevQueue(name);

            if (prev == NULL) {
                queue = queue -> next;
            } else {
                RevisitQueue* temp = prev -> next;
                prev -> next = prev -> next -> next;
                free(temp);
            }

            break;
        }

        default:
            break;
    }

    return 0;
}

int funcDeclaration(const char* name, int ret_type, int arg_count, Argument *args) {
    StorageNode* node = lookup(name);

    if (node != NULL) {
        printf("Function %s declared at line %d.\n", name, node -> lines -> line_no);

        if (node -> storage_type == UNDEF) {
            node -> storage_type = FUNCTION_TYPE;
            node -> inferred_type = ret_type;
            node -> arg_count = arg_count;
            node -> args = args;

            return 0;
        } else {
            fprintf(stderr, "Redeclaration of function %s at line %d.\n", name, node -> lines -> line_no);
            exit(1);
        }
    }
}

int funcArgCheck(const char* name, int call_count, int** arg_types, int* arg_count) {
    StorageNode* node = lookup(name);

    for (int i = 0; i < call_count; i++) {
        if (node -> arg_count != arg_count[i]) {
            fprintf(stderr, "Function %s called with incorrect number of arguments at line %d.\n", name, node -> lines -> line_no);
            exit(1);
        }

        for (int j = 0; j < arg_count[i]; j++) {
            int type_1 = node -> args[j].arg_type;
            int type_2 = arg_types[i][j];

            getResultType(type_1, type_2, NONE);
        }
    }

    return 0;
}

void printRevisitQueue(FILE* of) {
    RevisitQueue* q = queue;
    
    fprintf(of, "------------ -------------\n");
    fprintf(of, "Identifier   Revisit Type\n");
    fprintf(of, "------------ -------------\n");

    while (q != NULL) {
        fprintf(of, "%-13s", q -> storage_name);

        if (q -> revisit_type == ARG_CHECK) {
            fprintf(of, "%s", "Argument check ");
            fprintf(of, "for %d function calls.", q -> call_count);
        } else if (q -> revisit_type == ASSIGN_CHECK) {
            fprintf(of, "%s", "Assignment check ");
            fprintf(of, "for %d assignments.", q -> assign_count);
        }

        fprintf(of, "\n");
        q = q -> next;	
    }
}

int getResultType(int type_1, int type_2, int op) {
    switch (op) {
        case NONE:
            switch (type_1) {
                case INT_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return 1;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case REAL_TYPE:
                    if (type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE) {
                        return 1;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return 1;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                default:
                    break;
            }

            break;
        case ARITH_OP:
            switch (type_1) {
                case INT_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return INT_TYPE;
                    } else if (type_2 == REAL_TYPE) {
                        return REAL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case REAL_TYPE:
                    if (type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE) {
                        return REAL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return CHAR_TYPE;
                    } else if (type_2 == REAL_TYPE) {
                        return REAL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        case ARITH_ASSIGN_OP:
            switch (type_1) {
                case INT_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return INT_TYPE;
                    } else if (type_2 == REAL_TYPE) {
                        return REAL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case REAL_TYPE:
                    if (type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE) {
                        return REAL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return CHAR_TYPE;
                    } else if (type_2 == REAL_TYPE) {
                        return REAL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        case INCR_OP:
            switch (type_1) {
                case INT_TYPE:
                    return INT_TYPE;
                    break;
                case REAL_TYPE:
                    return REAL_TYPE;
                    break;
                case CHAR_TYPE:
                    return CHAR_TYPE;
                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        case BOOL_OP:
            switch (type_1) {
                case INT_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return INT_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return CHAR_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case BOOL_TYPE:
                    if (type_2 == BOOL_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }
                
                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        case NOT_OP:
            switch (type_1) {
                case INT_TYPE:
                    return INT_TYPE;
                    break;
                case CHAR_TYPE:
                    return INT_TYPE;
                    break;
                case BOOL_TYPE:
                    return BOOL_TYPE;
                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        case REL_OP:
            switch (type_1) {
                case INT_TYPE:
                    if (type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case REAL_TYPE:
                    if(type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE) {
                        return BOOL_TYPE;
                    } else{
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if(type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE) {
                        return BOOL_TYPE;
                    } else{
                        typeError(type_1, type_2, op);
                    }

                    break;
                case BOOL_TYPE:
                    if (type_2 == BOOL_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        case EQU_OP:
            switch (type_1) {
                case INT_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case REAL_TYPE:
                    if (type_2 == REAL_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if (type_2 == INT_TYPE || type_2 == CHAR_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                case BOOL_TYPE:
                    if (type_2 == BOOL_TYPE) {
                        return BOOL_TYPE;
                    } else {
                        typeError(type_1, type_2, op);
                    }

                    break;
                default:
                    typeError(type_1, type_2, op);
                    break;
            }

            break;
        default:
            fprintf(stderr, "Invalid operator at line %d.\n", yylineno);
            exit(1);
            break;
    }
}

void typeError(int type_1, int type_2, int op) {
    fprintf(stderr, "Type error between ");

    switch (type_1) {
        case INT_TYPE:
            fprintf(stderr, "int ");
            break;
        case REAL_TYPE:
            fprintf(stderr, "real ");
            break;
        case CHAR_TYPE:
            fprintf(stderr, "char ");
            break;
        case STRING_TYPE:
            fprintf(stderr, "string ");
            break;
        case BOOL_TYPE:
            fprintf(stderr, "bool ");
            break;
        case ARRAY_TYPE:
            fprintf(stderr, "array ");
            break;
        case POINTER_TYPE:
            fprintf(stderr, "pointer ");
            break;
        case FUNCTION_TYPE:
            fprintf(stderr, "function ");
            break;
        case VOID_TYPE:
            fprintf(stderr, "void ");
            break;
        default:
            fprintf(stderr, "undef ");
            break;
    }

    fprintf(stderr, "and ");

    switch (type_2) {
        case INT_TYPE:
            fprintf(stderr, "int ");
            break;
        case REAL_TYPE:
            fprintf(stderr, "real ");
            break;
        case CHAR_TYPE:
            fprintf(stderr, "char ");
            break;
        case STRING_TYPE:
            fprintf(stderr, "string ");
            break;
        case BOOL_TYPE:
            fprintf(stderr, "bool ");
            break;
        case ARRAY_TYPE:
            fprintf(stderr, "array ");
            break;
        case POINTER_TYPE:
            fprintf(stderr, "pointer ");
            break;
        case FUNCTION_TYPE:
            fprintf(stderr, "function ");
            break;
        case VOID_TYPE:
            fprintf(stderr, "void ");
            break;
        default:
            fprintf(stderr, "undef ");
            break;
    }

    fprintf(stderr, "with operator ");

    switch (op) {
        case NONE:
            fprintf(stderr, "NONE");
            break;
        case ARITH_OP:
            fprintf(stderr, "ARITH_OP");
            break;
        case ARITH_ASSIGN_OP:
            fprintf(stderr, "ARITH_ASSIGN_OP");
            break;
        case INCR_OP:
            fprintf(stderr, "INCR_OP");
            break;
        case BOOL_OP:
            fprintf(stderr, "BOOL_OP");
            break;
        case NOT_OP:
            fprintf(stderr, "NOT_OP");
            break;
        case REL_OP:
            fprintf(stderr, "REL_OP");
            break;
        case EQU_OP:
            fprintf(stderr, "EQU_OP");
            break;
        default:
            fprintf(stderr, "INVALID");
            exit(1);
            break;
    }

    fprintf(stderr, " in line %d.\n", yylineno);

    exit(1);
}