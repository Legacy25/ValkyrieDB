#ifndef VALKYRIEDB_EXPRESSION_H
#define VALKYRIEDB_EXPRESSION_H

#include <string>
#include "llvm/IR/Value.h"

using namespace llvm;
using namespace std;

namespace valkyrie{

    typedef enum {
        EXPRESSION,
        BINARYEXPRESSION,
        PRIMITIVEVALUE,
        ARITHEXPRESSION,
        LOGEXPRESSION,
        CMPEXPRESSION,
        ADDITIONEXPRESSION,
        SUBTRACTIONEXPRESSION,
        MULTIPLICATIONEXPRESSION,
        DIVISIONEXPRESSION,
        ANDEXPRESSION,
        OREXPRESSION,
        EQUALEXPRESSION,
        NOTEQUALEXPRESSION,
        GREATERTHANEXPRESSION,
        GREATERTHANEQUALEXPRESSION,
        LESSTHANEXPRESSION,
        LESSTHANEQUALEXPRESSION,
        COLEXPRESSION,
        LONGVALUEEXPRESSION,
        DOUBLEVALUEEXPRESSION,
        STRINGVALUEEXPRESSION,
        DATEVALUEEXPRESSION
    } ExprType;

    // Base class for all expressions
    class Expression {
    private:
        ExprType exprType = EXPRESSION;

    public:
        virtual Value* getValue() = 0;
        ExprType getType();
    };



    // Binary Expression is any expression with two child expressions
    class BinaryExpression: public Expression {
    private:
        ExprType exprType = BINARYEXPRESSION;

    protected:
        Expression* leftExpression = NULL;
        Expression* rightExpression = NULL;

    public:
        Expression* getLeftExpression();
        void setLeftExpression(Expression*);

        Expression* getRightExpression();
        void setRightExpression(Expression*);

        virtual Value* getValue() = 0;
    };



    // Primitive is the base of all primitive values
    class PrimitiveValue: public Expression {
    private:
        ExprType exprType = PRIMITIVEVALUE;

    public:
        virtual Value* getValue() = 0;
    };






    // Base class of all arithmetic expressions
    class ArithExpression: public BinaryExpression {
    private:
        ExprType exprType = ARITHEXPRESSION;

    public:
        virtual Value* getValue() = 0;
    };



    // Base class of all logical expressions
    class LogExpression: public BinaryExpression {
    private:
        ExprType exprType = LOGEXPRESSION;

    public:
        virtual Value* getValue() = 0;
    };



    // Base class of all comparison expressions
    class CmpExpression: public BinaryExpression{
    private:
        ExprType exprType = CMPEXPRESSION;

    public:
        virtual Value* getValue() = 0;
    };







    // Arithmetic Operators
    class AdditionExpression: public ArithExpression {
    private:
        ExprType exprType = ADDITIONEXPRESSION;

    public:
        Value* getValue();
    };

    class SubtractionExpression: public ArithExpression {
    private:
        ExprType exprType = SUBTRACTIONEXPRESSION;

    public:
        Value* getValue();
    };

    class MultiplicationExpression: public ArithExpression {
    private:
        ExprType exprType = MULTIPLICATIONEXPRESSION;

    public:
        Value* getValue();
    };

    class DivisionExpression: public ArithExpression {
    private:
        ExprType exprType = DIVISIONEXPRESSION;

    public:
        Value* getValue();
    };






    // Logical Operators
    class AndExpression: public LogExpression {
    private:
        ExprType exprType = ANDEXPRESSION;

    public:
        Value* getValue();
    };

    class OrExpression: public LogExpression {
    private:
        ExprType exprType = OREXPRESSION;

    public:
        Value* getValue();
    };







    // Comparision Operators
    class EqualExpression: public CmpExpression {
    private:
        ExprType exprType = EQUALEXPRESSION;

    public:
        Value* getValue();
    };

    class NotEqualExpression: public CmpExpression {
    private:
        ExprType exprType = NOTEQUALEXPRESSION;

    public:
        Value* getValue();
    };

    class LessThanExpression: public CmpExpression {
    private:
        ExprType exprType = LESSTHANEXPRESSION;

    public:
        Value* getValue();
    };

    class LessThanEqualExpression: public CmpExpression {
    private:
        ExprType exprType = LESSTHANEQUALEXPRESSION;

    public:
        Value* getValue();
    };

    class GreaterThanExpression: public CmpExpression {
    private:
        ExprType exprType = GREATERTHANEXPRESSION;

    public:
        Value* getValue();
    };

    class GreaterThanEqualExpression: public CmpExpression {
    private:
        ExprType exprType = GREATERTHANEQUALEXPRESSION;

    public:
        Value* getValue();
    };







    // Primitives
    class LongValueExpression: public PrimitiveValue {
    private:
        ExprType exprType = LONGVALUEEXPRESSION;
        long data;

    public:
        long getData();
        void setData(long);
        Value* getValue();
    };


    class DoubleValueExpression: public PrimitiveValue {
    private:
        ExprType exprType = DOUBLEVALUEEXPRESSION;
        double data;

    public:
        double getData();
        void setData(double);
        Value* getValue();
    };


    class StringValueExpression: public PrimitiveValue {
    private:
        ExprType exprType = STRINGVALUEEXPRESSION;
        string* data;

    public:
        string* getData();
        void setData(string*);
        Value* getValue();
    };


    class DateValueExpression: public PrimitiveValue {
    private:
        ExprType exprType = DATEVALUEEXPRESSION;
        string* data;

    public:
        string* getData();
        void setData(string*);
        Value* getValue();
    };




    // This represents a column
    class ColExpression: public Expression {
    private:
        ExprType exprType = COLEXPRESSION;
        string colname;

    public:
        string getColName();
        void setColName(string);
        Value getValue();
    };
}



#endif //VALKYRIEDB_EXPRESSION_H
