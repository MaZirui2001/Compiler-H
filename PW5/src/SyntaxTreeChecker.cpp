#include "SyntaxTreeChecker.h"
#include <iostream>
#include <vector>

using namespace SyntaxTree;

std::vector<SymbolTable> symbol_table;
int loc_scope = 0;

void SyntaxTreeChecker::visit(Assembly& node) {
    symbol_table.push_back(SymbolTable());
    for (auto def : node.global_defs) {
        def->accept(*this);
    }
}

void SyntaxTreeChecker::visit(FuncDef& node) {
    if(symbol_table[loc_scope].item_exist(node.name)){
        err.error(node.loc, "Function duplicated.");
        exit(int(ErrorType::FuncDuplicated));
    }
    Attributes attr;
    attr.def_type = AttrType::Func;
    attr.type = node.ret_type;
    for(auto parameter:node.param_list->params){
        attr.FParamList.push_back(parameter->param_type);
    }
    symbol_table[loc_scope].add_item(node.name, attr);
    node.param_list->accept(*this);
    node.body->accept(*this);
}

void SyntaxTreeChecker::visit(BinaryExpr& node) {
    node.lhs->accept(*this);
    bool lhs_int = this->Expr_int;
    Type lhs_type = this->Expr_type;
    node.rhs->accept(*this);
    bool rhs_int = this->Expr_int;
    Type rhs_type = this->Expr_type;
    if (node.op == SyntaxTree::BinOp::MODULO) {
        if (!lhs_int || !rhs_int) {
            err.error(node.loc, "Operands of modulo should be integers.");
            exit(int(ErrorType::Modulo));
        }
    }
    this->Expr_int = lhs_int & rhs_int;
    this->Expr_type = (lhs_type == Type::FLOAT || rhs_type == Type::FLOAT) ? Type::FLOAT:Type::INT;
}

void SyntaxTreeChecker::visit(UnaryExpr& node) {
    node.rhs->accept(*this);
}

void SyntaxTreeChecker::visit(LVal& node) {
    int cur_scope;
    // lookup the node name;
    for(cur_scope = loc_scope; cur_scope >= 0 && !symbol_table[cur_scope].item_exist(node.name); cur_scope--);
    if(cur_scope < 0){
        err.error(node.loc, "Variable unknown.");
        exit(int(ErrorType::VarUnknown));
    }
    this->Expr_type = symbol_table[cur_scope].search(node.name).type;
    this->Expr_int = (this->Expr_type == Type::INT);
}

void SyntaxTreeChecker::visit(Literal& node) {
    this->Expr_type = node.literal_type;
    this->Expr_int = (node.literal_type == SyntaxTree::Type::INT);
}

void SyntaxTreeChecker::visit(ReturnStmt& node) {
    node.ret->accept(*this);
}

void SyntaxTreeChecker::visit(VarDef& node) {
    if (node.is_inited) {
        node.initializers->accept(*this);
    }
    if(symbol_table[loc_scope].item_exist(node.name)){
        err.error(node.loc, "Variable duplicated.");
        exit(int(ErrorType::VarDuplicated));
    }
    Attributes attr;
    attr.def_type = AttrType::Var;
    attr.type = node.btype;
    symbol_table[loc_scope].add_item(node.name, attr);
}

void SyntaxTreeChecker::visit(AssignStmt& node) {
    node.value->accept(*this);
}
void SyntaxTreeChecker::visit(FuncCallStmt& node) {
    int cur_scope;
    // lookup the node name;
    for(cur_scope = loc_scope; cur_scope >= 0 && !symbol_table[cur_scope].item_exist(node.name); cur_scope--);
    if(cur_scope < 0){
        err.error(node.loc, "Function unknown.");
        exit(int(ErrorType::FuncUnknown));
    }

    // check the return type
    Attributes attr = symbol_table[cur_scope].search(node.name);
    this->Expr_int = (attr.type == Type::INT);

    // check parameter
    int list_realsize = attr.FParamList.size();
    int list_nodesize = node.params.size();
    if(list_nodesize){
        int num = 0;
        for(auto parameter : node.params){
            parameter->accept(*this);
            if(num >= list_realsize || this->Expr_type != attr.FParamList[num]){
                err.error(node.loc, "FuncParams error.");
                exit(int(ErrorType::FuncParams));
            }
            num++;
        }
    }
    else if(list_nodesize != list_realsize){
        err.error(node.loc, "FuncParams error.");
        exit(int(ErrorType::FuncParams));
    }
    this->Expr_type = symbol_table[cur_scope].search(node.name).type;

}
void SyntaxTreeChecker::visit(BlockStmt& node) {
    loc_scope++;
    symbol_table.push_back(SymbolTable());
    for(auto block_stmt:node.body) 
        block_stmt->accept(*this);
    symbol_table.pop_back();
    loc_scope--;
}
void SyntaxTreeChecker::visit(EmptyStmt& node) {
    // no operation
}
void SyntaxTreeChecker::visit(SyntaxTree::ExprStmt& node) {
    node.exp->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::FuncParam& node) {
    if(symbol_table[loc_scope].item_exist(node.name)){
        err.error(node.loc, "Variable duplicated.");
        exit(int(ErrorType::VarDuplicated));
    }
    Attributes attr;
    attr.def_type = AttrType::Var;
    attr.type = node.param_type;
    symbol_table[loc_scope].add_item(node.name, attr);
}
void SyntaxTreeChecker::visit(SyntaxTree::FuncFParamList& node) {
    loc_scope++;
    if(node.params.size()){
        symbol_table.push_back(SymbolTable());
        for(auto parameter:node.params) 
            parameter->accept(*this);
    }
    loc_scope--;
}
void SyntaxTreeChecker::visit(SyntaxTree::BinaryCondExpr& node) {
    node.lhs->accept(*this);
    node.rhs->accept(*this);
    this->Expr_type = Type::BOOL;
}
void SyntaxTreeChecker::visit(SyntaxTree::UnaryCondExpr& node) {
    node.rhs->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::IfStmt& node) {
    node.if_statement->accept(*this);
    node.cond_exp->accept(*this);
    if(node.else_statement) 
        node.else_statement->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::WhileStmt& node) {
    node.cond_exp->accept(*this);
    node.statement->accept(*this);
}
void SyntaxTreeChecker::visit(SyntaxTree::BreakStmt& node) {
    // No operation
}
void SyntaxTreeChecker::visit(SyntaxTree::ContinueStmt& node) {
    // No operation
}

void SyntaxTreeChecker::visit(SyntaxTree::InitVal& node) {
    if (node.isExp) {
        node.expr->accept(*this);
    } 
    else {
        for (auto element : node.elementList) {
            element->accept(*this);
        }
    }
}