#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>

#define SIZE 211

#define MAX_TOKEN_LEN 40

#define BY_VALUE 1
#define BY_REFER 2

#define ARG_CHECK 1

typedef union ValueTypeUnion {
    int integer;
    double real;
    char character;
    char* string;
} ValueType;

typedef struct ArgumentStruct {
    ValueType val;
    int arg_type;
    char arg_name[MAX_TOKEN_LEN];
    int pass; 
} Argument;

typedef struct ReferencedStruct {
    struct ReferencedStruct* next;
    int line_no;
} Referenced;

typedef struct StorageNodeStruct {
    struct StorageNodeStruct* next;
    Referenced* lines;
    Argument* args;
    ValueType val;
    ValueType* vals;
    char storage_name[MAX_TOKEN_LEN];
    int storage_size;
    int scope;
    int storage_type;
    int inferred_type;
    int array_size;
    int arg_count;
} StorageNode;

typedef struct RevisitQueueStruct {
    struct RevisitQueueStruct* next;
    char* storage_name;
    int revisit_type;
} RevisitQueue;

typedef struct SymbolTableStruct {
    StorageNode** buckets;
    int size;
    int count;
} SymbolTable;

static SymbolTable* table;
static RevisitQueue* queue;

void initSymbolTable();
unsigned int hash(const char* key);
void insert(const char* name, int len, int type, int line_no);
StorageNode* lookup(const char* name);
void setType(const char* name, int storage_type, int inferred_type);
int getType(const char* name);
Argument defArg(int arg_type, const char* arg_name, int pass); 
int funcDeclaration(const char* name, int return_type, int arg_count, Argument* args); 
int funcArgCheck(const char* name, int arg_count, Argument* args); 
void hideScope();
void incrScope();
void pushToQueue(const char* name, int type); 
int revisit(const char* name);
void printSymbolTable(FILE* of);
void printRevisitQueue(FILE* of);

#endif