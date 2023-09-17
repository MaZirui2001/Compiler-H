#ifndef _SYSYF_SYNTAX_TREE_CHECKER_H_
#define _SYSYF_SYNTAX_TREE_CHECKER_H_

#include <cassert>
#include "ErrorReporter.h"
#include "SyntaxTree.h"

class SyntaxTreeChecker : public SyntaxTree::Visitor {
   public:
    SyntaxTreeChecker(ErrorReporter& e) : err(e) {}
    virtual void visit(SyntaxTree::Assembly& node) override;
    virtual void visit(SyntaxTree::FuncDef& node) override;
    virtual void visit(SyntaxTree::BinaryExpr& node) override;
    virtual void visit(SyntaxTree::UnaryExpr& node) override;
    virtual void visit(SyntaxTree::LVal& node) override;
    virtual void visit(SyntaxTree::Literal& node) override;
    virtual void visit(SyntaxTree::ReturnStmt& node) override;
    virtual void visit(SyntaxTree::VarDef& node) override;
    virtual void visit(SyntaxTree::AssignStmt& node) override;
    virtual void visit(SyntaxTree::FuncCallStmt& node) override;
    virtual void visit(SyntaxTree::BlockStmt& node) override;
    virtual void visit(SyntaxTree::EmptyStmt& node) override;
    virtual void visit(SyntaxTree::ExprStmt& node) override;
    virtual void visit(SyntaxTree::FuncParam& node) override;
    virtual void visit(SyntaxTree::FuncFParamList& node) override;
    virtual void visit(SyntaxTree::BinaryCondExpr& node) override;
    virtual void visit(SyntaxTree::UnaryCondExpr& node) override;
    virtual void visit(SyntaxTree::IfStmt& node) override;
    virtual void visit(SyntaxTree::WhileStmt& node) override;
    virtual void visit(SyntaxTree::BreakStmt& node) override;
    virtual void visit(SyntaxTree::ContinueStmt& node) override;
    virtual void visit(SyntaxTree::InitVal& node) override;

   private:
    ErrorReporter& err;
    bool Expr_int;
    SyntaxTree::Type Expr_type;
};

enum class ErrorType {
    Accepted = 0,
    Modulo,
    VarUnknown,
    VarDuplicated,
    FuncUnknown,
    FuncDuplicated,
    FuncParams
};
enum class AttrType{
    Var = 0,
    Func
};

struct Attributes{
    AttrType def_type;
    SyntaxTree::Type type;
    std::vector<SyntaxTree::Type> FParamList;
};

class SymbolTable{

    private:
    std::unordered_map<std::string, Attributes> sym_table;
    
    public:
    SymbolTable(){}

    // search the location of name
    Attributes search(std::string name){
        return sym_table[name];
    }

    // add item to table
    void add_item(std::string name, Attributes attr){
        sym_table[name] = attr;
    }

    // check if an item is in table 
    bool item_exist(std::string name){
        return !(sym_table.find(name) == sym_table.end());
    }
    ~SymbolTable(){}
};

#endif  // _SYSYF_SYNTAX_TREE_CHECKER_H_