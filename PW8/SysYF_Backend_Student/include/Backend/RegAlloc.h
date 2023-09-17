#ifndef _SYSYF_REGALLOC_H_
#define _SYSYF_REGALLOC_H_

#include "Value.h"
#include "Module.h"
#include <map>
#include <set>
#include <queue>
#include <unordered_map>

class Interval;
class RegAlloc;


struct Range{
    Range(int f,int t):from(f),to(t){}
    int from;
    int to;
};
class Interval{
public:
    explicit Interval(Value* value):val(value){}
    int reg_num = -1;
    Value* val;
    std::list<Range*> range_list;
    std::list<int> position_list;
    double spill_cost;
    int range_sum = 0;
    int length = 0;
    void cal_length(){length = range_list.back()->to - range_list.front()->from;}
    void cal_range_sum();
    void add_range(int from,int to);
    void add_use_pos(int pos){position_list.push_front(pos);}

    bool covers(int id);
    bool covers(Instruction* inst);
    bool covers(Range* range);
    bool intersects(Interval* interval);
    void union_interval(Interval* interval);
};
class reg_log{
public:
    int begin = INT32_MAX;
    int end = 0;
    int range_length = 0;
    double spill_cost;
    void maintain(Interval *intv){
        begin = std::min(begin, intv->range_list.front()->from);
        end = std::max(end, intv->range_list.back()->to);
        range_length += intv->range_sum;
        spill_cost += intv->spill_cost;
    }
    void change(Interval *intv){
        begin = intv->range_list.front()->from;
        end = intv->range_list.back()->to;
        range_length += intv->range_sum;
        spill_cost = range_length / (end - begin);
    }

};



struct cmp_interval{
    bool operator()(const Interval* a, const Interval* b) const {
        auto a_from = (*(a->range_list.begin()))->from;
        auto b_from = (*(b->range_list.begin()))->from;
        if(a_from!=b_from){
            return a_from < b_from;
        }else{
            return a->val->get_name() < b->val->get_name();
        }
    }
};
struct cmp_interval_in_alloc_by_to{
    bool operator()(const Interval* a, const Interval* b) const {
        auto a_to = (a->range_list.back())->to;
        auto b_to = (b->range_list.back())->to;
        if(a_to != b_to){
            return a_to < b_to;
        }else{
            return a->val->get_name() < b->val->get_name();
        }
    }
};

const int priority[] = {
        5,//r0
        4,//r1
        3,//r2
        2,//r3
        12,//r4
        11,//r5
        10,//r6
        9,//r7
        8,//r8
        7,//r9
        6,//r10
        -1,//r11
        1//r12
};


struct cmp_reg {
    bool operator()(const int reg1,const int reg2)const{
#ifdef DEBUG
        assert(reg1>=0&&reg1<=11&&reg2<=11&&reg2>=0&&"invalid reg id");
#endif
        return priority[reg1] > priority[reg2];
    }
};

const std::vector<int> all_reg_id = {0,1,2,3,4,5,6,7,8,9,10,12};

class RegAllocDriver{
public:
    explicit RegAllocDriver(Module* m, bool _optimize):module(m), optimize(_optimize){}
    void compute_reg_alloc();
    std::map<Value*, Interval*>& get_reg_alloc_in_func(Function* f){return reg_alloc[f];}
    std::list<BasicBlock*>& get_bb_order_in_func(Function* f){return bb_order[f];}
private:
    std::map<Function*, std::list<BasicBlock*>> bb_order;
    std::map<Function*,std::map<Value*,Interval*>> reg_alloc;
    Module* module;
    bool optimize;
};
struct cost_cmp{
    bool operator()(Interval *a, Interval *b){
        return a->range_list.back()->to - a->range_list.front()->from < b->range_list.back()->to - b->range_list.front()->from;
    }
};
struct spill_cost_cmp{
    bool operator()(Interval *a, Interval *b){
        return a->spill_cost < b->spill_cost;
    }
};


/*****************Linear Scan Register Allocation*******************/

class RegAlloc{
public:
    explicit RegAlloc(Function* f, bool _optimize):func(f), optimize(_optimize){}
    void execute();
    std::map<Value*,Interval*>& get_reg_alloc(){return val2Inter;}
    std::list<BasicBlock*>& get_block_order(){return block_order;}
    std::set<Interval *> active_intervals;
    std::priority_queue<Interval*, std::vector<Interval*>, cost_cmp> grd_heap;
    std::map<unsigned int, std::set<Interval*>> reg_ma;
    std::map<unsigned int, reg_log> reg_cost;
    std::priority_queue<Interval*, std::vector<Interval*>, spill_cost_cmp> spill_list;
private:
    
    void compute_block_order();
    void number_operations();
    void build_intervals();
    void walk_intervals();
    void set_unused_reg_num();
    void get_dfs_order(BasicBlock* bb,std::set<BasicBlock*>& visited);
    void add_interval(Interval* interval){interval_list.insert(interval);}
    void add_reg_to_pool(Interval* inter);
    bool try_alloc_free_reg(int &id);
    void reg_adjust(Interval *inter);
    
    std::set<int, cmp_reg> unused_reg_id = {all_reg_id.begin(),all_reg_id.end()};
    Interval* current = nullptr;
    std::map<Value*, Interval*> val2Inter;
    Function* func;
    bool optimize = false;
    std::list<BasicBlock*> block_order={};
    std::set<Interval*,cmp_interval> interval_list;
    
};

#endif // _SYSYF_REGALLOC_H_