#ifndef PROJECT_DATATYPES_H
#define PROJECT_DATATYPES_H

#include <cstdint>

using namespace std;

enum DataType {
    LONG,
    DOUBLE,
    STRING,
    DATE
};

typedef union {
    long l;
    double d;
    char* c;
} LeafValue;

typedef struct tuplePointer {
    int64_t ptr;
    size_t att_count;
    size_t tup_count;
} TupPtr;

#endif //PROJECT_DATATYPES_H
