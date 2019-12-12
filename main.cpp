#include "KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace llvm;
using namespace llvm::orc;

////////////////////////////////////////////////////////////////////////////////
static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, Type *tp,
                                          const std::string &VarName) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(tp, nullptr, VarName);
}

struct Foo {
    int32_t a;
    int32_t b;
    int32_t c;
};

Type *getType() {
    vector<Type *> tps;
    tps.push_back(Type::getInt32Ty(TheContext));
    tps.push_back(Type::getInt32Ty(TheContext));
    tps.push_back(Type::getInt32Ty(TheContext));
    StructType *tp = StructType::create(TheContext, tps, "foo_type");
    tp->print(errs());
    return tp;
}

static void InitializeModuleAndPassManager() {
    // Open a new module.
    TheModule = std::make_unique<Module>("my cool jit", TheContext);
    TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

    // Create a new pass manager attached to it.
    TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

    // Promote allocas to registers.
    TheFPM->add(createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->add(createInstructionCombiningPass());
    // Reassociate expressions.
    TheFPM->add(createReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->add(createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->add(createCFGSimplificationPass());

    TheFPM->doInitialization();
}

Value *getInt(int val, LLVMContext &ctx) {
    Type *i32_type = IntegerType::getInt32Ty(ctx);
    auto ret = ConstantInt::get(i32_type, val, true);
    return ret;
}

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    TheJIT = std::make_unique<KaleidoscopeJIT>();
    InitializeModuleAndPassManager();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string funcName = "_func_";

    auto inputType = getType();
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(TheContext), {inputType}, false);

    Function *TheFunction =
        Function::Create(FT, Function::ExternalLinkage, funcName, TheModule.get());

    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    auto Arg = TheFunction->arg_begin();
    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, inputType, Arg->getName());
    Builder.CreateStore(Arg, Alloca);
    auto ptrb =
        Builder.CreateGEP(inputType, Alloca, {getInt(0, TheContext), getInt(1, TheContext)});
    auto valb = Builder.CreateLoad(ptrb);

    Builder.CreateRet(valb);

    verifyFunction(*TheFunction);
    TheFunction->print(outs());

    TheJIT->addModule(std::move(TheModule));
    auto ExprSymbol = TheJIT->findSymbol(funcName);
    assert(ExprSymbol && "Function not found");

    auto FP = (int32_t(*)(Foo))(intptr_t)cantFail(ExprSymbol.getAddress());

    Foo f{};
    f.a = 2;
    f.b = 5;
    f.c = 7;

    cout << FP(f) << endl;
}
