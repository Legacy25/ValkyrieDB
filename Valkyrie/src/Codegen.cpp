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
#include "../include/DataTypes.h"

using namespace llvm;
using namespace std;
using namespace valkyrie;

/* Codegen Essentials */
static LLVMContext &context = getGlobalContext();
static Module *module;
static IRBuilder<> *builder;

/* Declaration of llvm functions */
static Function *mainFunction,
        *hashFunction,
        *joinFunction,
        *schControlFunction,
        *strCompFunction;
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

static bool llvmoutput = true;

void codegen::nollvm() {
    llvmoutput = false;
}

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

    // The hashing function
    FunctionType *hashFunctionType = FunctionType::get(
            Type::getVoidTy(context),
            ArrayRef<Type *>({int64Type, int64Type, int32Type, int64Type, int32Type}),
            false
    );
    hashFunction = Function::Create(
            hashFunctionType,
            Function::ExternalLinkage,
            "hasher",
            module
    );

    // The joining function
    FunctionType *joinFunctionType = FunctionType::get(
            Type::getVoidTy(context),
            ArrayRef<Type *>({int64Type, int64Type, int32Type, int64Type, int32Type}),
            false
    );
    joinFunction = Function::Create(
            joinFunctionType,
            Function::ExternalLinkage,
            "joiner",
            module
    );

    // The schema controller function
    FunctionType *schControlFunctionType = FunctionType::get(
            Type::getInt64Ty(context),
            ArrayRef<Type *>({int64Type, int32Type}),
            false
    );
    schControlFunction = Function::Create(
            schControlFunctionType,
            Function::ExternalLinkage,
            "schemaController",
            module
    );

    // The string compare function
    FunctionType *strCompFunctionType = FunctionType::get(
            Type::getInt32Ty(context),
            ArrayRef<Type *>({int64Type, int64Type, int32Type}),
            false
    );
    strCompFunction = Function::Create(
            strCompFunctionType,
            Function::ExternalLinkage,
            "strComparer",
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
    if(llvmoutput)
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

    BasicBlock *dataBlock = BasicBlock::Create(context, "blockloader" + to_string(nameCtr++), mainFunction);
    BasicBlock *afterDataBlock = BasicBlock::Create(context, "afterblockloader" + to_string(nameCtr++), mainFunction);
    BasicBlock *loopHead = BasicBlock::Create(context, "loophead" + to_string(nameCtr++), mainFunction);


    builder->CreateBr(dataBlock);
    builder->SetInsertPoint(dataBlock);

    Value *args[2];
    args[0] = ConstantInt::get(int64Type, (uint64_t) &schema);
    args[1] = ConstantInt::get(int64Type, LD_BLK);
    ArrayRef<Value*> argsRef(args);
    tc = builder->CreateCall(schControlFunction, argsRef);

    builder->CreateCondBr(
            builder->CreateICmpNE(tc, ConstantInt::get(int64Type, 0)),
            loopHead,
            afterDataBlock
    );

    builder->SetInsertPoint(loopHead);

    Value* gpargs[2];
    gpargs[0] = ConstantInt::get(int64Type, (uint64_t) &schema);
    gpargs[1] = ConstantInt::get(int64Type, GET_PTR);
    ArrayRef<Value *> gpargsRef(gpargs);

    Value *ptr = builder->CreateIntToPtr(builder->CreateCall(schControlFunction, gpargsRef), ptrToPtr);
    ac = ConstantInt::get(int32Type, schema.getAttributes().size());

    Value *loopVar =
            builder->CreateAlloca(
                    int64Type,
                    ConstantInt::get(int64Type, 1),
                    "loopVar" + to_string(nameCtr++)
            );
    builder->CreateStore(ConstantInt::get(int64Type, 0), loopVar);

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
    Value *cmp = builder->CreateICmpSLT(inc, tc);
    builder->CreateCondBr(cmp, loopBody, dataBlock);

    builder->SetInsertPoint(afterDataBlock);

    Value* clargs[2];
    clargs[0] = ConstantInt::get(int64Type, (uint64_t) &schema);
    clargs[1] = ConstantInt::get(int64Type, CLOSE);
    ArrayRef<Value*> clargsRef(clargs);
    builder->CreateCall(schControlFunction, clargsRef);
}



