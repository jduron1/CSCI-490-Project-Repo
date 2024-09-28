#define UNDEF 0
#define INT_TYPE 1
#define REAL_TYPE 2
#define CHAR_TYPE 3
#define STRING_TYPE 4
#define BOOL_TYPE 5
#define ARRAY_TYPE 6
#define POINTER_TYPE 7
#define FUNCTION_TYPE 8
#define VOID_TYPE 9

#define NONE 0
#define ARITH_OP 1
#define ARITH_ASSIGN_OP 2
#define INCR_OP 3
#define BOOL_OP 4
#define NOT_OP 5
#define REL_OP 6
#define EQU_OP 7

int getResultType (int, int, int);
void typeError(int, int, int);