// LLVM
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/TargetSelect.h>

// standard
#include <iostream>
#include <memory>
#include <string>

using namespace std;
using namespace llvm;

struct S {
    short s;
    union U {
        bool b;
        void *v;
    };
    U u;
};

ExecutionEngine *createEngine(Module *module) {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    unique_ptr<Module> u(module);
    EngineBuilder eb(move(u));
    string errStr;
    eb.setErrorStr(&errStr);
    eb.setEngineKind(EngineKind::JIT);
    ExecutionEngine *const exec = eb.create();
    if (!exec) {
        cerr << "Could not create ExecutionEngine: " << errStr << endl;
        exit(1);
    }
    return exec;
}

/*
int main() {
    LLVMContext ctx;

    vector<Type *> members;
    members.push_back(IntegerType::get(ctx, sizeof(short) * 8));
    members.push_back(ArrayType::get(IntegerType::get(ctx, 8), sizeof(S::U)));

    StructType *const llvm_S = StructType::create(ctx, "S");
    llvm_S->setBody(members);

    Module *const module = new Module("size_test", ctx);
    ExecutionEngine *const exec = createEngine(module);
    auto layout = exec->getDataLayout();
    module->setDataLayout(layout);

    cout << "sizeof(S) = " << sizeof(S) << endl;
    cout << "allocSize(S) = " << layout.getTypeAllocSize(llvm_S) << endl;

    delete exec;
    return 0;
}
*/
Type *MakeUnionType(Module *module, LLVMContext &ctx, const vector<Type *>& um) {
    const DataLayout dl(module);
    size_t maxSize = 0;
    size_t maxAlign = 0;
    Type *maxAlignTy = nullptr;

    for (auto m : um) {
        size_t sz = dl.getTypeAllocSize(m);
        size_t al = dl.getPrefTypeAlignment(m);
        if (sz > maxSize)
            maxSize = sz;
        if (al > maxAlign) {
            maxAlign = al;
            maxAlignTy = m;
        }
    }
    vector<Type *> sv = {maxAlignTy};
    size_t mas = dl.getTypeAllocSize(maxAlignTy);
    if (mas < maxSize) {
        size_t n = maxSize - mas;
        sv.push_back(ArrayType::get(IntegerType::get(ctx, 8), n));
    }
    StructType *u = StructType::create(ctx, "U");
    u->setBody(sv);
    return u;
}

int main() {
    LLVMContext ctx;

    auto *const module = new Module("size_test", ctx);
    ExecutionEngine *const exec = createEngine(module);
    auto layout = exec->getDataLayout();
    module->setDataLayout(layout);

    vector<Type *> members;
    members.push_back(IntegerType::get(ctx, sizeof(short) * 8));
    vector<Type *> unionMembers = {PointerType::getUnqual(IntegerType::get(ctx, 8)),
                                   IntegerType::get(ctx, 1)};
    members.push_back(MakeUnionType(module, ctx, unionMembers));

    StructType *const llvm_S = StructType::create(ctx, "S");
    llvm_S->setBody(members);
    llvm_S->print(outs());

    cout << "sizeof(S) = " << sizeof(S) << endl;
    cout << "allocSize(S) = " << layout.getTypeAllocSize(llvm_S) << endl;


    delete exec;
    return 0;
}