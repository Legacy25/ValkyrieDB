#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "../include/Expression.h"
#include "../include/Codegen.h"

using namespace valkyrie;
using namespace llvm;

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
    switch(rightExpression->getType()) {
        case LONGVALUEEXPRESSION:
            return builder->CreateICmpEQ(leftExpression->getValue(), rightExpression->getValue());
        case DOUBLEVALUEEXPRESSION:
            return builder->CreateFCmpOEQ(leftExpression->getValue(), rightExpression->getValue());
        case STRINGVALUEEXPRESSION:

            // Use StringRef's compare()
            // (or)
            // iterate through the characters in the String for comparision
            //------------------------------------------------------------------------
            /*static uint32_t nameCounter = 0;

            FunctionType *loopFunctionType = FunctionType::get(int32Type, false);
            Function *loopFunction = Function::Create(loopFunctionType, Function::ExternalLinkage, "StringCheckStart", module);

            BasicBlock *entry = BasicBlock::Create(context, "entry", loopFunction);
            builder->SetInsertPoint(entry);


            Value *loopIndex = builder->CreateAlloca(int32Type, ConstantInt::get(int32Type, 1), "loopVar" + to_string(nameCounter++));
            builder->CreateStore(ConstantInt::get(int32Type, 0), loopIndex);

            BasicBlock *startLoopBody = BasicBlock::Create(context, "startLoopBody" + to_string(nameCounter++), loopFunction);
            builder->CreateBr(startLoopBody);
            builder->SetInsertPoint(startLoopBody);

            Value *i = builder->CreateLoad(loopIndex);

            Value *indices[1];
            indices[0] = i;
            ArrayRef<Value *> indicesRef(indices);*/

            //TODO
            //Value *charString1 = builder->CreateLoad(   /* Load each character of the String 1 */   );
            //Value *charString2 = builder->CreateLoad(   /* Load each character of the String 2 */   );

            //Value *increment = builder->CreateAdd(i, ConstantInt::get(int32Type, 1));
            //builder->CreateStore(increment, loopIndex);

            // Comapare both characters and use this as an exit condition of the loop
            // Or Run the loop until the length of small String
            //TODO
            //Value *cmp = builder->CreateICmpSLT(increment, /* min of lenString1 and lenString2 */);

            /*BasicBlock *endLoopBody = BasicBlock::Create(context, "endLoopBody"+to_string(nameCounter++), loopFunction);
            builder->CreateCondBr(cmp, startLoopBody, endLoopBody);
            builder->SetInsertPoint(endLoopBody);*/

            //------------------------------------------------------------------------


            /* BackUp - Code to use an if condition within the loop

            // if both characters not equal return false
            // false block which returns FALSE
            BasicBlock *startFalseBlock = BasicBlock::Create(context, "If"+to_string(nameCtr++), loopFunction);
            builder->CreateBr(startFalseBlock);
            builder->SetInsertPoint(startFalseBlock);

            //Compare characters

            BasicBlock *endFalseBlock = BasicBlock::Create(context, "endLoopBody"+to_string(nameCounter++), loopFunction);
            builder->CreateCondBr(cmp, startFalseBlock, endFalseBlock);
            builder->SetInsertPoint(endLoopBody);
            */

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
    switch(rightExpression->getType()) {
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

LongValueExpression::LongValueExpression(long data){
    this->data = data;
}

Value* LongValueExpression::getValue() {
    Type* int64Ty = Type::getInt64Ty(getGlobalContext());
    return ConstantInt::get(int64Ty, data, true);
}

DoubleValueExpression::DoubleValueExpression(double data){
    this->data = data;
}

Value* DoubleValueExpression::getValue() {
    Type* doubleTy = Type::getDoubleTy(getGlobalContext());
    return ConstantFP::get(doubleTy, data);
}

StringValueExpression::StringValueExpression(std::string* data){
    this->data = data;
}

Value* StringValueExpression::getValue() {
    // TODO
    return NULL;
}

Value* DateValueExpression::getValue() {
    // TODO
    return NULL;
}

ColExpression::ColExpression(std::string name){
    colname = name;
}

ColExpression::ColExpression(string name, int colPos) {
    colname = name;
    index = colPos;
}

void ColExpression::setColPos(int pos){
    this->index = pos;
}

int ColExpression::getColPos(){
    return this->index;
}

string ColExpression::getColName() {
    return this->colname;
}

void ColExpression::setColName(string name) {
    this->colname = name;
}

Value* ColExpression::getValue() {
    IRBuilder<>* builder = codegen::getBuilder();
    DataType dt = codegen::getAttType(index);
    Value* tupleptr = codegen::getTupleptr();
    Value *indices[1];
    indices[0] = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), (uint64_t) index);
    ArrayRef<Value*> indicesRef(indices);
    Value *data = builder->CreateLoad(builder->CreateInBoundsGEP(tupleptr, indicesRef));
    switch(dt){
        case LONG:
            return data;
        case DOUBLE:
            return builder->CreateUIToFP(data, Type::getDoubleTy(getGlobalContext()));
        case STRING:
        case DATE:
            break;
    }
    return NULL;
}
