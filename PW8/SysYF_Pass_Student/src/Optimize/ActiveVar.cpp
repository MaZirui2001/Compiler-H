#include "ActiveVar.h"
#include <fstream>

#include <algorithm>

void ActiveVar::execute() {
    //  请不要修改该代码。在被评测时不要在中间的代码中重新调用set_print_name
    module->set_print_name();
    //

    for (auto &func : this->module->get_functions()){
        if (func->get_basic_blocks().empty()){
            continue;
        }
        func_ = func;
        std::map<BasicBlock *, std::map<Value *, std::set<BasicBlock *>>> active_origin;
        bool continue_flag = true;
        while (continue_flag){
            continue_flag = false;
            std::set<Value *> def_list;
            for (BasicBlock *block : func->get_basic_blocks()){
                /* in_before, out_before: used to judge when to break, if no change occured */
                std::set<Value *> in_before;
                std::set<Value *> out_before;
                /* def_list: maintain the def list*/
                def_list.clear();
                /* in before: maintain */
                in_before.clear();
                in_before.insert(block->get_live_in().begin(), block->get_live_in().end());
                /* clear the in_set: make it easy for maintain in_set */ 
                block->get_live_in().clear();
                /* flag: now is phi instruction*/
                bool phi_flag = true;
                /* traverse every instruction: for maintaining def_list */
                for (auto inst : block->get_instructions()) {
                    if(!inst->is_phi()) phi_flag = false;
                    uint32_t oprands_size = inst->get_operands().size();
                    for (int i = 0; i < oprands_size; i++) {
                        /* traverse every operand */
                        auto oprand = inst->get_operands()[i];
                        if (
                           (oprand->get_type()->is_array_type()     ||
                            oprand->get_type()->is_integer_type()   ||
                            oprand->get_type()->is_float_type()     ||
                            oprand->get_type()->is_pointer_type())  && 
                            !(dynamic_cast<Constant *>(oprand))     &&
                            !(dynamic_cast<GlobalVariable *>(oprand))
                        ){
                            /* case: not in define_list */
                            if (phi_flag == true || def_list.find(oprand) == def_list.end() ) {
                                /* cope with phi inst: op1 relys on bb1, op2 relys on bb2 */
                                if (inst->is_phi()) 
                                    active_origin[block][oprand].insert((BasicBlock *)inst->get_operands()[i + 1]);
                                else 
                                    active_origin[block][oprand].insert(block);
                                
                                block->get_live_in().insert(oprand);
                            }
                        }
                    }
                    /* not void: must have defination */
                    if(!inst->is_void())
                        def_list.insert(inst);
                    
                }
                /* OUT - def */
                for (auto out : block->get_live_out()) {
                    if (def_list.find(out) == def_list.end())
                        block->get_live_in().insert(out);
                }
                /* out_before: maintain*/
                out_before.clear();
                out_before.insert(block->get_live_out().begin(), block->get_live_out().end());
                /* clear out*/
                block->get_live_out().clear();
                /* traverse every succ_block */
                for (auto succ_block : block->get_succ_basic_blocks()){
                    for (auto succ_in : succ_block->get_live_in()){
                        /* the active must comes from correct block */
                        if (active_origin[succ_block][succ_in].count(succ_block) || active_origin[succ_block][succ_in].count(block)){
                            block->get_live_out().insert(succ_in);
                            /* if the succ is not def, it should coherit the active */
                            if(!def_list.count(succ_in)) 
                                active_origin[block][succ_in].insert(block);
                        }
                    }
                }
                if (in_before != block->get_live_in() || out_before != block->get_live_out()) 
                    continue_flag = true;
            }
        }
    }
    //  请不要修改该代码，在被评测时不要删除该代码
    dump();
    //
    return;
}

void ActiveVar::dump() {
    std::fstream f;
    f.open(avdump, std::ios::out);
    for (auto &func: module->get_functions()) {
        for (auto &bb: func->get_basic_blocks()) {
            f << bb->get_name() << std::endl;
            auto &in = bb->get_live_in();
            auto &out = bb->get_live_out();
            auto sorted_in = sort_by_name(in);
            auto sorted_out = sort_by_name(out);
            f << "in:\n";
            for (auto in_v: sorted_in) {
                f << in_v->get_name() << " ";
            }
            f << "\n";
            f << "out:\n";
            for (auto out_v: sorted_out) {
                f << out_v->get_name() << " ";
            }
            f << "\n";
        }
    }
    f.close();
}

bool ValueCmp(Value* a, Value* b) {
    return a->get_name() < b->get_name();
}

std::vector<Value*> sort_by_name(std::set<Value*> &val_set) {
    std::vector<Value*> result;
    result.assign(val_set.begin(), val_set.end());
    std::sort(result.begin(), result.end(), ValueCmp);
    return result;
}