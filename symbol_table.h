#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include "value_type.h"

#define SIZE 211
#define MAX_TOKEN_LEN 40

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
    char *array_size; // TODO: change to ASTNode
    char **indices; // TODO: change to array of ASTNode
    int index_count;
    int cur_idx;
    Argument *args;
    int arg_count;
    struct StorageNodeStruct *next;
} StorageNode;

static StorageNode **table;
static int cur_scope = 0;
static int declared = 0;
static int function_declared = 0;

void initSymbolTable();
unsigned int hash(const char *);
void insert(const char *, int, int, int);
StorageNode* lookup(const char *);
void setDataType(const char *, int, int);
int getDataType(const char *);
Argument defineArg(int, int, const char *, int);
void hideScope();
void incrScope();
void printSymbolTable(FILE *);

#endif // SYMBOL_TABLE_H