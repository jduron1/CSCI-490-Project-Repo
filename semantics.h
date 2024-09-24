#ifndef SEMANTICS_H
#define SEMANTICS_H

#define UNDEF 0
#define INT_TYPE 1
#define REAL_TYPE 2
#define CHAR_TYPE 3
#define STR_TYPE 4
#define BOOL_TYPE 5
#define ARR_TYPE 6
#define PTR_TYPE 7
#define FUNC_TYPE 8
#define VOID_TYPE 9

#define NONE 0
#define ARITH_OP 1
#define BOOL_OP 2
#define NOT_OP 3
#define REL_OP 4
#define EQ_OP 5

int getResultType(int type_1, int type_2, int op);
void typeError(int type_1, int type_2, int op);

#endif