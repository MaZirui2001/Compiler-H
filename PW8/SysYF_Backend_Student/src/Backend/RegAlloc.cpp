//
// Created by cjb on 7/24/21.
//

#include "RegAlloc.h"
#include <iostream>
#include <set>
#include <float.h>
#include <algorithm>

void Interval::add_range(int from, int to) {
    if(range_list.empty()){
        range_list.push_front(new Range(from, to));
        return;
    }
    auto top_range = *range_list.begin();
    if(from>=top_range->from && from<=top_range->to){
        top_range->to = to > top_range->to?to:top_range->to;
    }else if(from < top_range->from){
        if(to <= top_range->to && to>=top_range->from){
            top_range->from = from;
        }else{
            auto new_range = new Range(from,to);
            range_list.push_front(new_range);
        }
    }else{
        auto new_range = new Range(from,to);
        range_list.push_front(new_range);
    }
}
void Interval::cal_range_sum(){
    range_sum = 0;
    for(auto range:range_list){
        range_sum += range->to - range->from;
    }
}
bool Interval::covers(int id){
    for(auto range:range_list){
        if(range->from<=id&&range->to>id){
            return true;
        }
    }
    return false;
}
bool Interval::covers(Range* range){
    for(auto r : range_list){
        if(range->from < r->to && range->to > r->from){
            return true;
        }
    }
    return false;
}

bool Interval::covers(Instruction* inst){
    return covers(inst->get_id());
}

bool Interval::intersects(Interval *interval) {
    auto target_it = range_list.begin();
    auto with_it = interval->range_list.begin();
    while(with_it!=interval->range_list.end()&&target_it!=range_list.end()){
        auto target_range = *target_it;
        auto with_range = *with_it;
        if(target_range->to<=with_range->from){
            target_it++;
            continue;
        }else if(with_range->to<=target_range->from){
            with_it++;
            continue;
        }else{
            return true;
        }
    }
    return false;
}


struct cmp_range{
    bool operator()(const Range* a,const Range* b) const {
        return a->from > b->from;
    }
};

void Interval::union_interval(Interval *interval) {
    std::priority_queue<Range*, std::vector<Range*>, cmp_range> all_range;
    for(auto range:range_list){
        all_range.push(range);
    }
    for(auto range:interval->range_list){
        all_range.push(range);
    }
    if(all_range.empty()){
        return;
    }
    range_list.clear();
    auto cur_range = all_range.top();
    all_range.pop();
    while(!all_range.empty()){
        auto merge_range = all_range.top();
        all_range.pop();
        if(merge_range->from > cur_range->to){
            range_list.push_back(cur_range);
            cur_range = merge_range;
        }else{
            cur_range->to = cur_range->to >= merge_range->to?cur_range->to:merge_range->to;
        }
    }
    range_list.push_back(cur_range);
}


void RegAllocDriver::compute_reg_alloc() {
    for(auto func:module->get_functions()){
        if(func->get_basic_blocks().empty()){
            continue;
        }else{
#ifdef DEBUG
            std::cerr << "function " << func->get_name() << std::endl;
#endif
            auto allocator = new RegAlloc(func, optimize);
            allocator->execute();
            reg_alloc[func] = allocator->get_reg_alloc();
            bb_order[func] = allocator->get_block_order();
        }
    }
#ifdef DEBUG
    std::cerr << "finish reg alloc\n";
#endif
}

void RegAlloc::execute() {
    compute_block_order();
    number_operations();
    build_intervals();
    walk_intervals();
    set_unused_reg_num();
}

void RegAlloc::compute_block_order() {
    block_order.clear();
    auto entry = func->get_entry_block();
    std::set<BasicBlock*> visited = {};
    get_dfs_order(entry,visited);
}

void RegAlloc::get_dfs_order(BasicBlock *bb, std::set<BasicBlock *> &visited) {
    visited.insert(bb);
    block_order.push_back(bb);
    auto children = bb->get_succ_basic_blocks();
    for(auto child : children){
        auto is_visited = visited.find(child);
        if(is_visited == visited.end()){
            get_dfs_order(child,visited);
        }
    }
}

