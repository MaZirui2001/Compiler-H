#include<CodeGen.h>
#include<string>
#include <stack>
#include <unordered_set>

/*
    code for moving data from src list to dst list:
        - parameter `src` contains data sources, they can be constant (immediate),
          register or address on stack (address specified by base register and offset)
        - parameter `dst` contains data destinations, they can be register 
          or address on stack (address specified by base register and offset)
        - parameter `cmpop` is used to generate instructions with conditional excution method
    
    you should generate code for data moving, the method for moving single data from a source location (or const)
    to a destination location is given, parameters:
        - `src_loc` is data source location, can be constant (immediate),
          register or address on stack (address specified by base register and offset)
        - parameter `dst` contains data destinations, they can be register 
          or address on stack (address specified by base register and offset)
        - parameter `reg_tmp` is the temporary register used for data moving under situations like
            - moving data from stack location to another (this function doesn't distinguish if the location is the same)
            - moving constant value (immediate) to a stack location
            - ...
        - parameter `cmpop` is used to generate instructions with conditional excution method
    the function may generate more than one instruction.
*/

    std::string CodeGen::data_move(std::vector<IR2asm::Location*> &src,
                                   std::vector<IR2asm::Location*> &dst,
                                   std::string cmpop){
    if(!optimize){
        std::string code_pos, code_neg, code;
        /* TODO: put your code here */
        std::unordered_map<std::string, int> offset_list;
        auto temp_reg = new IR2asm::Reg(12);
        auto sp_reg = IR2asm::Reg(IR2asm::sp);
        int n = src.size();
        int offset = -8;
        for(int i = 0; i < n; i++) std::cout << src[i]->get_code() << "\t";
        std::cout << std::endl;
        for(int i = 0; i < n; i++) std::cout << dst[i]->get_code() << "\t";
        std::cout << std::endl;
        
        bool lr_flag = false;
        for(int i = 0; i < n; i++){
            if(dynamic_cast<IR2asm::RegLoc *>(src[i])){
                auto regloc = dynamic_cast<IR2asm::RegLoc *>(src[i]);
                if(regloc->is_constant()){
                    if(!dynamic_cast<IR2asm::RegLoc*>(dst[i])){
                        lr_flag = true;
                        break;
                    }
                }
                else{
                    if(!dynamic_cast<IR2asm::RegLoc*>(dst[i])){
                        lr_flag = true;
                        break;
                    }
                }
            }
            else{
                lr_flag = true;
                break;
            }
        }
        if(lr_flag){
            code += single_data_move(dynamic_cast<IR2asm::Location*>(new IR2asm::RegLoc(IR2asm::lr)), 
                            dynamic_cast<IR2asm::Location*>(new IR2asm::Regbase(sp_reg, offset)), temp_reg, cmpop);
            offset -= 4;
        }
        /* pos walk */
        for(int i = 0; i < n; i++){
            bool cflt = false;
            if(src[i]->get_code() == dst[i]->get_code()) continue;
            for(int j = i + 1; j < n; j++){
                if(src[j]->get_code() == dst[i]->get_code()){
                    cflt = true;
                    break;
                }
            }
            if(cflt){
                code += single_data_move(dst[i], dynamic_cast<IR2asm::Location *>(new IR2asm::Regbase(sp_reg, offset)), temp_reg, cmpop);
                offset_list[dst[i]->get_code()] = offset;
                offset -= 4;
            }
        }
        
        for(int i = 0; i < n; i++){
            if(offset_list.find(src[i]->get_code()) == offset_list.end()){
                code += single_data_move(src[i], dst[i], temp_reg, cmpop);
            }
            else 
                code += single_data_move(dynamic_cast<IR2asm::Location *>(new IR2asm::Regbase(sp_reg, offset_list[src[i]->get_code()])), dst[i], temp_reg, cmpop);
        }
        if(lr_flag){
            code += single_data_move(dynamic_cast<IR2asm::Location*>(new IR2asm::Regbase(sp_reg, -8)), 
                            dynamic_cast<IR2asm::Location*>(new IR2asm::RegLoc(IR2asm::lr)), temp_reg, cmpop);
        }
        std::cout << code << std::endl;
        return code;
    }
        std::string code;
        auto temp_reg = new IR2asm::Reg(12);
        auto sp_reg = IR2asm::Reg(IR2asm::sp);
        
        std::unordered_map<std::string, std::unordered_map<std::string, int>> offset_list;
        std::unordered_map<std::string, std::vector<std::string>> graph;
        std::set<std::pair<std::string, std::string>> del_edge;
        std::unordered_map<std::string, IR2asm::Location*> reg_name_map;
        std::unordered_map<std::string, std::string> parent;
        std::unordered_set<std::string> have_parent;
        std::unordered_map<std::string, bool>visited;

        int offset = -8;
        int n = src.size();
        for(int i = 0; i < n; i++) std::cout << src[i]->get_code() << "\t";
        std::cout << std::endl;
        for(int i = 0; i < n; i++) std::cout << dst[i]->get_code() << "\t";
        std::cout << std::endl;
        /* find if needs to use lr */
        bool lr_flag = false;
        for(int i = 0; i < n; i++){
            if(dynamic_cast<IR2asm::RegLoc *>(src[i])){
                auto regloc = dynamic_cast<IR2asm::RegLoc *>(src[i]);
                if(regloc->is_constant()){
                    if(!dynamic_cast<IR2asm::RegLoc*>(dst[i])){
                        lr_flag = true;
                        break;
                    }
                }
                else{
                    if(!dynamic_cast<IR2asm::RegLoc*>(dst[i])){
                        lr_flag = true;
                        break;
                    }
                }
            }
            else{
                lr_flag = true;
                break;
            }
        }
        if(lr_flag){
            code += single_data_move(dynamic_cast<IR2asm::Location*>(new IR2asm::RegLoc(IR2asm::lr)), 
                            dynamic_cast<IR2asm::Location*>(new IR2asm::Regbase(sp_reg, offset)), temp_reg, cmpop);
            offset -= 4;
        }
        /* initialize */
        for(int i = 0; i < n; i++){
            if(src[i]->get_code() == dst[i]->get_code()) continue;
            graph[src[i]->get_code()].push_back(dst[i]->get_code());
            parent[dst[i]->get_code()] = src[i]->get_code();
            have_parent.insert(dst[i]->get_code());
            reg_name_map[src[i]->get_code()] = src[i];
            reg_name_map[dst[i]->get_code()] = dst[i];
            visited[src[i]->get_code()] = false;
        }
        /* cut circuit */
        std::stack<std::pair<std::string, int>> stk;
        for(auto node : graph){
            for(auto &v : visited) v.second = false;
            stk.push(std::pair(node.first, 0));
            visited[node.first] = true;
            while(!stk.empty()){
                auto vpair = stk.top();
                auto v = vpair.first;
                if(graph.count(v) == 0 || vpair.second >= (int)graph[v].size()){
                    stk.pop();
                    continue;
                }
                for(int i = vpair.second; i < (int)graph[v].size();){
                    if(!visited[graph[v][i]]){
                        visited[graph[v][i]] = true;
                        stk.pop();
                        stk.push(std::pair(v, i+1));
                        stk.push(std::pair(graph[v][i], 0));
                        break;
                    }
                    else{
                        code += single_data_move(reg_name_map[v], 
                                dynamic_cast<IR2asm::Location*>(new IR2asm::Regbase(sp_reg, offset)), temp_reg, cmpop);
                        have_parent.erase(graph[v][i]);
                        del_edge.insert(std::pair(v, graph[v][i]));
                        offset_list[v][graph[v][i]] = offset;
                        offset -= 4;
                        graph[v].erase(graph[v].begin() + i);
                    }
                }
            }
        }
        std::vector<std::string> top;
        std::queue<std::string> topq;

        /* top sort */
        for(auto node : graph){
            if(have_parent.count(node.first) == 0) topq.push(node.first);
        }
        while(!topq.empty()){
            auto v = topq.front();
            top.push_back(v);
            topq.pop();
            for(auto v_aim : graph[v]){
                if(have_parent.count(v_aim))topq.push(v_aim);
            } 
        }
        
        /* reverse walk top seqence */
        for(auto riter = top.rbegin(); riter != top.rend(); riter++){
            auto v = *riter;
            if(have_parent.count(v)) 
                code += single_data_move(reg_name_map[parent[v]], reg_name_map[v], temp_reg, cmpop);
        }

        /* cope with deleted edges */
        for(auto vpair : del_edge){
            code += single_data_move(dynamic_cast<IR2asm::Location*> (new IR2asm::Regbase(sp_reg, offset_list[vpair.first][vpair.second])), reg_name_map[vpair.second], temp_reg, cmpop);
        }

        /* restore lr */
        if(lr_flag){
            code += single_data_move(dynamic_cast<IR2asm::Location*>(new IR2asm::Regbase(sp_reg, -8)), 
                            dynamic_cast<IR2asm::Location*>(new IR2asm::RegLoc(IR2asm::lr)), temp_reg, cmpop);
        }
        std::cout << code <<std::endl;
        return code;
    }

    std::string CodeGen::single_data_move(IR2asm::Location* src_loc,
                                 IR2asm::Location* target_loc,
                                 IR2asm::Reg *reg_tmp,
                                 std::string cmpop){
        std::string code;
        if(dynamic_cast<IR2asm::RegLoc *>(src_loc)){
            auto regloc = dynamic_cast<IR2asm::RegLoc *>(src_loc);
            if(regloc->is_constant()){
                if(dynamic_cast<IR2asm::RegLoc*>(target_loc)){
                    auto target_reg_loc = dynamic_cast<IR2asm::RegLoc*>(target_loc);
                    code += IR2asm::space;
                    code += "Ldr" + cmpop + " ";
                    code += target_reg_loc->get_code();
                    code += ", =";
                    code += std::to_string(regloc->get_constant());
                    code += IR2asm::endl;
                }
                else{
                    std::vector<int>tmp_reg;
                    tmp_reg.push_back(reg_tmp->get_id());
                    code += push_regs(tmp_reg);
                    code += IR2asm::space;
                    code += "Ldr" + cmpop + " ";
                    code += reg_tmp->get_code();
                    code += ", =";
                    code += std::to_string(regloc->get_constant());
                    code += IR2asm::endl;
                    code += IR2asm::safe_store(reg_tmp, target_loc, sp_extra_ofst, long_func, cmpop);
                    code += pop_regs(tmp_reg);
                }
            }
            else{
                if(dynamic_cast<IR2asm::RegLoc*>(target_loc)){
                    auto target_reg_loc = dynamic_cast<IR2asm::RegLoc*>(target_loc);
                    code += IR2asm::space;
                    code += "Mov" + cmpop + " ";
                    code += target_reg_loc->get_code();
                    code += ", ";
                    code += regloc->get_code();
                    code += IR2asm::endl;
                }
                else{
                    code += IR2asm::safe_store(new IR2asm::Reg(regloc->get_reg_id()),
                                                target_loc, sp_extra_ofst, long_func, cmpop);
                }
            }
        }
        else{
            auto stackloc = dynamic_cast<IR2asm::Regbase *>(src_loc);
            if(dynamic_cast<IR2asm::RegLoc*>(target_loc)){
                auto target_reg_loc = dynamic_cast<IR2asm::RegLoc*>(target_loc);
                code += IR2asm::safe_load(new IR2asm::Reg(target_reg_loc->get_reg_id()),
                                            stackloc, sp_extra_ofst, long_func, cmpop);
            }
            else{
                std::vector<int>tmp_reg;
                tmp_reg.push_back(reg_tmp->get_id());
                code += push_regs(tmp_reg);
                code += IR2asm::safe_load(reg_tmp, stackloc, sp_extra_ofst, long_func, cmpop);
                code += IR2asm::safe_store(reg_tmp, target_loc, sp_extra_ofst, long_func, cmpop);
                code += pop_regs(tmp_reg);
            }
        }
        return code;
    }