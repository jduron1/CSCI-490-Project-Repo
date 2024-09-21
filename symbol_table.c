#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "symbol_table.h"

int cur_scope = 0;
int declared = 0;

void initTable() {
    table = (Table*)malloc(sizeof(Table));

    table -> size = SIZE;
    table -> count = 0;
    table -> buckets = (Node**)calloc(table -> size, sizeof(Node*));

    for (int i = 0; i < table -> size; i++) {
        table -> buckets[i] = NULL;
    }
}

unsigned int hash(const char* key) {
    unsigned int hash_value = 0;

    for (int i = 0; key[i] != '\0'; i++) {
        hash_value += key[i];
    }

    hash_value += key[0] % 11 + (key[0] << 3) - key[0];

    return hash_value % table -> size;
}

void insert(const char* name, int length, int type, int line_no) {
    if (table -> count == table -> size) {
        resize();
    }

    unsigned int hash_value = hash(name);
    Node* node = table -> buckets[hash_value];

    while (node != NULL && strcmp(node -> name, name) != 0) {
        node = node -> next;
    }

    if (node == NULL) {
        if (declared) {
            node = (Node*)malloc(sizeof(Node));

            strncpy(node -> name, name, length);
            node -> storage = type;
            node -> scope = cur_scope;
            node -> lines = (Referenced*)malloc(sizeof(Referenced));
            node -> lines -> line_no = line_no;
            node -> lines -> next = NULL;

            node -> next = table -> buckets[hash_value];
            table -> buckets[hash_value] = node;
            table -> count++;
        } else {
            node = (Node*)malloc(sizeof(Node));

            strncpy(node -> name, name, length);
            node -> storage = type;
            node -> scope = cur_scope;
            node -> lines = (Referenced*)malloc(sizeof(Referenced));
            node -> lines -> line_no = line_no;
            node -> lines -> next = NULL;

            node -> next = table -> buckets[hash_value];
            table -> buckets[hash_value] = node;
            table -> count++;
            
            push(node -> name, ARG_CHECK);
        }
    } else {
        if (!declared) {
            Referenced* last = node -> lines;

            while (last -> next != NULL) {
                last = last -> next;
            }

            last -> next = (Referenced*)malloc(sizeof(Referenced));
            last -> next -> line_no = line_no;
            last -> next -> next = NULL;
        } else {
            if (node -> scope == cur_scope) {
                fprintf(stderr, "Redefinition of %s.\n", name);
                exit(1);
            } else {
                node = (Node*)malloc(sizeof(Node));

                strncpy(node -> name, name, length);
                node -> storage = type;
                node -> scope = cur_scope;
                node -> lines = (Referenced*)malloc(sizeof(Referenced));
                node -> lines -> line_no = line_no;
                node -> lines -> next = NULL;

                node -> next = table -> buckets[hash_value];
                table -> buckets[hash_value] = node;
                table -> count++;
            }
        }
    }
}

void resize() {
    int new_size = table -> size * 2;
    Node** new_buckets = (Node**)calloc(new_size, sizeof(Node*));

    for (int i = 0; i < table -> size; i++) {
        Node* cur = table -> buckets[i];

        while (cur != NULL) {
            Node* next = cur -> next;
            unsigned int new_idx = hash(cur -> name);
            cur -> next = new_buckets[new_idx];
            new_buckets[new_idx] = cur;
            cur = next;
        }
    }

    free(table -> buckets);
    table -> buckets = new_buckets;
    table -> size = new_size;
}

Node* lookup(const char* name) {
    unsigned int hash_value = hash(name);
    Node* node = table -> buckets[hash_value];

    while (node != NULL && strcmp(node -> name, name) != 0) {
        node = node -> next;
    }

    return node;
}

void setType(const char* name, int storage, int inferrence) {
    Node* node = lookup(name);

    node -> storage = storage;

    if (inferrence != UNDEF) {
        node -> inferred = inferrence;
    }
}

int getType(const char* name) {
    Node* node = lookup(name);

    if (node -> storage == INT_TYPE || node -> storage == REAL_TYPE || node -> storage == CHAR_TYPE || node -> storage == BOOL_TYPE) {
        return node -> storage;
    } else {
        return node -> inferred;
    }
}

Argument defArg(int type, const char* name, int pass) {
    Argument arg;

    arg.type = type;
    strcpy(arg.name, name);
    arg.pass = pass;

    return arg;
}

int funcDeclaration(const char* name, int type, int num_args, Argument* args) {
    Node* node = lookup(name);

    if (node -> storage != UNDEF) {
        node -> storage = FUNC_TYPE;
        node -> inferred = type;
        node -> num_args = num_args;
        node -> args = args;
    } else {
        fprintf(stderr, "Redefinition of %s.\n", name);
        exit(1);
    }

    return 0;
}

int funcArgCheck(const char* name, int num_args, Argument* args) {
    Node* node = lookup(name);

    if (node -> num_args != num_args) {
        fprintf(stderr, "Too few arguments to function %s.\n", name);
        exit(1);
    } else {
        int type_1, type_2;

        for (int i = 0; i < num_args; i++) {
            type_1 = node -> args[i].type;
            type_2 = args[i].type;

            getResultType(type_1, type_2, NONE);
        }
    }

    return 0;
}