void RegAlloc::number_operations() {
    int next_id = 0;
    for(auto bb:block_order){
        auto instrs = bb->get_instructions();
        for(auto instr:instrs){
            instr->set_id(next_id);
            next_id += 2;
        }
    }
}

void RegAlloc::build_intervals() {
    for(auto iter = block_order.rbegin();iter != block_order.rend();iter++)
    {
        auto bb = *iter;
        auto instrs = bb->get_instructions();
        int block_from = (*(instrs.begin()))->get_id();
        auto lst_instr = instrs.rbegin();
        int block_to = (*(lst_instr))->get_id() + 2;
        for(auto opr:bb->get_live_out()){
            if((!dynamic_cast<Instruction*>(opr) && !dynamic_cast<Argument*>(opr))||dynamic_cast<AllocaInst *>(opr)){
                continue;
            }
            if(val2Inter.find(opr)==val2Inter.end()){
                auto new_interval = new Interval(opr);
                val2Inter[opr] = new_interval;
            }
            val2Inter[opr]->add_range(block_from,block_to);
        }
        for(auto instr_iter = instrs.rbegin();instr_iter!=instrs.rend();instr_iter++){
            auto instr = *instr_iter;

            if(!instr->is_void()){
                if(dynamic_cast<AllocaInst *>(instr))continue;
                if(val2Inter.find(instr)==val2Inter.end()){
                    auto new_interval = new Interval(instr);
                    new_interval->add_range(block_from,block_to);
                    val2Inter[instr] = new_interval;
                }
                auto cur_inter = val2Inter[instr];
                auto top_range = *(cur_inter->range_list.begin());
                top_range->from = instr->get_id();
                cur_inter->add_use_pos(instr->get_id());
            }

            if(instr->is_phi()){//analyze
                continue;
            }

            for(auto opr:instr->get_operands()){
                if((!dynamic_cast<Instruction*>(opr) && !dynamic_cast<Argument*>(opr))||dynamic_cast<AllocaInst *>(opr)){
                    continue;
                }
                if(val2Inter.find(opr)==val2Inter.end()){
                    auto new_interval = new Interval(opr);
                    val2Inter[opr] = new_interval;
                    new_interval->add_range(block_from,instr->get_id()+2);
                    new_interval->add_use_pos(instr->get_id());
                }
                else{
                    auto cur_inter = val2Inter[opr];
                    cur_inter->add_range(block_from,instr->get_id()+2);
                    cur_inter->add_use_pos(instr->get_id());
                }
            }
        }
    }
    for(auto pair:val2Inter){
#ifdef DEBUG
        std::cerr << "op:" <<pair.first->get_name() << std::endl;
#endif
        add_interval(pair.second);
#ifdef DEBUG
        for(auto range:pair.second->range_list){
            std::cerr << "from: " << range->from << " to: " << range->to << std::endl;
        }
#endif
    }
}

