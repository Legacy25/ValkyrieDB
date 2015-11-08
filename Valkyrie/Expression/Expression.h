#ifndef VALKYRIEDB_EXPRESSION_H
#define VALKYRIEDB_EXPRESSION_H


class Expression {
public:
    virtual Value* getValue() = 0;
};

#endif //VALKYRIEDB_EXPRESSION_H
