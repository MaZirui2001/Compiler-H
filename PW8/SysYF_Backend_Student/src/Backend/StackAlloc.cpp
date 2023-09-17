#include <CodeGen.h>
#include <RegAlloc.h>
#include <string>

/*
    Code for allocating spaces for the varibles in the function (specified by parameter)
        - parameter `fun`: pointer to the function to be handled
    
    You need to implement this function, according to the stack frame layout stated in the documentation
    
    Here are some tips:
        - Be careful with the order of the arguments to be passed through stack. They are pushed in reversed order,
          which means that a parameter in front will be closer to the stack pointer. 
          For example, the fourth parameter will be closer to the stack pointer than the fifth parameter.
        - Don't forget to reserve the space for callee-saved registers. The number of callee-saved registers
          can be obtained with `used_reg.second.size()`. 
        - ...
*/

int CodeGen::stack_space_allocation(Function *fun)
{
    
    int size = 0;

    // std::map<Value *, Interval *> CodeGen::reg_map
    auto _reg_map = &reg_map;   // Hint: use this to get register for values
    

    // std::map<Value *, IR2asm::Regbase *> CodeGen::stack_map
    stack_map.clear();          // You need to fill in this container to finish allocation

    // std::vector<IR2asm::Regbase *> CodeGen::arg_on_stack
    arg_on_stack.clear();       // You need to maintain this information, the order is the same as parameter

    /* TODO：put your code here */
    int offset = 0;
    
    /* caller saved reg */
    if(have_func_call){
      offset += caller_saved_reg_num * reg_size;
    }

    /* temp_reg_store_num */
    if(have_temp_reg){
      offset += temp_reg_store_num * reg_size;
    }

    /* local array */
    for(auto inst : fun->get_entry_block()->get_instructions()){
      
      if(inst->is_alloca()){
        stack_map[inst] = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset);
        offset += inst->get_type()->get_size();
      }
    }

    /* statistic */
    int stack_time = 0;
    for(auto inter : *_reg_map){
        if(inter.second->reg_num == -1){
          stack_map[inter.first] = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset);
          offset += inter.first->get_type()->get_size();
          for(auto range : inter.second->range_list){
            stack_time += range->to - range->from;
          }
        }
    }
    std::cout << "@" << fun->get_name() << ": " << std::endl;
    std::cout << "total time on stack: " << stack_time << std::endl;

    /* alooc space for args */
    int arg_on_stack_offset = (used_reg.second.size() + 1)* reg_size;
    for(auto arg : fun->get_args()){
      /* arg by reg */
      if(arg->get_arg_no() < 4){
        stack_map[arg] = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), offset);
        offset += reg_size;
      }
      /* arg by stack */
      else if(arg->get_arg_no() == 4){
        arg_on_stack_offset += offset;
        arg_on_stack.push_back(new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), arg_on_stack_offset));
        stack_map[arg] = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), arg_on_stack_offset);
        arg_on_stack_offset += arg->get_type()->get_size();
        

      }
      else{
        arg_on_stack.push_back(new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), arg_on_stack_offset));
        stack_map[arg] = new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp), arg_on_stack_offset);
        arg_on_stack_offset += arg->get_type()->get_size();
      }
    }
    size += offset;

    return size;
}