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
    int array_size;
    Argument* args;
    int arg_count;
    struct StorageNodeStruct* next;
} StorageNode;

typedef struct RevisitQueueStruct {
    StorageNode* entry;
    char* storage_name;
    int revisit_type;
    int** arg_types;
    int* arg_count;
    int call_count;
    void** nodes;
    int assign_count;
    struct RevisitQueueStruct* next;
} RevisitQueue;

static StorageNode** table;
static RevisitQueue* queue;

void initSymbolTable();
unsigned int hash(const char* key);
void insert(const char*, int, int, int);
StorageNode* lookup(const char*);
void setDataType(const char*, int, int);
int getDataType(const char*);
Argument defArg(int, const char*, int);
int funcDeclaration(const char*, int, int, Argument*);
int funcArgCheck(const char*, int, int**, int*);
void hideScope();
void incrScope();
void pushToQueue(StorageNode*, char*, int);
RevisitQueue* searchQueue(const char*);
RevisitQueue* searchPrevQueue(const char*);
int revisit(const char*);
void printSymbolTable(FILE*);
void printRevisitQueue(FILE*);

#endif
