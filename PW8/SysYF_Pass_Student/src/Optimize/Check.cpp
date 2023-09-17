#include "Check.h"
#include "Module.h"
#include "logging.hpp"
#include <unordered_map>
#include <algorithm>
void Check::execute() {
    //TODO write your IR Module checker here.
    std::unordered_map<BasicBlock*, std::set<Value*>> def_block;
    std::unordered_map<BasicBlock*, std::set<Value*>> pre_def;
    std::set<Value*> all_def;
    for (auto &func : this->module->get_functions()){
        if (func->get_basic_blocks().empty())
            continue;
        std::cout << func->get_name() << std::endl;
        /* succ and pre block check */
        for(auto &block : func->get_basic_blocks()){
            for(auto &pre_block : block->get_pre_basic_blocks()){
                auto pre_succ_blocks = pre_block->get_succ_basic_blocks();
                if(find(pre_succ_blocks.begin(), pre_succ_blocks.end(), block) == pre_succ_blocks.end()){
                    std::cout << "a block's pre-block's succ-block is not itself!" << std::endl;
                    std::cout << "error point: " << block->get_name() <<", pre-block is " << pre_block->get_name() << std::endl;
                    exit(0);
                }
            }
            for(auto &succ_block : block->get_succ_basic_blocks()){
                auto succ_pre_blocks = succ_block->get_pre_basic_blocks();
                if(find(succ_pre_blocks.begin(), succ_pre_blocks.end(), block) == succ_pre_blocks.end()){
                    std::cout << "a block's succ-block's pre-block is not itself!" << std::endl;
                    std::cout << "error point: " << block->get_name() <<", succ-block is " << succ_block->get_name() << std::endl;
                    exit(0);
                }
            }
        }
        std::cout << "Pass succ-pre block check." << std::endl;
        
        for(auto &block : func->get_basic_blocks()){
            /* check def-use link */
            for(auto inst : block->get_instructions()){
                for(auto oprand : inst->get_operands()){
                    auto iter = std::find_if(oprand->get_use_list().begin(), oprand->get_use_list().end(), [inst](Use use){ return use.val_== inst; });
                    if(iter == oprand->get_use_list().end()){
                        std::cout << "use-def is not valid!" << std::endl;
                        std::cout << "error point: " << block->get_name() << ", unvalid inst is " << inst->get_instr_op_name() << 
                        ", unvalid operand is " << oprand->get_name() << std::endl;
                        exit(0);
                    }

                }
                if(!inst->is_void()) {
                    def_block[block].insert(inst);
                    all_def.insert(inst);
                }
            }
            /* check last instruction of block */
            auto last_inst = block->get_instructions().back();
            if(!last_inst->is_ret() && !last_inst->is_br()){
                std::cout << "last instruction of block is not valid!" << std::endl;
                std::cout << "error point: "<< block->get_name() << ", last instruction is " << last_inst->get_instr_op_name() << std::endl;
                exit(0);
            }
        }
        std::cout << "Pass use-def check." << std::endl;
        std::cout << "Pass last-instruction check. " << std::endl;
        
        bool continue_flag = true;
        while(continue_flag){
            
            continue_flag = false;
            std::set<Value*> def_before;
            for(auto &block : func->get_basic_blocks()){
                def_before.clear();
                def_before.insert(pre_def[block].begin(), pre_def[block].end());
                std::set<Value *>pre_intersection;
                pre_intersection.insert(all_def.begin(), all_def.end());
                pre_def[block].clear();
                for(auto &pre_block : block->get_pre_basic_blocks()){
                    std::set<Value*>tmp;
                    std::set_union(pre_def[pre_block].begin(), pre_def[pre_block].end(), 
                                        def_block[pre_block].begin(), def_block[pre_block].end(), std::inserter(tmp, tmp.begin()));
                    std::set_intersection(pre_intersection.begin(), pre_intersection.end(), 
                                        tmp.begin(), tmp.end(), std::inserter(pre_def[block], pre_def[block].begin()));
                    
                }
                if(def_before != pre_def[block]) continue_flag = true;
            }
        }
        for(auto &block : func->get_basic_blocks()){
            LOG_ENABLE
            LOG(INFO) << block->get_name();
            for(auto oprand : pre_def[block]){
                LOG(INFO) << oprand->get_name();
            }
            LOG_POP
            std::set<Value*> cur_def;
            cur_def.clear();
            for(auto &inst : block->get_instructions()){
                int opsize = inst->get_num_operand();
                for(int i = 0; i < opsize; i++){
                    auto oprand = inst->get_operands()[i];
                    if (
                        (oprand->get_type()->is_array_type()     ||
                        oprand->get_type()->is_integer_type()   ||
                        oprand->get_type()->is_float_type()     ||
                        oprand->get_type()->is_pointer_type())  && 
                        !(dynamic_cast<Constant *>(oprand))     &&
                        !(dynamic_cast<GlobalVariable *>(oprand))
                    ){
                        if(!inst->is_phi()){
                            if(cur_def.count(oprand) == 0 && pre_def[block].count(oprand) == 0  \
                                && find(func->get_args().begin(), func->get_args().end(), oprand) == func->get_args().end()){
                                std::cout << "use before define!" << std::endl;
                                std::cout << "error point: " << block->get_name() << ", instruction is " << inst->get_instr_op_name() << ", oprand is " << oprand->get_name() << std::endl;
                                exit(0);
                            }
                        }
                        else{
                            auto pre_block = std::find(block->get_pre_basic_blocks().begin(), block->get_pre_basic_blocks().end(), inst->get_operands()[i+1]);
                            if(pre_def[*pre_block].count(oprand) == 0 && def_block[*pre_block].count(oprand) == 0){
                                std::cout << "use before define!" << std::endl;
                                std::cout << "error point: " << block->get_name() << ", instruction is " << inst->get_instr_op_name() << ", oprand is " << oprand->get_name() << std::endl;
                                exit(0);
                            }
                        }

                    }
                }
                if(!inst->is_void()) cur_def.insert(inst);
            }
        }
        std::cout << "Pass quote check." << std::endl << std::endl;
    }
}