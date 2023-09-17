#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <memory>
#include "runtime/runtime.h"

using namespace llvm; // 指明命名空间为llvm


#define CONST_INT(num) \
    ConstantInt::get(context, APInt(32, num)) //得到常数值的表示,方便后面多次用到
#define CONST_FP(num) \
    ConstantFP::get(context, APFloat(32, num))

llvm::Module *qs_generator(LLVMContext &context, runtime_info *&runtime)
{
    IRBuilder<> builder(context);
    llvm::Type *INT32 = llvm::Type::getInt32Ty(context); 
    llvm::Type *FLOAT = llvm::Type::getFloatTy(context);
    llvm::Type *VOID = llvm::Type::getVoidTy(context);
    llvm::Type *FLOAT_PTR = llvm::Type::getFloatPtrTy(context);
    llvm::Type *ARRAY_F = llvm::ArrayType::get(FLOAT, 4096);
    auto module = new llvm::Module("qs", context);
    runtime = new runtime_info(module);

    auto qs_fun = Function::Create(llvm::FunctionType::get(VOID, {FLOAT_PTR, INT32, INT32}, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage, "qs", module);
    auto bb = BasicBlock::Create(context, "entry", qs_fun);
    builder.SetInsertPoint(bb);
    
    auto low_alloca = builder.CreateAlloca(INT32);
    auto high_allloca = builder.CreateAlloca(INT32);
    std::vector<Value *> args;
    for(auto arg = qs_fun->arg_begin(); arg != qs_fun->arg_end(); arg++){
        args.push_back(arg);
    }
    auto arr = args[0];
    builder.CreateStore(args[1], low_alloca);
    builder.CreateStore(args[2], high_allloca);

    auto itruebb1 = BasicBlock::Create(context, "if1_true", qs_fun);
    auto ifalsebb1 = BasicBlock::Create(context, "if1_false", qs_fun);
    auto wcondbb1 = BasicBlock::Create(context, "while1_cond", qs_fun);
    auto wtruebb1 = BasicBlock::Create(context, "while1_true", qs_fun);
    auto wfalsebb1 = BasicBlock::Create(context, "while1_false", qs_fun);
    auto wcondbb2 = BasicBlock::Create(context, "while2_cond", qs_fun);
    auto wcondbb2_and = BasicBlock::Create(context, "while2_cond_and", qs_fun);
    auto wtruebb2 = BasicBlock::Create(context, "while2_true", qs_fun);
    auto wfalsebb2 = BasicBlock::Create(context, "while2_false", qs_fun);
    auto itruebb2 = BasicBlock::Create(context, "if2_true", qs_fun);
    auto ifalsebb2 = BasicBlock::Create(context, "if2_false", qs_fun);
    auto wcondbb3 = BasicBlock::Create(context, "while3_cond", qs_fun);
    auto wtruebb3 = BasicBlock::Create(context, "while3_true", qs_fun);
     auto wcondbb3_and = BasicBlock::Create(context, "while3_cond_and", qs_fun);
    auto wfalsebb3 = BasicBlock::Create(context, "while3_false", qs_fun);
    auto itruebb3 = BasicBlock::Create(context, "if3_true", qs_fun);
    auto ifalsebb3 = BasicBlock::Create(context, "if3_false", qs_fun);

    auto l_load = builder.CreateLoad(low_alloca);
    auto h_load = builder.CreateLoad(high_allloca);
    auto icmp = builder.CreateICmpSLT(l_load, h_load);
    builder.CreateCondBr(icmp, itruebb1, ifalsebb1);
    // if true
    builder.SetInsertPoint(itruebb1);
    auto i_alloca = builder.CreateAlloca(INT32);
    auto j_alloca = builder.CreateAlloca(INT32);
    auto k_alloca = builder.CreateAlloca(FLOAT);
    // i = low, j = high
    l_load = builder.CreateLoad(low_alloca);
    h_load = builder.CreateLoad(high_allloca);
    builder.CreateStore(l_load, i_alloca);
    builder.CreateStore(h_load, j_alloca);
    // k = arr[low]
    auto arrl_gep = builder.CreateGEP(FLOAT, arr, {l_load});
    auto k_load = builder.CreateLoad(arrl_gep);
    builder.CreateStore(k_load, k_alloca);
    builder.CreateBr(wcondbb1);

    // while i < j
    builder.SetInsertPoint(wcondbb1);
    auto i_load = builder.CreateLoad(i_alloca);
    auto j_load = builder.CreateLoad(j_alloca);
    icmp = builder.CreateICmpSLT(i_load, j_load);
    builder.CreateCondBr(icmp, wcondbb2, wfalsebb1);

    // i < j
    builder.SetInsertPoint(wcondbb2);
    i_load = builder.CreateLoad(i_alloca);
    j_load = builder.CreateLoad(j_alloca);
    icmp = builder.CreateICmpSLT(i_load, j_load);
    builder.CreateCondBr(icmp, wcondbb2_and, wfalsebb2);

    // a[j] >= k
    builder.SetInsertPoint(wcondbb2_and);
    j_load = builder.CreateLoad(j_alloca);
    auto arrj_gep = builder.CreateGEP(FLOAT, arr, {j_load});
    auto arrj_load = builder.CreateLoad(arrj_gep);
    k_load = builder.CreateLoad(k_alloca);
    icmp = builder.CreateFCmpOGE(arrj_load, k_load);
    builder.CreateCondBr(icmp, wtruebb2, wfalsebb2);

    // j--
    builder.SetInsertPoint(wtruebb2);
    j_load = builder.CreateLoad(j_alloca);
    auto j_load_minus = builder.CreateSub(j_load, CONST_INT(1));
    builder.CreateStore(j_load_minus, j_alloca);
    builder.CreateBr(wcondbb2);

    // wfalsebb2
    builder.SetInsertPoint(wfalsebb2);
    i_load = builder.CreateLoad(i_alloca);
    j_load = builder.CreateLoad(j_alloca);
    icmp = builder.CreateICmpSLT(i_load, j_load);
    builder.CreateCondBr(icmp, itruebb2, wcondbb3);

    // itruebb2
    builder.SetInsertPoint(itruebb2);
    i_load = builder.CreateLoad(i_alloca);
    j_load = builder.CreateLoad(j_alloca);
    arrj_gep = builder.CreateGEP(FLOAT, arr, {j_load});
    auto arri_gep = builder.CreateGEP(FLOAT, arr, {i_load});
    arrj_load = builder.CreateLoad(arrj_gep);
    builder.CreateStore(arrj_load, arri_gep);
    auto i_add1 = builder.CreateAdd(i_load, CONST_INT(1));
    builder.CreateStore(i_add1, i_alloca);
    builder.CreateBr(wcondbb3);

    // i < j
    builder.SetInsertPoint(wcondbb3);
    i_load = builder.CreateLoad(i_alloca);
    j_load = builder.CreateLoad(j_alloca);
    icmp = builder.CreateICmpSLT(i_load, j_load);
    builder.CreateCondBr(icmp, wcondbb3_and, wfalsebb3);

    // a[i] < k
    builder.SetInsertPoint(wcondbb3_and);
    i_load = builder.CreateLoad(i_alloca);
    arri_gep = builder.CreateGEP(FLOAT, arr, {i_load});
    auto arri_load = builder.CreateLoad(arri_gep);
    k_load = builder.CreateLoad(k_alloca);
    icmp = builder.CreateFCmpOLT(arri_load, k_load);
    builder.CreateCondBr(icmp, wtruebb3, wfalsebb3);

    // i++
    builder.SetInsertPoint(wtruebb3);
    i_load = builder.CreateLoad(i_alloca);
    i_add1 = builder.CreateAdd(i_load, CONST_INT(1));
    builder.CreateStore(i_add1, i_alloca);
    builder.CreateBr(wcondbb3);

    // wfalsebb3
    builder.SetInsertPoint(wfalsebb3);
    i_load = builder.CreateLoad(i_alloca);
    j_load = builder.CreateLoad(j_alloca);
    icmp = builder.CreateICmpSLT(i_load, j_load);
    builder.CreateCondBr(icmp, itruebb3, wcondbb1);

    // itruebb3
    builder.SetInsertPoint(itruebb3);
    i_load = builder.CreateLoad(i_alloca);
    j_load = builder.CreateLoad(j_alloca);
    arri_gep = builder.CreateGEP(FLOAT, arr, {i_load});
    arrj_gep = builder.CreateGEP(FLOAT, arr, {j_load});
    arri_load = builder.CreateLoad(arri_gep);
    builder.CreateStore(arri_load, arrj_gep);
    auto j_sub1 = builder.CreateSub(j_load, CONST_INT(1));
    builder.CreateStore(j_sub1, j_alloca);
    builder.CreateBr(wcondbb1);

    //wfalsebb1
    builder.SetInsertPoint(wfalsebb1);
    i_load = builder.CreateLoad(i_alloca);
    k_load = builder.CreateLoad(k_alloca);
    arri_gep = builder.CreateGEP(FLOAT, arr, {i_load});
    builder.CreateStore(k_load, arri_gep);

    l_load = builder.CreateLoad(low_alloca);
    h_load = builder.CreateLoad(high_allloca);
    i_load = builder.CreateLoad(i_alloca);
    i_add1 = builder.CreateAdd(i_load, CONST_INT(1));
    auto i_sub1 = builder.CreateSub(i_load, CONST_INT(1));
    builder.CreateCall(qs_fun, {arr, l_load, i_sub1});
    builder.CreateCall(qs_fun, {arr, i_add1, h_load});
    builder.CreateBr(ifalsebb1);

    // ifalsebb1
    builder.SetInsertPoint(ifalsebb1);
    builder.CreateRet(CONST_INT(0));
    
    // main
    auto main_fun = Function::Create(llvm::FunctionType::get(INT32, false),
                                GlobalValue::LinkageTypes::ExternalLinkage,
                                "main", module);
    bb = BasicBlock::Create(context, "entry", main_fun);
    builder.SetInsertPoint(bb);
    auto array_alloca = builder.CreateAlloca(ARRAY_F);
    arr = builder.CreateGEP(ARRAY_F, array_alloca, {CONST_INT(0), CONST_INT(0)});
    auto array_num = builder.CreateAlloca(INT32);
    auto start = builder.CreateAlloca(INT32);
    auto end = builder.CreateAlloca(INT32);
    auto array_num_load = builder.CreateCall(runtime->get_float_array_func, {arr});
    builder.CreateStore(array_num_load, array_num);
    auto start_load = builder.CreateCall(runtime->current_time_func);
    builder.CreateStore(start_load, start);
    auto array_num_minus1 = builder.CreateSub(array_num_load, CONST_INT(1));
    builder.CreateCall(qs_fun, {arr, CONST_INT(0), array_num_minus1});
    auto end_load = builder.CreateCall(runtime->current_time_func);
    builder.CreateStore(end_load, end);
    builder.CreateCall(runtime->put_float_array_func, {array_num_load, arr});
    auto time = builder.CreateSub(end_load, start_load);
    builder.CreateCall(runtime->put_int_func, {time});
    builder.CreateCall(runtime->put_char_func, {CONST_INT(' ')});
    builder.CreateCall(runtime->put_char_func, {CONST_INT('m')});
    builder.CreateCall(runtime->put_char_func, {CONST_INT('s')});
    builder.CreateCall(runtime->put_char_func, {CONST_INT('\n')});
    builder.CreateRet(CONST_INT(0));
    builder.ClearInsertionPoint();
    return module;
}
