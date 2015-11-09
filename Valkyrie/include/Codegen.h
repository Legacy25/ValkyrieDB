
#ifndef CDB_CODEGEN_H
#define CDB_CODEGEN_H

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "DataTypes.h"
#include "Operator.h"
#include "Schema.h"

using namespace llvm;
using namespace std;
using namespace valkyrie;

namespace codegen {
    void initialize(std::string);
    ExecutionEngine* compile();

    IRBuilder<>* getBuilder();

    void scanConsume(const Schema&, valkyrie::Operator* parent);
    void selectConsume(std::vector<std::string> expressions, valkyrie::Operator *parent);
    void printConsume(int*);
}

#endif //CDB_CODEGEN_H
