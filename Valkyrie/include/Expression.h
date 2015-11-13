#ifndef VALKYRIEDB_EXPRESSION_H
#define VALKYRIEDB_EXPRESSION_H

#include <string>
#include "llvm/IR/Value.h"

using namespace llvm;
using namespace std;

namespace valkyrie{

    enum ExprType {
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
    };

    // Base class for all expressions
    class Expression {
    private:
    public:
        virtual Value* getValue() = 0;
        virtual ExprType getType(){ return ExprType::EXPRESSION; }
    };



    // Binary Expression is any expression with two child expressions
    class BinaryExpression: public Expression {
    private:
    protected:
        Expression* leftExpression = NULL;
        Expression* rightExpression = NULL;

    public:
        Expression* getLeftExpression();
        void setLeftExpression(Expression*);

        Expression* getRightExpression();
        void setRightExpression(Expression*);

        virtual Value* getValue() = 0;
        virtual ExprType getType(){ return ExprType::BINARYEXPRESSION; }
    };



    // Primitive is the base of all primitive values
    class PrimitiveValue: public Expression {
    private:
    public:
        virtual Value* getValue() = 0;
        virtual ExprType getType(){ return ExprType::PRIMITIVEVALUE; }
    };






    // Base class of all arithmetic expressions
    class ArithExpression: public BinaryExpression {
    private:
    public:
        virtual Value* getValue() = 0;
        virtual ExprType getType(){ return ExprType::ARITHEXPRESSION; }
    };



    // Base class of all logical expressions
    class LogExpression: public BinaryExpression {
    private:
    public:
        virtual Value* getValue() = 0;
    };



    // Base class of all comparison expressions
    class CmpExpression: public BinaryExpression{
    private:
    public:
        virtual Value* getValue() = 0;
        virtual ExprType getType(){ return ExprType::CMPEXPRESSION; }
    };







    // Arithmetic Operators
    class AdditionExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
    };

    class SubtractionExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
    };

    class MultiplicationExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
    };

    class DivisionExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
    };






    // Logical Operators
    class AndExpression: public LogExpression {
    private:
    public:
        Value* getValue();
    };

    class OrExpression: public LogExpression {
    private:
    public:
        Value* getValue();
    };







    // Comparision Operators
    class EqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        ExprType getType(){ return ExprType::EQUALEXPRESSION; }
    };

    class NotEqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
    };

    class LessThanExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
    };

    class LessThanEqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
    };

    class GreaterThanExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        ExprType getType(){ return ExprType::GREATERTHANEXPRESSION; }
    };

    class GreaterThanEqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
    };







    // Primitives
    class LongValueExpression: public PrimitiveValue {
    private:
        long data;

    public:
        LongValueExpression(long data);
        long getData();
        void setData(long);
        Value* getValue();
        ExprType getType(){ return ExprType::LONGVALUEEXPRESSION; }
    };


    class DoubleValueExpression: public PrimitiveValue {
    private:
        double data;

    public:
        DoubleValueExpression(double data);
        double getData();
        void setData(double);
        Value* getValue();
        ExprType getType(){ return ExprType::DOUBLEVALUEEXPRESSION; }
    };


    class StringValueExpression: public PrimitiveValue {
    private:
        string* data;

    public:
        StringValueExpression(string* data);
        string* getData();
        void setData(string*);
        Value* getValue();
    };


    class DateValueExpression: public PrimitiveValue {
    private:
        string* data;

    public:
        string* getData();
        void setData(string*);
        Value* getValue();
    };




    // This represents a column
    class ColExpression: public Expression {
    private:
        string colname;
    public:
        ColExpression(string name);
        string getColName();
        void setColName(string);
        Value* getValue();
        ExprType getType(){ return ExprType::COLEXPRESSION; }
    };
}



#endif //VALKYRIEDB_EXPRESSION_H
