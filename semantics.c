#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"

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
                    typeError(type_1, type_2, op);
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
                    if(type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE){
                        return BOOL_TYPE;
                    } else{
                        typeError(type_1, type_2, op);
                    }

                    break;
                case CHAR_TYPE:
                    if(type_2 == INT_TYPE || type_2 == REAL_TYPE || type_2 == CHAR_TYPE){
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
        case EQ_OP:
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
            fprintf(stderr, "Invalid operator.\n");
            exit(1);
            break;
    }
}

void typeError(int type_1, int type_2, int op) {
    fprintf(stderr, "Type conflict between %d and %d in operation %d.\n", type_1, type_2, op);
    exit(1);
}