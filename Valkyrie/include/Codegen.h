
#ifndef CDB_CODEGEN_H
#define CDB_CODEGEN_H

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "DataTypes.h"
#include "Operator.h"

using namespace llvm;
using namespace std;

namespace codegen {
    void initialize(string);
    ExecutionEngine* compile();

    void scanConsume(const TupPtr, valkyrie::Operator* parent);
    void selectConsume(std::vector<std::string> expressions, valkyrie::Operator *parent);
    void printConsume(int*);
}

#endif //CDB_CODEGEN_H
