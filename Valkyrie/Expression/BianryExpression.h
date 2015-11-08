#ifndef VALKYRIEDB_BIANRYEXPRESSION_H
#define VALKYRIEDB_BIANRYEXPRESSION_H

#include "Expression.h"
#include "Operations.h"

class BinaryExpression: public Expression {
protected:
    Expression* leftExpression;
    Expression* rightExpression;
public:
    Expression* getLeftExpression();
    void setLeftExpression(Expression*);
    Expression* getRightExpression();
    void setRightExpression(Expression*);

    virtual Value* getValue() = 0;
    virtual Value* evaluate() = 0;
};

//Arithmetic Operators
class Addition: public BinaryExpression {
    Operations operation = Operations.ADDITION;
public:
    Value* getValue();
    Value* evaluate();
};


class Subtraction: public BinaryExpression {
    Operations operation = Operations.SUBTRACTION;
public:
    Value* getValue();
    Value* evaluate();
};

class Multiplication: public BinaryExpression {
    Operations operation = Operations.MULTIPLICATION;
public:
    Value* getValue();
    Value* evaluate();
};

class Division: public BinaryExpression {
    Operations operation = Operations.DIVISION;
public:
    Value* getValue();
    Value* evaluate();
};

//Logical Operators
class And: public BinaryExpression {
    Operations operation = Operations.AND;
public:
    Value* getValue();
};

class Or: public BinaryExpression {
    Operations operation = Operations.OR;
public:
    Value* getValue();
    Value* evaluate();
};

class Equal: public BinaryExpression {
    Operations operation = Operations.EQUAL;
public:
    Value* getValue();
    Value* evaluate();
};

class NotEqual: public BinaryExpression {
    Operations operation = Operations.NOT_EQUAL;
public:
    Value* getValue();
    Value* evaluate();
};

class LessThan: public BinaryExpression {
    Operations operation = Operations.LESS_THAN;
public:
    Value* getValue();
    Value* evaluate();
};

class LessThanEqual: public BinaryExpression {
    Operations operation = Operations.LESS_THAN_EQUAL;
public:
    Value* getValue();
    Value* evaluate();
};

class GreaterThan: public BinaryExpression {
    Operations operation = Operations.GREATER_THAN;
public:
    Value* getValue();
    Value* evaluate();
};

class GreaterThanEqual: public BinaryExpression {
    Operations operation = Operations.GREATER_THAN_EQUAL;
public:
    Value* getValue();
    Value* evaluate();
};


#endif //VALKYRIEDB_BIANRYEXPRESSION_H
