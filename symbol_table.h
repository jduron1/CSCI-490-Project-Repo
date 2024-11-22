#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define SIZE 211
#define MAX_TOKEN_LEN 40

#include <stdio.h>
#include "value_type.h"

typedef struct ASTNodeStruct ASTNode;

typedef enum PassTypeEnum {
    BY_VALUE,
    BY_REFER
} PassType;

typedef enum CheckTypeEnum {
    ARG_CHECK,
    ASSIGN_CHECK
} CheckType;

typedef struct ArgumentStruct {
    int arg_type;
    int storage_type;
    char arg_name[MAX_TOKEN_LEN];
    ValueType val;
    int pass;
} Argument;

typedef struct ReferencedStruct { 
    int line_no;
    struct ReferencedStruct *next;
} Referenced;

typedef struct StorageNodeStruct {
    char storage_name[MAX_TOKEN_LEN];
    int storage_size;
    int scope;
    Referenced *lines;
    ValueType val;
    int storage_type;
    int inferred_type;
    ValueType *vals;
    ASTNode *assigned;
    ASTNode *array_size;
    char **indices;
    int index_count;
    int cur_idx;
    Argument *args;
    int arg_count;
    struct StorageNodeStruct *next;
} StorageNode;

static StorageNode **table;

void initSymbolTable();
unsigned int hash(char * key);
void insert(char *, int, int, int);
StorageNode *lookup(char *);
void setDataType(char *, int, int);
int getDataType(char *);
Argument defineArg(int, int, char *, int);
void hideScope();
void incrScope();
void printSymbolTable(FILE *);

#endif // SYMBOL_TABLE_H