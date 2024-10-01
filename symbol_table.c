#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "symbol_table.h"

static int cur_scope = 0;
static int declared = 0;
static int function_declared = 0;
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
        if (declared) {
            node = (StorageNode*)malloc(sizeof(StorageNode));

            strncpy(node -> storage_name, name, len);
            node -> storage_size = len;
            node -> storage_type = type;
            node -> scope = cur_scope;
            node -> lines = (Referenced*)malloc(sizeof(Referenced));
            node -> lines -> line_no = line_no;
            node -> lines -> next = NULL;

            node -> next = table[hash_val];
            table[hash_val] = node; 
        } else {
            node = (StorageNode*)malloc(sizeof(StorageNode));

            strncpy(node -> storage_name, name, len);
            node -> storage_size = len;
            node -> storage_type = type;
            node -> scope = cur_scope;
            node -> lines = (Referenced*)malloc(sizeof(Referenced));
            node -> lines -> line_no = line_no;
            node -> lines -> next = NULL;

            node -> next = table[hash_val];
            table[hash_val] = node;

            pushToQueue(node, node -> storage_name, ARG_CHECK);
        }
    } else {
        if (!declared) {
            Referenced* lines = node -> lines;

            while (lines -> next != NULL) {
                lines = lines -> next;
            }

            lines -> next = (Referenced*)malloc(sizeof(Referenced));
            lines -> next -> line_no = line_no;
            lines -> next -> next = NULL;
        } else {
            if (node -> scope == cur_scope) {
                fprintf(stderr, "Redefinition of %s at line %d.\n", name, yylineno);
                exit(1);
            } else if (function_declared) {
                Referenced* lines = node -> lines;

                while (lines -> next != NULL) {
                    lines = lines -> next;
                }

                lines -> next = (Referenced*)malloc(sizeof(Referenced));
                lines -> next -> line_no = line_no;
                lines -> next -> next = NULL;
            } else {
                node = (StorageNode*)malloc(sizeof(StorageNode));

                strncpy(node -> storage_name, name, len);
                node -> storage_size = len;
                node -> storage_type = type;
                node -> scope = cur_scope;
                node -> lines = (Referenced*)malloc(sizeof(Referenced));
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
    StorageNode* node = table[hash_val];

    while ((node != NULL) && (strcmp(name, node -> storage_name) != 0)) {
        node = node -> next;
    }

    return node;
}

void setDataType(const char* name, int storage_type, int inferred_type) {
    StorageNode* node = lookup(name);

    node -> storage_type = storage_type;

    if (inferred_type != UNDEF) {
        node -> inferred_type = inferred_type;
    }
}

int getDataType(const char* name) {
    StorageNode* node = lookup(name);

    if (node -> storage_type == INT_TYPE || node -> storage_type == REAL_TYPE || node -> storage_type == CHAR_TYPE || node -> storage_type == STRING_TYPE || node -> storage_type == BOOL_TYPE) {
        return node -> storage_type;
    } else {
        return node -> inferred_type;
    }
}

Argument defArg(int arg_type, const char* arg_name, int pass) {
    Argument arg;

    arg.arg_type = arg_type;
    strcpy(arg.arg_name, arg_name);
    arg.pass = pass;

    return arg;
}

int funcDeclaration(const char* name, int ret_type, int arg_count, Argument *args) {
    StorageNode* node = lookup(name);

    if (node != NULL) {
        if (node -> storage_type == UNDEF) {
            node -> storage_type = FUNCTION_TYPE;
            node -> inferred_type = ret_type;
            node -> arg_count = arg_count;
            node -> args = args;

            return 0;
        } else {
            fprintf(stderr, "Redeclaration of function %s at line %d.\n", name, yylineno);
            exit(1);
        }
    }
}

int funcArgCheck(const char* name, int call_count, int** arg_types, int* arg_count) {
    StorageNode* node = lookup(name);

    for (int i = 0; i < call_count; i++) {
        if (node -> arg_count != arg_count[i]) {
            fprintf(stderr, "Function %s called with incorrect number of arguments at line %d.\n", name, yylineno);
            exit(1);
        }

        for (int j = 0; j < arg_count[i]; j++) {
            int type_1 = node -> args[j].arg_type;
            int type_2 = arg_types[i][j];

            getResultType(type_1, type_2, NONE);
        }
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

void pushToQueue(StorageNode* entry, char* name, int type) {
    RevisitQueue* q;

    if(queue == NULL) {
        q = (RevisitQueue*)malloc(sizeof(RevisitQueue));

        q -> entry = entry;
        q -> storage_name = name;
        q -> revisit_type = type;
        q -> next = NULL;

        if (type == ARG_CHECK) {
            q -> call_count = 0;
        } else if (type == ASSIGN_CHECK) {
            q -> assign_count = 0;
        }

        queue = q;
    } else {
        q = queue;

        while (q -> next != NULL) {
            q = q -> next;
        }

        q -> next = (RevisitQueue*)malloc(sizeof(RevisitQueue));

        q -> next -> entry = entry;
        q -> next -> storage_name = name;
        q -> next -> revisit_type = type;
        q -> next -> next = NULL;

        if (type == ARG_CHECK) {
            q -> next -> call_count = 0;
        } else if (type == ASSIGN_CHECK) {
            q -> next -> assign_count = 0;
        }
    }		
}

RevisitQueue* searchQueue(const char* name) {
    RevisitQueue* q = queue;

    while ((q != NULL) && (strcmp(q -> storage_name, name) != 0)) {
        q = q -> next;
    }

    return q;
}

RevisitQueue* searchPrevQueue(const char* name) {
    if (queue == NULL) {
        return NULL;
    }

    if (strcmp(queue -> storage_name, name) == 0) {
        return NULL;
    }

    RevisitQueue* q = queue;

    while ((q != NULL) && (strcmp(q -> next -> storage_name, name) != 0)) {
        q = q -> next;
    }

    return q;
}

int revisit(const char* name) {
    RevisitQueue* q = searchQueue(name);

    if (q == NULL) {
        return -1;
    }

    switch (q -> revisit_type) {
        case ARG_CHECK: {
            if (!funcArgCheck(name, q -> call_count, q -> arg_types, q -> arg_count)) {
                printf("Function %s called with correct number of arguments.\n", name);
            }

            RevisitQueue* prev = searchPrevQueue(name);

            if (prev == NULL) {
                queue = queue -> next;
            } else {
                prev -> next = prev -> next -> next;
            }

            break;
        }

        case ASSIGN_CHECK: {
            int type_1 = getDataType(q -> entry -> storage_name);

            for (int i = 0; i < q -> assign_count; i++) {
                int type_2 = getExpressionType(q -> nodes[i]);

                getResultType(type_1, type_2, NONE);
            }

            RevisitQueue* prev = searchPrevQueue(name);

            if (prev == NULL) {
                queue = queue -> next;
            } else {
                prev -> next = prev -> next -> next;
            }

            break;
        }

        default:
            break;
    }

    return 0;
}

void printSymbolTable(FILE* of) {
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
                    case VOID_TYPE:
                        fprintf(of, "%-15s", "void");
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
                            case VOID_TYPE:
                                fprintf(of, "%-6s", "void");
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
                        fprintf(of, "%-6s", "func ret ");

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
                            case VOID_TYPE:
                                fprintf(of, "%-6s", "void");
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
            fprintf(of, "%s", "Argument check ");
            fprintf(of, "for %d function calls.", q -> call_count);
        } else if (q -> revisit_type == ASSIGN_CHECK) {
            fprintf(of, "%s", "Assignment check ");
            fprintf(of, "for %d assignments.", q -> assign_count);
        }

        fprintf(of, "\n");
        q = q -> next;	
    }
}