void hideScope() {
    Node* node;

    for (int i = 0; i < table -> size; i++) {
        if (table -> buckets[i] != NULL) {
            node = table -> buckets[i];

            while (node != NULL && node -> scope == cur_scope) {
                node = node -> next;
            }

            table -> buckets[i] = node;
            table -> count--;
        }
    }

    cur_scope--;
}

void incrScope() {
    cur_scope++;
}

void push(char* name, int type) {
    Queue* temp;

    if (queue == NULL) {
        temp = (Queue*)malloc(sizeof(Queue));

        temp -> name = name;
        temp -> type = type;
        temp -> next = NULL;

        queue = temp;
    } else {
        temp = queue;

        while (temp -> next != NULL) {
            temp = temp -> next;
        }

        temp -> next = (Queue*)malloc(sizeof(Queue));

        temp -> next -> name = name;
        temp -> next -> type = type;
        temp -> next -> next = NULL;
    }
}

int revisit(const char* name) {
    Queue* cur;

    if (strcmp(queue -> name, name) == 0) {
        switch (queue -> type) {
            case ARG_CHECK:
                break;
            default:
                break;
        }

        cur = queue;
        queue = queue -> next;
        free(cur);

        return 0;
    } else {
        cur = queue;

        while (strcmp(cur -> next -> name, name) != 0) {
            cur = cur -> next;
        }

        if (cur == NULL) {
            return 1;
        }

        switch (cur -> next -> type) {
            case ARG_CHECK:
                break;
            default:
                break;
        }

        Queue* temp = cur -> next;
        cur -> next = cur -> next -> next;
        free(temp);

        return 0;
    }
}

void printTable(FILE * of){  
  fprintf(of, "------------ ------ ------ ------------\n");
  fprintf(of, "Name         Type   Scope  Line Numbers\n");
  fprintf(of, "------------ ------ ------ ------------\n");

  for (int i = 0; i < table -> size; i++) {
    if (table -> buckets[i] != NULL) { 
        Node* node = table -> buckets[i];
        
        while (node != NULL) {
            Referenced *ref = node -> lines;
            fprintf(of, "%-12s ", node -> name);

            switch (node -> storage) {
                case INT_TYPE:
                    fprintf(of, "%-7s", "int");
                    break;
                case REAL_TYPE:
                    fprintf(of, "%-7s", "real");
                    break;
                case CHAR_TYPE:
                    fprintf(of, "%-7s", "string");
                    break;
                case BOOL_TYPE:
                    fprintf(of, "%-7s", "bool");
                    break;
                case ARR_TYPE:
                    fprintf(of, "array of ");

                    switch (node -> inferred) {
                        case INT_TYPE:
                            fprintf(of, "%-7s", "int");
                            break;
                        case REAL_TYPE:
                            fprintf(of, "%-7s", "real");
                            break;
                        case CHAR_TYPE:
                            fprintf(of, "%-7s", "string");
                            break;
                        case BOOL_TYPE:
                            fprintf(of, "%-7s", "bool");
                            break;
                        default:
                            fprintf(of, "%-7s", "undef");
                            break;
                    }
                    
                    break;
                case FUNC_TYPE:
                    fprintf(of, "%-7s", "function returns ");

                    switch (node -> inferred) {
                        case INT_TYPE:
                            fprintf(of, "%-7s", "int");
                            break;
                        case REAL_TYPE:
                            fprintf(of, "%-7s", "real");
                            break;
                        case CHAR_TYPE:
                            fprintf(of, "%-7s", "string");
                            break;
                        case BOOL_TYPE:
                            fprintf(of, "%-7s", "bool");
                            break;
                        default:
                            fprintf(of, "%-7s", "undef");
                            break;
                    }

                    break;
                default:
                    fprintf(of, "%-7s", "undef");
                    break;
            }

            fprintf(of, "  %d  ", node -> scope);
            
            while (ref != NULL) {
                fprintf(of, "%4d ", ref -> line_no);
                ref = ref -> next;
            }
            
            fprintf(of,"\n");
            node = node -> next;
        }
    }
  }
}

void printQueue(FILE* of) {
    Queue* temp = queue;

    fprintf(of, "------------ -------------\n");
    fprintf(of, "Identifier   Revisit Type\n");
    fprintf(of, "------------ -------------\n");

    while (temp != NULL) {
        fprintf(of, "%-13s", temp -> name);

        if (temp -> type == ARG_CHECK) {
            fprintf(of, "%s", "Parameter Check");
        }

        fprintf(of, "\n");
        temp = temp -> next;
    }
}

void freeTable() {
    for (int i = 0; i < table -> size; i++) {
        Node* cur = table -> buckets[i];

        while (cur != NULL) {
            Node* next = cur -> next;
            free(cur);
            cur = next;
        }
    }

    free(table -> buckets);
    free(table);
}