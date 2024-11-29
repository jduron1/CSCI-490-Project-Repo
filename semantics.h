#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "symbol_table.h"

typedef enum DataTypeEnum {
    UNDEF,
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    BOOL_TYPE,
    ARRAY_TYPE,
    POINTER_TYPE,
    FUNCTION_TYPE,
    VOID_TYPE
} DataType;

typedef enum OperationEnum {
    NONE,
    ARITH_OP,
    ARITH_ASSIGN_OP,
    INCR_OP,
    BOOL_OP,
    NOT_OP,
    REL_OP,
    EQU_OP
} Operation;

typedef struct RevisitQueueStruct {
    StorageNode *entry;
    char *storage_name;
    int revisit_type;
    int **arg_types;
    int *arg_count;
    int call_count;
    void **nodes;
    int assign_count;
    struct RevisitQueueStruct *next;
} RevisitQueue;

static RevisitQueue *queue;

void pushToQueue(StorageNode *, char *, int);
RevisitQueue* searchQueue(const char *);
RevisitQueue* searchPrevQueue(const char *);
int revisit(const char *);
int funcDeclaration(const char *, int, int, Argument *);
int funcArgCheck(const char *, int, int **, int *);
void printRevisitQueue(FILE *);
int getResultType(int, int, int);
void typeError(int, int, int);

#endif // SEMANTICS_H