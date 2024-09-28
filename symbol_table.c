#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "symbol_table.h"

static int cur_scope = 0;
static int declared = 0;
extern int yylineno;

void initSymbolTable() {
    table = (StorageNode**)malloc(SIZE * sizeof(StorageNode*));

    for(int i = 0; i < SIZE; i++) {
        table[i] = NULL;
    }
}

unsigned int hash(const char* key) {
    unsigned int hash_val = 0;

    while (*key != '\0') {
        hash_val += *key++;
    }

    hash_val += key[0] % 11 + (key[0] << 3) - key[0];

    return hash_val % SIZE;
}

void insert(const char* name, int len, int type, int line_no) {
    unsigned int hash_val = hash(name);
    StorageNode* node = table[hash_val];

    while ((node != NULL) && (strcmp(name, node -> storage_name) != 0)) {
        node = node -> next;
    }

    if (node == NULL) {
        if(declared == 1) {
            node = (StorageNode*)malloc(sizeof(StorageNode));
            strncpy(node -> storage_name, name, len);

            node -> storage_size = len;
            node -> storage_type = type;
            node -> scope = cur_scope;
            node -> lines = (Referenced*) malloc(sizeof(Referenced));
            node -> lines -> line_no = line_no;
            node -> lines -> next = NULL;

            node -> next = table[hash_val];
            table[hash_val] = node; 
        } else {
            node = (StorageNode*) malloc(sizeof(StorageNode));
            strncpy(node -> storage_name, name, len);

            node -> storage_size = len;
            node -> storage_type = type;
            node -> scope = cur_scope;
            node -> lines = (Referenced*) malloc(sizeof(Referenced));
            node -> lines -> line_no = line_no;
            node -> lines -> next = NULL;

            node -> next = table[hash_val];
            table[hash_val] = node;

            pushToQueue(node -> storage_name, ARG_CHECK);
        }
    } else {
        if (declared == 0) {
            Referenced* lines = node -> lines;

            while (lines -> next != NULL) {
                lines = lines -> next;
            }

            lines -> next = (Referenced*)malloc(sizeof(Referenced));
            lines -> next -> line_no = line_no;
            lines -> next -> next = NULL;
        } else{
            if (node -> scope == cur_scope) {
                fprintf(stderr, "Redefinition of %s at line %d.\n", name, yylineno);
                exit(1);
            } else{
                node = (StorageNode*) malloc(sizeof(StorageNode));
                strncpy(node -> storage_name, name, len);

                node -> storage_size = len;
                node -> storage_type = type;
                node -> scope = cur_scope;
                node -> lines = (Referenced*) malloc(sizeof(Referenced));
                node -> lines -> line_no = line_no;
                node -> lines -> next = NULL;

                node -> next = table[hash_val];
                table[hash_val] = node;
            }	
        }		
    }
}

StorageNode* lookup(const char* name) {
    unsigned int hash_val = hash(name);
    StorageNode *node = table[hash_val];

    while ((node != NULL) && (strcmp(name, node -> storage_name) != 0)) {
        node = node -> next;
    }

    return node;
}

void setType(const char* name, int storage_type, int inferred_type) {
    StorageNode* node = lookup(name);

    node -> storage_type = storage_type;

    if (inferred_type != UNDEF) {
        node -> inferred_type = inferred_type;
    }
}

int getType(const char* name) {
    StorageNode* node = lookup(name);

    if (node -> storage_type == INT_TYPE || node -> storage_type == REAL_TYPE || node -> storage_type == CHAR_TYPE || node -> storage_type == STRING_TYPE || node -> storage_type == BOOL_TYPE) {
        return node -> storage_type;
    } else {
        return node -> inferred_type;
    }
}

Argument defArg(int arg_type, const char* arg_name, int pass){
    Argument arg;

    arg.arg_type = arg_type;
    strcpy(arg.arg_name, arg_name);
    arg.pass = pass;

    return arg;
}

int funcDeclaration(const char* name, int ret_type, int arg_count, Argument *args) {
    StorageNode* node = lookup(name);

    if (node -> storage_type != UNDEF) {
        node -> storage_type = FUNCTION_TYPE;
        node -> inferred_type = ret_type;
        node -> arg_count = arg_count;
        node -> args = args;

        return 0;
    } else{
        fprintf(stderr, "Redefinition of %s at line %d.\n", name, yylineno);
        exit(1);
    }
}

int funcArgCheck(const char* name, int arg_count, Argument* args) {
    int type_1, type_2;
    StorageNode* node = lookup(name);

    if(node -> arg_count != arg_count){
        fprintf(stderr, "Too few arguments to function %s at line %d.\n", name, yylineno);
        exit(1);
    }
    
    for (int i = 0; i < arg_count; i++) {
        type_1 = node -> args[i].arg_type;
        type_2 = args[i].arg_type;

        getResultType(type_1, type_2, NONE);
    }

    return 0;
}

void hideScope() {
    StorageNode* node;

    for (int i = 0; i < SIZE; i++) {
        if (table[i] != NULL) {
            node = table[i];

            while (node != NULL && node -> scope == cur_scope) {
                node = node -> next;
            }

            table[i] = node;
        }
    }

    cur_scope--;
}

