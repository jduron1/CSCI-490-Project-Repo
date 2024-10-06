#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define SIZE 211
#define MAX_TOKEN_LEN 40

#define BY_VALUE 1
#define BY_REFER 2

#define ARG_CHECK 1
#define ASSIGN_CHECK 2

typedef union ValueTypeUnion {
    long long int integer;
    double real;
    char character;
    char* string;
    char* boolean;
} ValueType;

typedef struct ArgumentStruct {
    int arg_type;
    int storage_type;
    char arg_name[MAX_TOKEN_LEN];
    ValueType val;
    int pass;
} Argument;

typedef struct ReferencedStruct { 
    int line_no;
    struct ReferencedStruct* next;
} Referenced;

typedef struct StorageNodeStruct {
    char storage_name[MAX_TOKEN_LEN];
    int storage_size;
    int scope;
    Referenced* lines;
    ValueType val;
    int storage_type;
    int inferred_type;
    ValueType* vals;
    char* array_size;
    char** indices;
    int index_count;
    int cur_idx;
    Argument* args;
    int arg_count;
    struct StorageNodeStruct* next;
} StorageNode;

static StorageNode** table;

void initSymbolTable();
unsigned int hash(const char* key);
void insert(const char*, int, int, int);
StorageNode* lookup(const char*);
void setDataType(const char*, int, int);
int getDataType(const char*);
Argument defArg(int, int, const char*, int);
void hideScope();
void incrScope();
void printSymbolTable(FILE*);

#endif