void RegAlloc::walk_intervals() {
    /*you need to finish this function*/
    /* greedy allocation */
    if(optimize){

        /* initialize */
        int n = interval_list.size();
        for(auto interval : interval_list){
            interval->cal_length();
            interval->cal_range_sum();
            interval->spill_cost = interval->range_sum / interval->length;
            grd_heap.push(interval);
        }

        /* try to fill in an allocated reg */
        for(int i = 0; i < n; i++){
            auto intv = grd_heap.top();
            grd_heap.pop();
            for(auto &int_list : reg_ma){
                bool cflt = false;
                for(auto &intval : int_list.second){
                    if(intv->intersects(intval)){
                        cflt = true;
                        break;
                    }
                }
                if(!cflt){
                    intv->reg_num = int_list.first;
                    int_list.second.insert(intv);
                    reg_cost[int_list.first].maintain(intv);
                    break;
                }
            }
            if(intv->reg_num >= 0) {
                continue;
            }
            else spill_list.push(intv);

            /* try to allocate free reg */
            int id = -1;
            if(try_alloc_free_reg(id) == false){
                /* find the least cost reg */
                int min_reg = -1;
                double min_cost = DBL_MAX;
                for(auto &reg : reg_cost){
                    if(reg.second.spill_cost < min_cost){
                        min_reg = reg.first;
                        min_cost = reg.second.spill_cost;
                    }
                }
                /* spill intervals */
                if(intv->spill_cost < min_cost) continue;
                for(auto intval : reg_ma[min_reg]){
                    intval->reg_num = -1;
                    spill_list.push(intval);
                }
                reg_cost[min_reg].change(intv);
                reg_ma[min_reg].clear();
                reg_ma[min_reg].insert(intv);
                intv->reg_num = min_reg;
            }
            else{
                /* alloca free reg */
                intv->reg_num = id;
                reg_ma[id].insert(intv);
                reg_cost[id].change(intv);
            }
        }
        /* second chance */
        while(!spill_list.empty()){
            auto intv = spill_list.top();
            spill_list.pop();
            for(auto &int_list : reg_ma){
                bool cflt = false;
                for(auto &intval : int_list.second){
                    if(intv->intersects(intval)){
                        cflt = true;
                        break;
                    }
                }
                if(!cflt){
                    intv->reg_num = int_list.first;
                    int_list.second.insert(intv);
                    reg_cost[int_list.first].maintain(intv);
                    break;
                }
            }
            if(intv->reg_num >= 0) continue;
            int min_reg = -1;
            double min_cost = DBL_MAX;
            for(auto &reg : reg_cost){
                if(reg.second.spill_cost < min_cost){
                    min_reg = reg.first;
                    min_cost = reg.second.spill_cost;
                }
            }
            if(intv->spill_cost < min_cost) continue;
            for(auto intval : reg_ma[min_reg]){
                intval->reg_num = -1;
            }
            reg_cost[min_reg].change(intv);
            reg_ma[min_reg].clear();
            reg_ma[min_reg].insert(intv);
            intv->reg_num = min_reg;
        }
        return;    
    }


    active_intervals.clear();
    for(auto current_it=interval_list.begin();current_it!=interval_list.end();current_it++){
        current = *current_it;
        /* alloc and free */
        auto cur_loc = current->range_list.front()->from;
        /* walk the interval by time */
        for(auto active_iter = active_intervals.begin(); active_iter != active_intervals.end();){
            auto active_inter = *active_iter;
            if(active_inter->range_list.back()->to <= cur_loc){
                add_reg_to_pool(active_inter);
                active_intervals.erase(active_iter++);
                continue;
            }
            active_iter++;
        }
        /* try to alloca free reg */
        int id = -1 ;
        if(try_alloc_free_reg(id) == false) {
            reg_adjust(current);
        }
        else {
            current->reg_num = id;
            active_intervals.insert(current);
            
        }
    }
}

void RegAlloc::reg_adjust(Interval * inter){
    double cur_cost =  (inter->range_list.back()->to - inter->range_list.front()->from) / inter->range_list.size();
    double max_cost = cur_cost;
    Interval *max_inter = inter;
    /* find a max cost in cuurent active list */
    for(auto iter = active_intervals.begin(); iter != active_intervals.end(); iter++){
        auto intval = *iter;
        int num = 0;
        int n = intval->range_list.size();
        for(auto range : intval->range_list){
            if(range->from <= intval->range_list.front()->from && range->to >= intval->range_list.front()->from){
                auto cost = (intval->range_list.back()->to - range->from) * 1.0 / (n - num);
                if(cost > max_cost){
                    max_cost = cost;
                    max_inter = intval;
                }
                break;
            }
            num++;
        }
    }
    if(max_inter != inter){
        inter->reg_num = max_inter->reg_num;
        max_inter->reg_num = -1;
        active_intervals.insert(inter);
        active_intervals.erase(max_inter);
    }
    
}

void RegAlloc::set_unused_reg_num() {
    std::set<int> tmp;
    tmp.insert(unused_reg_id.begin(), unused_reg_id.end());
    func->set_unused_reg_num(tmp);
}

void RegAlloc::add_reg_to_pool(Interval * inter){
    unused_reg_id.insert(inter->reg_num);
}

bool RegAlloc::try_alloc_free_reg(int &id) {
    if(unused_reg_id.empty()) return false;
        
    id = *unused_reg_id.begin();
    unused_reg_id.erase(unused_reg_id.begin());
    return true;

}