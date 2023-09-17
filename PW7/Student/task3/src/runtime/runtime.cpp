#include <iostream>
#include "runtime/io.h"
#include "runtime/runtime.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>

using namespace std;
using namespace llvm;

runtime_info::runtime_info(Module* module)
{
    get_int_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "getint", 
                            module); 

    get_char_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "getch", 
                            module); 
    get_float_func = Function::Create(FunctionType::get(Type::getFloatTy(module->getContext()), {}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "getfloat", 
                            module); 
    get_int_array_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), 
                            {Type::getInt32PtrTy(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "getarray", 
                            module); 
    get_float_array_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), 
                            {Type::getFloatPtrTy(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "getfarray", 
                            module); 
    put_int_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), 
                            {Type::getInt32Ty(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "putint", 
                            module);
    put_char_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), 
                            {Type::getInt32Ty(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "putch",
                            module);
    put_float_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), 
                            {Type::getFloatTy(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "putfloat",
                            module);
    put_int_array_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), 
                            {Type::getInt32Ty(module->getContext()), Type::getInt32PtrTy(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "putarray",
                            module);
    put_float_array_func = Function::Create(FunctionType::get(Type::getVoidTy(module->getContext()), 
                            {Type::getFloatTy(module->getContext()), Type::getFloatPtrTy(module->getContext())}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "putfarray",
                            module);
    current_time_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), 
                            {}, false), 
                            GlobalValue::LinkageTypes::ExternalLinkage,
                            "current_time",
                            module);
}

using namespace string_literals;


std::vector<std::tuple<std::string, void *>> runtime_info::get_runtime_symbols()
{
    return {
        make_tuple("getint"s, (void *)&::getint),
        make_tuple("getfloat"s, (void *)&::getfloat),
        make_tuple("getch"s, (void *)&::getch),
        make_tuple("getarray"s, (void *)&::getarray),
        make_tuple("getfarray"s, (void *)&::getfarray),
        make_tuple("putint"s, (void *)&::putint),
        make_tuple("putfloat"s, (void *)&::putfloat),
        make_tuple("putch"s, (void *)&::putch),
        make_tuple("putarray"s, (void *)&::putarray),
        make_tuple("putfarray"s, (void *)&::putfarray),
        make_tuple("current_time"s, (void *)&::current_time)
         };
}