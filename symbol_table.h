#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define SIZE 10
#define MAX_TOKEN_LEN 100

#define BY_VALUE 1
#define BY_REF 2

#define ARG_CHECK 1

typedef struct ArgumentStruct {
    char name[MAX_TOKEN_LEN];
    int type;
    int integer;
    double real;
    char* string;
    int pass;
} Argument;

typedef struct ReferencedStruct {
    struct ReferencedStruct* next;
    int type;
    int line_no;
} Referenced;

typedef struct NodeStruct {
    struct NodeStruct* next;
    Referenced* lines;
    Argument* args;
    char name[MAX_TOKEN_LEN];
    int size;
    int scope;
    int storage;
    int inferred;
    int integer;
    double real;
    char character;
    int* int_array;
    double* real_array;
    char* char_array;
    int array_size;
    int num_args;
} Node;

typedef struct QueueStruct {
    struct QueueStruct* next;
    char* name;
    int type;
} Queue;

typedef struct TableStruct {
    Node** buckets;
    int size;
    int count;
} Table;

static Table* table;
static Queue* queue;

void initTable();
unsigned int hash(const char*);
void insert(const char*, int, int, int);
void resize();
Node* lookup(const char*);
void setType(const char*, int, int);
int getType(const char*);
Argument defArg(int, const char*, int);
int funcDeclaration(const char*, int, int, Argument*);
int funcArgCheck(const char*, int, Argument*);
void hideScope();
void incrScope();
void push(char*, int);
int revisit(const char*);
void printTable(FILE*);
void printQueue(FILE*);
void freeReferencedList(Referenced*);
void freeTable();

#endif