void codegen::joinConsume(Schema& schema, valkyrie::Operator *parent) {
    gschema = &schema;
    Type *ptrToPtr = PointerType::get(int64PtrType, 0);

    Value* gpargs[2];
    gpargs[0] = ConstantInt::get(int64Type, (uint64_t) &schema);
    gpargs[1] = ConstantInt::get(int64Type, GET_PTR);
    ArrayRef<Value *> gpargsRef(gpargs);

    Value* gsargs[2];
    gsargs[0] = ConstantInt::get(int64Type, (uint64_t) &schema);
    gsargs[1] = ConstantInt::get(int64Type, GET_SIZE);
    ArrayRef<Value *> gsargsRef(gsargs);

    Value *ptr = builder->CreateIntToPtr(builder->CreateCall(schControlFunction, gpargsRef), ptrToPtr);
    ac = ConstantInt::get(int32Type, schema.getAttributes().size());
    tc = builder->CreateCall(schControlFunction, gsargsRef);

    Value *loopVar =
            builder->CreateAlloca(
                    int64Type,
                    ConstantInt::get(int64Type, 1),
                    "loopVar" + to_string(nameCtr++)
            );
    builder->CreateStore(ConstantInt::get(int64Type, 0), loopVar);

    BasicBlock *condCheck = BasicBlock::Create(context, "condCheck" + to_string(nameCtr++), mainFunction);
    BasicBlock *loopBody = BasicBlock::Create(context, "loopBody" + to_string(nameCtr++), mainFunction);
    BasicBlock *afterLoop = BasicBlock::Create(context, "afterloop"+to_string(nameCtr++), mainFunction);

    builder->CreateBr(condCheck);
    builder->SetInsertPoint(condCheck);

    Value *cmp = builder->CreateICmpSLT(builder->CreateLoad(loopVar), tc);
    builder->CreateCondBr(cmp, loopBody, afterLoop);

    builder->SetInsertPoint(loopBody);

    Value *i = builder->CreateLoad(loopVar);

    Value *indices[1];
    indices[0] = i;
    ArrayRef<Value *> indicesRef(indices);

    tuplePtr = builder->CreateLoad(builder->CreateInBoundsGEP(ptr, indicesRef));

    if (parent != NULL) {
        parent->consume();
    }

    Value *inc = builder->CreateAdd(i, ConstantInt::get(int64Type, 1));
    builder->CreateStore(inc, loopVar);
    builder->CreateBr(condCheck);

    builder->SetInsertPoint(afterLoop);

    Value* clargs[2];
    clargs[0] = ConstantInt::get(int64Type, (uint64_t) &schema);
    clargs[1] = ConstantInt::get(int64Type, CLOSE);
    ArrayRef<Value*> clargsRef(clargs);
    builder->CreateCall(schControlFunction, clargsRef);
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
    for(int i = 0; i < cols.size(); i++){
        Expression* exp = gschema->getAttrExpression(cols[i]);
        switch(t[i]){
            case DataType::DOUBLE:
                builder->CreateCall(printfFunc, vector<Value*>({doubleFmt, builder->CreateBitCast(exp->getValue(), Type::getDoubleTy(context))}));
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
    }
    builder->CreateCall(printfFunc, vector<Value*>({newLine}));
}

void codegen::joinLeftConsume(JoinOperator* op) {
    vector<ColExpression*> left = op->getLeftJoinAttrs();
    std::vector<string> cols = gschema->getAttributes();

    LeafValue *tuple = new LeafValue[cols.size()]; // Don't call delete on this
    LeafValue *keys = new LeafValue[left.size()];

    Value *tempPtr = builder->CreateIntToPtr(ConstantInt::get(int64Type, (int64_t)tuple), int64PtrType);
    Value *keyPtr = builder->CreateIntToPtr(ConstantInt::get(int64Type, (int64_t)keys), int64PtrType);

    // Copy val and call hashmap on ptr
    for(uint64_t i=0; i<cols.size(); i++) {
        Expression* exp = gschema->getAttrExpression(cols[i]);

        Value *indices[1];
        indices[0] = ConstantInt::get(int32Type, i);
        ArrayRef<Value *> indicesRef(indices);

        Value *tempTupPtr = builder->CreateGEP(tempPtr, indicesRef);
        builder->CreateStore(exp->getValue(), tempTupPtr);
    }

    for(uint64_t i=0; i<left.size(); i++) {
        Value *indices[1];
        indices[0] = ConstantInt::get(int32Type, i);
        ArrayRef<Value *> indicesRef(indices);

        Value *tempKeyPtr = builder->CreateGEP(keyPtr, indicesRef);
        builder->CreateStore(left[i]->getValue(), tempKeyPtr);
    }

    Value *opPtr = ConstantInt::get(int64Type, (int64_t)op);
    Value *keySize = ConstantInt::get(int32Type, left.size());
    Value *hashArgs[] = {opPtr, keyPtr, keySize, tempPtr, ac};
    ArrayRef<Value*> hargsref(hashArgs);
    builder->CreateCall(hashFunction, hargsref);
}

void codegen::joinRightConsume(JoinOperator* op) {
    vector<ColExpression*> right = op->getRightJoinAttrs();
    std::vector<string> cols = gschema->getAttributes();

    LeafValue *tuple = new LeafValue[cols.size()]; // Don't call delete on this
    LeafValue *keys = new LeafValue[right.size()];

    Value* tempPtr = builder->CreateIntToPtr(ConstantInt::get(int64Type, (int64_t)tuple), int64PtrType);
    Value *keyPtr = builder->CreateIntToPtr(ConstantInt::get(int64Type, (int64_t)keys), int64PtrType);

    // Copy val and call hashmap on ptr
    for(uint64_t i=0; i<cols.size(); i++) {
        Expression* exp = gschema->getAttrExpression(cols[i]);

        Value *indices[1];
        indices[0] = ConstantInt::get(int32Type, i);
        ArrayRef<Value *> indicesRef(indices);

        Value *tempTupPtr = builder->CreateGEP(tempPtr, indicesRef);
        builder->CreateStore(exp->getValue(), tempTupPtr);
    }
    for(uint64_t i=0; i<right.size(); i++) {
        Value *indices[1];
        indices[0] = ConstantInt::get(int32Type, i);
        ArrayRef<Value *> indicesRef(indices);

        Value *tempKeyPtr = builder->CreateGEP(keyPtr, indicesRef);
        builder->CreateStore(right[i]->getValue(), tempKeyPtr);
    }

    Value *opPtr = ConstantInt::get(int64Type, (int64_t)op);
    Value *keySize = ConstantInt::get(int32Type, right.size());
    Value *joinArgs[] = {opPtr, keyPtr, keySize, tempPtr, ac};
    ArrayRef<Value*> jargsref(joinArgs);
    builder->CreateCall(joinFunction, jargsref);
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
    gschema = schema;
}

Schema* codegen::getSchema(){
    return gschema;
}

Value* codegen::stringCmp(Value* left, Value* right, ExprType et) {
    Value* args[3];
    args[0] = left;
    args[1] = right;
    args[2] = ConstantInt::get(int32Type, et);
    ArrayRef<Value*> argsRef(args);
    return builder->CreateICmpEQ(builder->CreateCall(strCompFunction, argsRef), ConstantInt::get(int32Type, 1));
}

extern "C"
void hasher(int64_t opPtr, int64_t keyPtr, int32_t keySize, int64_t tupPtr, int32_t ac) {
    JoinOperator *op = (JoinOperator*)opPtr;
    LeafValue *tup = (LeafValue*)tupPtr;
    LeafValue *key = (LeafValue*)keyPtr;

    vector<LeafValue> *newt = new vector<LeafValue>();

    for(int i=0; i<ac; i++) {
        newt->push_back(tup[i]);
    }
    string keyStr = "";
    for(int i=0; i<keySize; i++) {
        keyStr += to_string(key[i].l)+".";
    }
    op->hashtable[keyStr].push_back(*newt);
}

extern "C"
void joiner(int64_t opPtr, int64_t keyPtr, int32_t keySize, int64_t tupPtr, int32_t ac) {
    JoinOperator *op = (JoinOperator*)opPtr;
    LeafValue *tup = (LeafValue*)tupPtr;
    LeafValue *key = (LeafValue*)keyPtr;

    string keyStr = "";
    for(int i=0; i<keySize; i++) {
        keyStr += to_string(key[i].l)+".";
    }

    for(auto i : op->hashtable[keyStr]) {
        vector<LeafValue> *joinedTuple = new vector<LeafValue>();
        joinedTuple->insert(joinedTuple->begin(), i.begin(), i.end());
        for(int j=0; j<ac; j++) {
            joinedTuple->push_back(tup[j]);
        }
        op->getSchema()->addTuple(&((*joinedTuple)[0]));
    }
}

extern "C"
uint64_t schemaController(int64_t schemaaddr, SCHEMA_OP op) {
    Schema *schema = (Schema*)schemaaddr;
    uint64_t k;
    switch(op) {
        case GET_PTR:
            k = schema->getTuplePtr();
//            cout << "GET_PTR: " << k << endl;
            break;
        case GET_SIZE:
            k = schema->getTuples().size();
//            cout << "GET_SIZE " << k << endl;
            break;
        case LD_BLK:
            k = schema->loadBlock();
//            cout << "LD_BLK: " << k << endl;
            break;
        case CLOSE:
            k = schema->close();
//            cout << "CLOSE: " << k << endl;
            break;
        default:
            cout << "Unknown schema op in controller!" << endl;
            exit(-1);
    }
    return k;
}

extern "C"
int32_t strComparer(char* lsPtr, char* rsPtr, ExprType et) {
    switch(et) {
        case EQUALEXPRESSION:
            if (strcasecmp(lsPtr, rsPtr) == 0) return 1; else return 0;
        case NOTEQUALEXPRESSION:
            if (strcasecmp(lsPtr, rsPtr) != 0) return 1; else return 0;
        case GREATERTHANEQUALEXPRESSION:
            if (strcasecmp(lsPtr, rsPtr) >= 0) return 1; else return 0;
        case GREATERTHANEXPRESSION:
            if (strcasecmp(lsPtr, rsPtr) > 0) return 1; else return 0;
        case LESSTHANEQUALEXPRESSION:
            if (strcasecmp(lsPtr, rsPtr) <= 0) return 1; else return 0;
        case LESSTHANEXPRESSION:
            if (strcasecmp(lsPtr, rsPtr) < 0) return 1; else return 0;
        default:
            cout << "Unknown schema op in controller!" << endl;
            exit(-1);
    }
}