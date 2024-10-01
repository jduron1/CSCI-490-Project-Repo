#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"

extern int yylineno;

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
    // fprintf(stderr, "Type conflict between %d and %d in operation %d at line %d.\n", type_1, type_2, op, yylineno);
    // exit(1);

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