void incrScope() {
    cur_scope++;
}

void pushToQueue(char* name, int type) {
    RevisitQueue* q;

    if(queue == NULL){
        q = (RevisitQueue*)malloc(sizeof(RevisitQueue));

        q -> storage_name = name;
        q -> revisit_type = type;
        q -> next = NULL;

        queue = q;
    } else {
        q = queue;

        while (q -> next != NULL) {
            q = q -> next;
        }

        q -> next = (RevisitQueue*) malloc(sizeof(RevisitQueue));

        q -> next -> storage_name = name;
        q -> next -> revisit_type = type;
        q -> next -> next = NULL;
    }		
}

int revisit(const char* name) {
    RevisitQueue* q;

    if (strcmp(queue -> storage_name, name) == 0) {
        switch(queue -> revisit_type) {
            case ARG_CHECK:
                break;
        }

        queue = queue -> next;

        return 0;
    }

    q = queue;

    while (strcmp(q -> next -> storage_name, name) != 0) {
        q = q -> next;
    }

    if (q == NULL) {
        return 1;
    }

    switch (q -> next -> revisit_type) {
        case ARG_CHECK:
            break;
    }

    q -> next = q -> next -> next;

    return 0;
}

void printSymbolTable(FILE* of){
    fprintf(of, "------------ -------------- ------ ------------\n");
    fprintf(of, "Name         Type           Scope  Line Numbers\n");
    fprintf(of, "------------ -------------- ------ ------------\n");

    for (int i = 0; i < SIZE; i++) {
        if (table[i] != NULL) { 
            StorageNode* node = table[i];
            
            while (node != NULL) {
                Referenced* last = node -> lines;
                fprintf(of, "%-13s ", node -> storage_name);

                switch (node -> storage_type) {
                    case INT_TYPE:
                        fprintf(of, "%-15s", "int");
                        break;
                    case REAL_TYPE:
                        fprintf(of, "%-15s", "real");
                        break;
                    case CHAR_TYPE:
                        fprintf(of, "%-15s", "char");
                        break;
                    case STRING_TYPE:
                        fprintf(of, "%-15s", "string");
                        break;
                    case BOOL_TYPE:
                        fprintf(of, "%-15s", "bool");
                        break;
                    case ARRAY_TYPE:
                        fprintf(of, "array of ");

                        switch (node -> inferred_type) {
                            case INT_TYPE:
                                fprintf(of, "%-6s", "int");
                                break;
                            case REAL_TYPE:
                                fprintf(of, "%-6s", "real");
                                break;
                            case CHAR_TYPE:
                                fprintf(of, "%-6s", "char");
                                break;
                            case STRING_TYPE:
                                fprintf(of, "%-6s", "string");
                                break;
                            case BOOL_TYPE:
                                fprintf(of, "%-6s", "bool");
                                break;
                            default:
                                fprintf(of, "%-13s", "undef");
                                break;
                        }
                        
                        break;
                    case POINTER_TYPE:
                        fprintf(of, "pointer to ");

                        switch (node -> inferred_type) {
                            case INT_TYPE:
                                fprintf(of, "%-4s", "int");
                                break;
                            case REAL_TYPE:
                                fprintf(of, "%-4s", "real");
                                break;
                            case CHAR_TYPE:
                                fprintf(of, "%-4s", "char");
                                break;
                            case STRING_TYPE:
                                fprintf(of, "%-4s", "string");
                                break;
                            case VOID_TYPE:
                                fprintf(of, "%-4s", "void");
                                break;
                            default:
                                fprintf(of, "%-4s", "undef");
                                break;
                        }

                        break;
                    case FUNCTION_TYPE:
                        fprintf(of, "%-6s", "function returns ");

                        switch (node -> inferred_type) {
                            case INT_TYPE:
                                fprintf(of, "%-6s", "int");
                                break;
                            case REAL_TYPE:
                                fprintf(of, "%-6s", "real");
                                break;
                            case CHAR_TYPE:
                                fprintf(of, "%-6s", "char");
                                break;
                            case STRING_TYPE:
                                fprintf(of, "%-6s", "string");
                                break;
                            case BOOL_TYPE:
                                fprintf(of, "%-6s", "bool");
                                break;
                            default:
                                fprintf(of, "%-4s", "undef");
                                break;
                        }

                        break;
                    default:
                        fprintf(of, "%-15s", "undef");
                        break;
                }

                fprintf(of, "  %d  ", node -> scope);
                
                while (last != NULL) {
                    fprintf(of, "%4d ", last -> line_no);
                    last = last -> next;
                }
                
                fprintf(of,"\n");
                node = node -> next;
            }
        }
    }
}

void printRevisitQueue(FILE* of) {
    RevisitQueue* q = queue;
    
    fprintf(of, "------------ -------------\n");
    fprintf(of, "Identifier   Revisit Type\n");
    fprintf(of, "------------ -------------\n");

    while (q != NULL) {
        fprintf(of, "%-13s", q -> storage_name);

        if (q -> revisit_type == ARG_CHECK) {
            fprintf(of,"%s","Parameter Check");
        }

        fprintf(of, "\n");
        q = q -> next;	
    }
}