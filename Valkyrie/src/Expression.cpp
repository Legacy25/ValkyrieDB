#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "../include/Expression.h"
#include "../include/Codegen.h"

using namespace valkyrie;
using namespace llvm;



ExprType Expression::getType() {
    return exprType;
}



Expression *BinaryExpression::getLeftExpression() {
    return leftExpression;
}

void BinaryExpression::setLeftExpression(Expression *expression) {
    leftExpression = expression;
}

Expression *BinaryExpression::getRightExpression() {
    return rightExpression;
}

void BinaryExpression::setRightExpression(Expression *expression) {
    rightExpression = expression;
}




Value* AdditionExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateAdd(leftExpression->getValue(), rightExpression->getValue());
}

Value* SubtractionExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateSub(leftExpression->getValue(), rightExpression->getValue());
}

Value* MultiplicationExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateMul(leftExpression->getValue(), rightExpression->getValue());
}

Value* DivisionExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateFDiv(leftExpression->getValue(), rightExpression->getValue());
}




Value* AndExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateAnd(leftExpression->getValue(), rightExpression->getValue());
}

Value* OrExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);

    IRBuilder<>* builder = codegen::getBuilder();
    return builder->CreateOr(leftExpression->getValue(), rightExpression->getValue());
}


/**
 * TODO The comparisions will be type dependent, now only works for longs and doubles
 * ICmp is for integers (long)
 * For doubles, need to use FCmp
 * For strings, need to iterate over the ConstantArray and compare each element
 * using getelementptr and invoking ICmp on them
 */

Value* EqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    switch(leftExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpEQ(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOEQ(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* NotEqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    switch(leftExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpNE(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpONE(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* GreaterThanExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    switch(leftExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSGT(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOGT(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* GreaterThanEqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    switch(leftExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSGE(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOGE(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* LessThanExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    switch(leftExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSLT(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOLT(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}

Value* LessThanEqualExpression::getValue() {
    assert(leftExpression != NULL);
    assert(rightExpression != NULL);
    assert(leftExpression->getType() == rightExpression->getType());

    IRBuilder<>* builder = codegen::getBuilder();
    switch(leftExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpSLE(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOLE(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:
        case DATEVALUEEXPRESSION:
            // TODO
            break;
        default:
            cerr << "Unknown expression type!" << endl;
            exit(-1);
    }

    return NULL;
}






Value* LongValueExpression::getValue() {
    Type* int64Ty = Type::getInt64Ty(getGlobalContext());
    return ConstantInt::get(int64Ty, data, true);
}

Value* DoubleValueExpression::getValue() {
    Type* doubleTy = Type::getDoubleTy(getGlobalContext());
    return ConstantFP::get(doubleTy, data);
}

Value* StringValueExpression::getValue() {
    // TODO
    return NULL;
}

Value* DateValueExpression::getValue() {
    // TODO
    return NULL;
}

Value* ColExpression::getValue() {
    IRBuilder<>* builder = codegen::getBuilder();
    // TODO
    return NULL;
}
