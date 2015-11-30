#ifndef VALKYRIEDB_EXPRESSION_H
#define VALKYRIEDB_EXPRESSION_H

#include <string>
#include <stack>
#include <algorithm>
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
        virtual string toString() = 0;
        /*string expressionPlan(){
            std::string res = "";
            std::stack<std::pair<Expression*, int> > s;
            s.push(std::make_pair(this, 0));
            while(!s.empty()){
                std::pair<Expression*, int> p = s.top();
                s.pop();
                res += "\n";
                for(int i = 0; i < p.second; i++)
                    res += "\t";
                res += p.first->toString();
                std::vector<Expression*> v = p.first->getChildren();
                for(int i = 0; i < v.size(); i++)
                    s.push(std::make_pair(v[i], p.second+1));
            }
            return res;
        }*/
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
        string toString(){return "+";}
    };

    class SubtractionExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "-";}
    };

    class MultiplicationExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "*";}
    };

    class DivisionExpression: public ArithExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "/";}
    };

    // Logical Operators
    class AndExpression: public LogExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "AND";}
    };

    class OrExpression: public LogExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "OR";}
    };

    // Comparision Operators
    class EqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        ExprType getType(){ return ExprType::EQUALEXPRESSION; }
        string toString(){return "=";}
    };

    class NotEqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "<>";}
    };

    class LessThanExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "<";}
    };

    class LessThanEqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        string toString(){return "<=";}
    };

    class GreaterThanExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        ExprType getType(){ return ExprType::GREATERTHANEXPRESSION; }
        string toString(){return ">";}
    };

    class GreaterThanEqualExpression: public CmpExpression {
    private:
    public:
        Value* getValue();
        string toString(){return ">=";}
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
        string toString(){return to_string(data);}
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
        string toString(){return to_string(data);}
    };

    class StringValueExpression: public PrimitiveValue {
    private:
        string* data;

    public:
        StringValueExpression(string* data);
        string* getData();
        void setData(string*);
        Value* getValue();
        string toString(){return *data;}
    };

    class DateValueExpression: public PrimitiveValue {
    private:
        string* data;

    public:
        string* getData();
        void setData(string*);
        Value* getValue();
        string toString(){return *data;}
    };

    // This represents a column
    class ColExpression: public Expression {
    private:
        string colname;
        int index = -1;
    public:
        ColExpression(string name);
        ColExpression(string name, int colPos);
        void setColPos(int pos);
        int getColPos();
        string getColName();
        void setColName(string);
        Value* getValue();
        ExprType getType(){ return ExprType::COLEXPRESSION; }
        string toString(){
            if(index != -1)
                return colname + "(" + std::to_string(index) + ")";
            return colname;
        }
    };
}

#endif //VALKYRIEDB_EXPRESSION_H
