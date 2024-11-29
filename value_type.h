#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

typedef union ValueTypeUnion {
    int boolean;
    char character;
    long long int integer;
    double real;
    char *string;
} ValueType;

#endif // VALUE_TYPE_H