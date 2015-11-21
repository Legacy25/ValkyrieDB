
#ifndef CDB_CODEGEN_H
#define CDB_CODEGEN_H

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "DataTypes.h"
#include "Operator.h"
#include "Schema.h"
#include "Expression.h"

using namespace llvm;
using namespace std;
using namespace valkyrie;

namespace codegen {
    void initialize(std::string);
    ExecutionEngine* compile();

    IRBuilder<>* getBuilder();
    Value* getTupleptr();
    size_t getAttPos(string colname);
    DataType getAttType(string colname);
    void scanConsume(Schema&, valkyrie::Operator* parent);
    void selectConsume(Expression* expressions, valkyrie::Operator *parent);
    void printConsume(int*);
    void setSchema(Schema *schema);
    Schema* getSchema();
}

#endif //CDB_CODEGEN_H
