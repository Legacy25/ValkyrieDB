#include <iostream>
#include <vector>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/TargetSelect.h"

#include "../include/Codegen.h"
#include "../include/Schema.h"
#include "../include/Expression.h"

using namespace llvm;
using namespace std;
using namespace valkyrie;

/* Codegen Essentials */
static LLVMContext &context = getGlobalContext();
static Module *module;
static IRBuilder<> *builder;

/* Declaration of llvm functions */
static Function *mainFunction;
static Constant *printfFunc;

/* Useful codegen variables */
static Type *int32Type = Type::getInt32Ty(context);
static Type *int32PtrType = Type::getInt32PtrTy(context);
static Type *int64Type = Type::getInt64Ty(context);
static Type *int64PtrType = Type::getInt64PtrTy(context);

static Value *longFmt;
static Value *doubleFmt;
static Value *stringFmt;
static Value *dateFmt;
static Value *newLine;

static Value *dtLong = ConstantInt::get(int32Type, LONG);
static Value *dtDouble = ConstantInt::get(int32Type, DOUBLE);
static Value *dtString = ConstantInt::get(int32Type, STRING);
static Value *dtDate = ConstantInt::get(int32Type, DATE);
static Schema *gschema;

/* Counters and useful globals for codegen */
static uint32_t nameCtr = 0;
static bool initialized = false;
static bool compiled = false;

static Value* tuplePtr;
static Value* tc;
static Value* ac;

/* Initialize codegen, sets everything up */
void codegen::initialize(string ModuleName) {
    if(initialized) {
        return;
    }

    InitializeNativeTarget();

    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    module = new Module(StringRef(ModuleName), context);
    builder = new IRBuilder<>(context);

    // Declare C standard library printf so that we can call it
    vector<Type *> printfArgsTypes({Type::getInt8PtrTy(context)});
    FunctionType *printfType = FunctionType::get(int32Type, printfArgsTypes, true);
    printfFunc = module->getOrInsertFunction("printf", printfType);

    // The main function
    FunctionType *mainFunctionType = FunctionType::get(
            int32Type,
            ArrayRef<Type *>({}),
            false
    );
    mainFunction = Function::Create(
            mainFunctionType,
            Function::ExternalLinkage,
            "llvmStart",
            module
    );

    BasicBlock *entry = BasicBlock::Create(context, "entry", mainFunction);
    builder->SetInsertPoint(entry);

    longFmt = builder->CreateGlobalStringPtr("%lu|");
    doubleFmt = builder->CreateGlobalStringPtr("%lf|");
    stringFmt = builder->CreateGlobalStringPtr("%s|");
    dateFmt = builder->CreateGlobalStringPtr("%s|");
    newLine = builder->CreateGlobalStringPtr("\n");

    initialized = true;
}

ExecutionEngine* codegen::compile() {
    if(compiled) {
        cerr << "Module has already been compiled, cannot compile again!"
            << "\nExiting..." << endl;
        exit(-1);
    }

    builder->CreateRet(ConstantInt::get(int32Type, 0));

    // This is for debugging
    module->dump();

    string error;
    ExecutionEngine *executionEngine = EngineBuilder(module).
            setErrorStr(&error).
            setUseMCJIT(true).
            create();

    executionEngine->finalizeObject();
    compiled = true;
    return executionEngine;
}



void codegen::scanConsume(Schema& schema, valkyrie::Operator *parent) {
    gschema = &schema;
    Type *ptrToPtr = PointerType::get(int64PtrType, 0);
    Value *ptr = builder->CreateIntToPtr(ConstantInt::get(int64Type, schema.getTuplePtr()), ptrToPtr);
    ac = ConstantInt::get(int32Type, schema.getAttributes().size());
    tc = ConstantInt::get(int32Type, schema.getTupleCount());

    Value *loopVar =
            builder->CreateAlloca(
                    int32Type,
                    ConstantInt::get(int32Type, 1),
                    "loopVar" + to_string(nameCtr++)
            );
    builder->CreateStore(ConstantInt::get(int32Type, 0), loopVar);

    BasicBlock *loopBody = BasicBlock::Create(context, "loopBody" + to_string(nameCtr++), mainFunction);
    builder->CreateBr(loopBody);
    builder->SetInsertPoint(loopBody);

    Value *i = builder->CreateLoad(loopVar);

    Value *indices[1];
    indices[0] = i;
    ArrayRef<Value *> indicesRef(indices);

    tuplePtr = builder->CreateLoad(builder->CreateInBoundsGEP(ptr, indicesRef));

    if (parent != NULL) {
        parent->consume();
    }

    Value *inc = builder->CreateAdd(i, ConstantInt::get(int32Type, 1));
    builder->CreateStore(inc, loopVar);
    BasicBlock *afterLoop = BasicBlock::Create(context, "afterloop"+to_string(nameCtr++), mainFunction);
    Value *cmp = builder->CreateICmpSLT(inc, tc);
    builder->CreateCondBr(cmp, loopBody, afterLoop);

    builder->SetInsertPoint(afterLoop);
}

void codegen::selectConsume(Expression *clause, valkyrie::Operator *parent){
    Value* condV = clause->getValue();
    BasicBlock *cond_true = BasicBlock::Create(context, "If"+to_string(nameCtr++), mainFunction);
    //If the tuple does not satisfy the where condition and also after returning from parent produce
    BasicBlock *merge = BasicBlock::Create(context, "continue"+to_string(nameCtr), mainFunction);
    builder->CreateCondBr(condV, cond_true, merge);
    builder->SetInsertPoint(cond_true);
    if(parent != NULL){
        parent->consume();
    }
    builder->CreateBr(merge);
    builder->SetInsertPoint(merge);
}

void codegen::printConsume(int *types) {
    std::vector<string> cols = gschema->getAttributes();
    std::vector<DataType> t = gschema->getTypes();
    cout << "column printing size " << cols.size() << endl;
    for(int i = 0; i < cols.size(); i++){
        std::cout << "printing " << cols[i] << endl;
        std::cout << "type " << t[i] << endl;
        Expression* exp = gschema->getAttrExpression(cols[i]);
        switch(t[i]){
            case DataType::DOUBLE:
                builder->CreateCall(printfFunc, vector<Value*>({doubleFmt, exp->getValue()}));
                break;
            case DataType::LONG:
                builder->CreateCall(printfFunc, vector<Value*>({longFmt, exp->getValue()}));
                break;
            case DataType::STRING:
                builder->CreateCall(printfFunc, vector<Value*>({stringFmt, exp->getValue()}));
                break;
            case DataType::DATE:
                builder->CreateCall(printfFunc, vector<Value*>({dateFmt, exp->getValue()}));
                break;
        }
        builder->CreateCall(printfFunc, vector<Value*>({newLine}));
    }
}

IRBuilder<>* codegen::getBuilder() {
    return builder;
}

size_t codegen::getAttPos(string colname){
    return gschema->getAttributePos(colname);
}

DataType codegen::getAttType(string colname){
    return gschema->getAttributeType(colname);
}

DataType codegen::getAttType(int colPos){
    return gschema->getTypes()[colPos];
}

Value* codegen::getTupleptr() {
    return tuplePtr;
}

void codegen::setSchema(Schema *schema) {
    std::cout << "size after projection change: " << schema->getAttributes().size() << std::endl;
    gschema = schema;
}

Schema* codegen::getSchema(){
    return gschema;
}