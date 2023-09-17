#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG  // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl;  // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module_ptr)

#define CONST_FP(num) \
    ConstantFloat::get(num, module_ptr) // 得到常数值的表示,方便后面多次用到

int main(){
    auto builder = new IRBuilder();
    auto module = builder->getModule();
    auto module_ptr = module.get();
    auto scope = builder->getScope();
    auto stmt_builder = builder->getStmtBuilder();

    Type *Int32Type = Type::get_int32_type(module_ptr);
    // Type *Int32Ptr = Type::get_int32_ptr_type(module_ptr);

    auto getarray = scope->find("getarray", true);
    auto putarray = scope->find("putarray", true);
    auto *arraytype_a = ArrayType::get(Int32Type, 100000);
    auto zero_initializer = ConstantZero::get(Int32Type, module_ptr);

    auto a = GlobalVariable::create("a", module_ptr, arraytype_a, false, zero_initializer);
    /* partition */

    auto partition_fun_ty = FunctionType::get(Int32Type, {Int32Type, Int32Type});
    auto partition_fun = Function::create(partition_fun_ty, "partition", module_ptr);
    auto bb = BasicBlock::create(module_ptr, "entry", partition_fun);
    stmt_builder->set_insert_point(bb);

    // get args
    auto p_alloca = stmt_builder->create_alloca(Int32Type);
    auto r_alloca = stmt_builder->create_alloca(Int32Type);
    std::vector<Value *> args;  
    for (auto arg = partition_fun->arg_begin(); arg != partition_fun->arg_end(); arg++) {
        args.push_back(*arg);
    }
    stmt_builder->create_store(args[0], p_alloca);
    stmt_builder->create_store(args[1], r_alloca);
    // int x = a[r];
    auto x_alloca = stmt_builder->create_alloca(Int32Type);
    auto r_load = stmt_builder->create_load(r_alloca);
    auto ar_grp = stmt_builder->create_gep(a, {CONST_INT(0), r_load});
    auto ar_load = stmt_builder->create_load(ar_grp);
    stmt_builder->create_store(ar_load, x_alloca);
    // int i = p;
    auto i_alloca = stmt_builder->create_alloca(Int32Type);
    auto p_load = stmt_builder->create_load(p_alloca);
    stmt_builder->create_store(p_load, i_alloca);
    // int j = p;
    auto j_alloca = stmt_builder->create_alloca(Int32Type);
    stmt_builder->create_store(p_load, j_alloca);
    // int tmp
    auto tmp_alloca = stmt_builder->create_alloca(Int32Type);
    stmt_builder->create_store(CONST_INT(0), tmp_alloca);
    // while branch
    auto wcondBB = BasicBlock::create(module_ptr, "while_cond", partition_fun);
    auto wtrueBB = BasicBlock::create(module_ptr, "while_true", partition_fun);
    auto wfalseBB = BasicBlock::create(module_ptr, "while_false", partition_fun);
    // if branch
    auto itrueBB = BasicBlock::create(module_ptr, "if_true", partition_fun);
    auto ifalseBB = BasicBlock::create(module_ptr, "if_false", partition_fun);

    stmt_builder->create_br(wcondBB);
    // while cond: j < r
    stmt_builder->set_insert_point(wcondBB);
    auto j_load = stmt_builder->create_load(j_alloca);
    r_load = stmt_builder->create_load(r_alloca);
    auto icmp = stmt_builder->create_icmp_lt(j_load, r_load);
    stmt_builder->create_cond_br(icmp, wtrueBB, wfalseBB);
    // while true
    stmt_builder->set_insert_point(wtrueBB);
    auto x_load = stmt_builder->create_load(x_alloca);
    j_load = stmt_builder->create_load(j_alloca);
    auto aj_gep = stmt_builder->create_gep(a, {CONST_INT(0), j_load});
    auto aj_load = stmt_builder->create_load(aj_gep);
    icmp = stmt_builder->create_icmp_gt(x_load, aj_load);
    stmt_builder->create_cond_br(icmp, itrueBB, ifalseBB);
    // if trueBB
    stmt_builder->set_insert_point(itrueBB);
    auto i_load = stmt_builder->create_load(i_alloca);
    j_load = stmt_builder->create_load(j_alloca);
    auto ai_gep = stmt_builder->create_gep(a, {CONST_INT(0), i_load});
    aj_gep = stmt_builder->create_gep(a, {CONST_INT(0), j_load});
    // swap
    auto ai_load = stmt_builder->create_load(ai_gep);
    stmt_builder->create_store(ai_load, tmp_alloca);
    aj_load = stmt_builder->create_load(aj_gep);
    stmt_builder->create_store(aj_load, ai_gep);
    auto tmp_load = stmt_builder->create_load(tmp_alloca);
    stmt_builder->create_store(tmp_load, aj_gep);
    // i = i + 1
    i_load = stmt_builder->create_load(i_alloca);
    auto iadd = stmt_builder->create_iadd(i_load, CONST_INT(1));
    stmt_builder->create_store(iadd, i_alloca);
    stmt_builder->create_br(ifalseBB);
    // if falseBB
    stmt_builder->set_insert_point(ifalseBB);
    // j = j + 1
    j_load = stmt_builder->create_load(j_alloca);
    iadd = stmt_builder->create_iadd(j_load, CONST_INT(1));
    stmt_builder->create_store(iadd, j_alloca);
    stmt_builder->create_br(wcondBB);
    // while false 
    stmt_builder->set_insert_point(wfalseBB);
    i_load = stmt_builder->create_load(i_alloca);
    r_load = stmt_builder->create_load(r_alloca);
    ai_gep = stmt_builder->create_gep(a, {CONST_INT(0), i_load});
    auto ar_gep = stmt_builder->create_gep(a, {CONST_INT(0), r_load});
    // swap
    ai_load = stmt_builder->create_load(ai_gep);
    stmt_builder->create_store(ai_load, tmp_alloca);
    ar_load = stmt_builder->create_load(ar_gep);
    stmt_builder->create_store(ar_load, ai_gep);
    tmp_load = stmt_builder->create_load(tmp_alloca);
    stmt_builder->create_store(tmp_load, ar_gep);
    // return i
    i_load = stmt_builder->create_load(i_alloca);
    stmt_builder->create_ret(i_load);


    /* qsort */
    auto qsort_fun_ty = FunctionType::get(Int32Type, {Int32Type, Int32Type});
    auto qsort_fun = Function::create(qsort_fun_ty, "qsort", module_ptr);
    bb = BasicBlock::create(module_ptr, "entry", qsort_fun);
    stmt_builder->set_insert_point(bb);
    auto q_alloca = stmt_builder->create_alloca(Int32Type);
    // get args
    // a_alloca = stmt_builder->create_alloca(Int32Ptr);
    p_alloca = stmt_builder->create_alloca(Int32Type);
    r_alloca = stmt_builder->create_alloca(Int32Type);
    std::vector<Value *> argsq;  
    for (auto arg = qsort_fun->arg_begin(); arg != qsort_fun->arg_end(); arg++) {
        argsq.push_back(*arg);
    }
    stmt_builder->create_store(args[0], p_alloca);
    stmt_builder->create_store(args[1], r_alloca);

    itrueBB = BasicBlock::create(module_ptr, "ifq_true", qsort_fun);
    ifalseBB = BasicBlock::create(module_ptr, "ifq_false", qsort_fun);
    p_load = stmt_builder->create_load(p_alloca);
    r_load = stmt_builder->create_load(r_alloca);
    icmp = stmt_builder->create_icmp_lt(p_load, r_load);
    stmt_builder->create_cond_br(icmp, itrueBB, ifalseBB);

    // ifq true
    stmt_builder->set_insert_point(itrueBB);

    // q = partition(a, p, r)
    p_load = stmt_builder->create_load(p_alloca);
    r_load = stmt_builder->create_load(r_alloca);
    auto q_load = stmt_builder->create_call(partition_fun, {p_load, r_load});
    stmt_builder->create_store(q_load, q_alloca);

    // qsort(a, p, q - 1);
    auto q_minus1 = stmt_builder->create_isub(q_load, CONST_INT(1));
    stmt_builder->create_call(qsort_fun, {p_load, q_minus1});

    // qsort(a, q + 1, r);
    auto q_add1 = stmt_builder->create_iadd(q_load, CONST_INT(1));
    stmt_builder->create_call(qsort_fun, {q_add1, r_load});
    stmt_builder->create_br(ifalseBB);
    // ifq false
    stmt_builder->set_insert_point(ifalseBB);
    stmt_builder->create_ret(CONST_INT(0));

    auto main_fun_ty = FunctionType::get(Int32Type, {});
    auto main_fun = Function::create(main_fun_ty, "main", module_ptr);
    bb = BasicBlock::create(module_ptr, "entry", main_fun);
    stmt_builder->set_insert_point(bb);
    // int n
    auto n_alloca = stmt_builder->create_alloca(Int32Type);
    // n = getarray(a)
    auto a_gep = stmt_builder->create_gep(a, {CONST_INT(0), CONST_INT(0)});
    auto n_load = stmt_builder->create_call(getarray, {a_gep});
    stmt_builder->create_store(n_load, n_alloca);
    // qsort(a, 0, n-1)
    auto n_minus1 = stmt_builder->create_isub(n_load, CONST_INT(1));
    stmt_builder->create_call(qsort_fun, {CONST_INT(0), n_minus1});
    stmt_builder->create_call(putarray, {n_load, a_gep});
    // return 0;
    stmt_builder->create_ret(CONST_INT(0));
    std::cout << module_ptr->print();
    return 0;
}