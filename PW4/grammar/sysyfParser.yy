%skeleton "lalr1.cc" /* -*- c++ -*- */
%require "3.0"
%defines
//%define parser_class_name {sysyfParser}
%define api.parser.class {sysyfParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include "SyntaxTree.h"
class sysyfDriver;
}

// The parsing context.
%param { sysyfDriver& driver }

// Location tracking
%locations
%initial-action
{
// Initialize the initial location.
@$.begin.filename = @$.end.filename = &driver.file;
};

// Enable tracing and verbose errors (which may be wrong!)
%define parse.trace
%define parse.error verbose

// Parser needs to know about the driver:
%code
{
#include "sysyfDriver.h"
#define yylex driver.lexer.yylex
}

// Tokens:
%define api.token.prefix {TOK_}

%token END
%token PLUS MINUS MULTIPLY DIVIDE MODULO        // + - * / % 
%token AND OR NOT                               // & | ~

%token ASSIGN                                   // = 
%token COMMA                                    // ,
%token SEMICOLON                                // ;
%token L_PARENTHESE R_PARENTHESE                  // ( )
%token L_BRACKET R_BRACKET                        // [ ]
%token L_BRACE R_BRACE                            // { }
%token LT GT LTE GTE EQ NEQ                     // < > <= >= == !=
%token LAND LOR                                 // && ||

%token INT FLOAT VOID                           
%token CONST
%token IF ELSE 
%token WHILE BREAK CONTINUE 
%token RETURN  

%token <int>CONST_INT
%token <float>CONST_FLOAT
%token <std::string>IDENTIFIER
%token EOL COMMENT
%token BLANK 


// Use variant-based semantic values: %type and %token expect genuine types
%type <SyntaxTree::Assembly*>CompUnit
%type <SyntaxTree::PtrList<SyntaxTree::GlobalDef>>GlobalDecl

%type <SyntaxTree::Type>BType
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDeclare
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDefList

%type <SyntaxTree::VarDef*>VarDef
%type <SyntaxTree::VarDef*>InitVarDef
%type <SyntaxTree::VarDef*>NoInitVarDef
%type <SyntaxTree::VarDef*>InitArrDef
%type <SyntaxTree::VarDef*>NoInitArrDef
%type <SyntaxTree::InitVal*>InitVal
%type <SyntaxTree::PtrList<SyntaxTree::InitVal>>InitValList

%type <SyntaxTree::FuncDef*>FuncDef
%type <SyntaxTree::FuncFParamList*>FuncFParamList
%type <SyntaxTree::FuncParam*>FuncParam
%type <SyntaxTree::FuncParam*>ValParam
%type <SyntaxTree::FuncParam*>ArrayParam

%type <SyntaxTree::BlockStmt*>Block
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItems
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItem
%type <SyntaxTree::Stmt*>UnmatchedWhileStmt
%type <SyntaxTree::Stmt*>MatchedWhileStmt
%type <SyntaxTree::Stmt*>ReturnStmt
%type <SyntaxTree::Stmt*>Stmt
%type <SyntaxTree::Stmt*>UnmatchedStmt
%type <SyntaxTree::Stmt*>MatchedStmt
%type <SyntaxTree::LVal*>LVal
%type <SyntaxTree::Expr*>Expr
%type <SyntaxTree::Literal*>Number
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>FuncRefParamList


// No %destructors are needed, since memory will be reclaimed by the
// regular destructors.

// Grammar:
%start Begin 

%%
Begin: CompUnit END {
    $1->loc = @$;
    driver.root = $1;
    return 0;
  };

CompUnit:CompUnit GlobalDecl{
	$1->global_defs.insert($1->global_defs.end(), $2.begin(), $2.end());
	$$=$1;
	} 
| GlobalDecl{
	$$=new SyntaxTree::Assembly();
	$$->global_defs.insert($$->global_defs.end(), $1.begin(), $1.end());
  };

GlobalDecl: FuncDef {
    $$ = SyntaxTree::PtrList<SyntaxTree::GlobalDef>(); 
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::GlobalDef>($1));
}
| VarDeclare {
    $$ = SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.insert($$.end(), $1.begin(), $1.end());
}
| COMMENT {};

BType:  INT { $$ = SyntaxTree::Type::INT;}
|       FLOAT {$$ = SyntaxTree::Type::FLOAT;}
|       VOID {$$ = SyntaxTree::Type::VOID;};

/* var declare */
VarDeclare: BType VarDefList SEMICOLON {   // int a = 1;
    for(auto &node : $2) {
        node->btype = $1;
        node->is_constant = false;
    }
    $$ = $2;
}
| CONST BType VarDefList SEMICOLON {    // const int a = 1;
    for(auto &node : $3) {
        node->btype = $2;
        node->is_constant = true;
    }
    $$ = $3;
};

VarDefList: VarDefList COMMA VarDef {   
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
    $$ = $1;
}
| VarDef {
    $$ = SyntaxTree::PtrList<SyntaxTree::VarDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
};

/* var parse */
VarDef: NoInitVarDef | InitVarDef | NoInitArrDef | InitArrDef;

NoInitVarDef: IDENTIFIER {                              // a;
    $$ = new SyntaxTree::VarDef();
    $$->name = $1;
    $$->is_inited = false;
    $$->loc = @$;
};

InitVarDef: IDENTIFIER ASSIGN InitVal {                 // a = 1;
    $$ = new SyntaxTree::VarDef();
    $$->name = $1;
    $$->is_inited = true;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
    $$->loc = @$;
};

NoInitArrDef: NoInitVarDef L_BRACKET Expr R_BRACKET {      //a[1]
    $1->array_length.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
}
| NoInitArrDef L_BRACKET Expr R_BRACKET {                  //a[1][1][1]...
    $1->array_length.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
};


InitArrDef: NoInitArrDef ASSIGN InitVal {               //a[1] = {0}
    $1->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
    $1->is_inited = true;
    $$ = $1;
};


InitVal: Expr {                              // a
    $$ = new SyntaxTree::InitVal();
    $$->expr = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$->loc = @$;
    $$->isExp = true;
}
| L_BRACE R_BRACE {                           //{}
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->loc = @$;
}
| L_BRACE InitValList R_BRACE {               //{1}
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList.insert($$->elementList.end(), $2.begin(), $2.end());
    $$->loc = @$;
};

InitValList: InitVal {                          //a 
    $$ = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($1));
}
| InitValList COMMA InitVal {                   // a, b, c
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($3));
    $$ = $1;
};

/* function */
FuncDef: BType IDENTIFIER L_PARENTHESE FuncFParamList R_PARENTHESE Block { // int func(int a){a = 1;}
    $$ = new SyntaxTree::FuncDef();
    $$->name = $2;
    $$->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>($4);
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($6); 
    $$->ret_type = $1;
    $$->loc = @$;
};

FuncFParamList: %empty {                                                    // 
    $$ = new SyntaxTree::FuncFParamList();
    $$->loc = @$;
}
| FuncParam {                                                               // (int a)
    $$ = new SyntaxTree::FuncFParamList();
    $$->params.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($1));
    $$->loc = @$;
}
| FuncFParamList COMMA FuncParam {                                          //(int a, int b)
    $1->params.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($3));
    $$ = $1;
};


FuncParam: ValParam | ArrayParam;

ValParam: BType IDENTIFIER {                                                // int a
    $$ = new SyntaxTree::FuncParam();
    $$->name = $2;
    $$->param_type = $1;
    $$->loc = @$;
};

ArrayParam: ValParam L_BRACKET R_BRACKET {                                    //int a[]
    $1->array_index.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>());
    $$ = $1;
}
| ValParam L_BRACKET Expr R_BRACKET {                                          //int a[1]
    $1->array_index.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
}
| ArrayParam L_BRACKET Expr R_BRACKET {                                        //in a[1][2]
    $1->array_index.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
};

/* function body */
Block: L_BRACE BlockItems R_BRACE {
    $$ = new SyntaxTree::BlockStmt();
    $$->body = $2;
    $$->loc = @$;
};

BlockItems: %empty {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
}
| BlockItems BlockItem {
    $1.insert($1.end(), $2.begin(), $2.end());
    // $$ = $1;
    $$ = std::move($1);
}

BlockItem: VarDeclare {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.insert($$.end(), $1.begin(), $1.end());
}
| Stmt {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Stmt>($1));
}
| COMMENT {};

/* SingleIfStmt: IF L_PARENTHESE Expr R_PARENTHESE Stmt {                              
    auto istmt = new SyntaxTree::IfStmt();
    istmt->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    istmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = istmt;
    $$->loc = @$;
} */
Stmt: MatchedStmt{
    $$ = $1;
}
| UnmatchedStmt{
    $$ = $1;
}
MatchedStmt: LVal ASSIGN Expr SEMICOLON {
    auto astmt = new SyntaxTree::AssignStmt();
    astmt->value = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    astmt->target = SyntaxTree::Ptr<SyntaxTree::LVal>($1);
    $$ = astmt;
    $$->loc = @$;
}
| Expr SEMICOLON {
    auto estmt = new SyntaxTree::ExprStmt();
    estmt->exp = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$ = estmt;
    $$->loc = @$;
}
| SEMICOLON {
    $$ = new SyntaxTree::EmptyStmt();
    $$->loc = @$;
}
| MatchedWhileStmt{
    $$ = $1;
}
| ReturnStmt{
    $$ = $1;
}
| Block{
    $$ = $1;
}
| IF L_PARENTHESE Expr R_PARENTHESE MatchedStmt ELSE MatchedStmt{
    auto temp = new SyntaxTree::IfStmt();
	temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
	temp->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
	temp->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
	$$ = temp;
	$$->loc = @$;
};
UnmatchedStmt: IF L_PARENTHESE Expr R_PARENTHESE Stmt{
    auto istmt = new SyntaxTree::IfStmt();
    istmt->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    istmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = istmt;
    $$->loc = @$;
}
| IF L_PARENTHESE Expr R_PARENTHESE MatchedStmt ELSE UnmatchedStmt{
    auto temp = new SyntaxTree::IfStmt();
	temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
	temp->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
	temp->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
	$$ = temp;
	$$->loc = @$;
}
| UnmatchedWhileStmt{
    $$ = $1;
}

/* %precedence ELSE; */
/* IfStmt: SingleIfStmt {
    $$ = $1;
}
| SingleIfStmt ELSE Stmt {                     //if ... else ...
    $1->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($3);
    $$ = $1;
    $$->loc = @$;
}; */

MatchedWhileStmt: WHILE L_PARENTHESE Expr R_PARENTHESE MatchedStmt {
    auto wstmt = new SyntaxTree::WhileStmt();
    wstmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    wstmt->statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = wstmt;
    $$->loc = @$;
}
| BREAK SEMICOLON {
    $$ = new SyntaxTree::BreakStmt();
    $$->loc = @$;
}
| CONTINUE SEMICOLON {
    $$ = new SyntaxTree::ContinueStmt();
    $$->loc = @$;
};
UnmatchedWhileStmt: WHILE L_PARENTHESE Expr R_PARENTHESE UnmatchedStmt {
    auto wstmt = new SyntaxTree::WhileStmt();
    wstmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    wstmt->statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = wstmt;
    $$->loc = @$;
};

ReturnStmt: RETURN Expr SEMICOLON {
    auto rstmt = new SyntaxTree::ReturnStmt();
    rstmt->ret = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = rstmt;
    $$->loc = @$;
}
| RETURN SEMICOLON {
    $$ = new SyntaxTree::ReturnStmt();
    $$->loc = @$;
};


LVal: IDENTIFIER {
    $$ = new SyntaxTree::LVal();
    $$->name = $1;
    $$->loc = @$;
}
| LVal L_BRACKET Expr R_BRACKET {
    $1->array_index.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
}

Number: CONST_INT {
    $$ = new SyntaxTree::Literal();
    $$->literal_type = SyntaxTree::Type::INT;
    $$->int_const = $1;
    $$->loc = @$;
}
| CONST_FLOAT {
    $$ = new SyntaxTree::Literal();
    $$->literal_type = SyntaxTree::Type::FLOAT;
    $$->float_const = $1;
    $$->loc = @$;
};

FuncRefParamList: Expr {
    $$ = SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($1));
}
| FuncRefParamList COMMA Expr {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
};

%left LOR LAND;
%left EQ NEQ;
%left LT LTE GT GTE;
%left PLUS MINUS;
%left MULTIPLY DIVIDE MODULO;
%precedence NOT UPLUS UMINUS;

Expr: LVal {
    $$ = $1;
}
| Number {
    $$ = $1;
}
| L_PARENTHESE Expr R_PARENTHESE {
    $$ = $2;
}
| PLUS Expr %prec UPLUS {
    auto uplus_stmt = new SyntaxTree::UnaryExpr();
    uplus_stmt->op = SyntaxTree::UnaryOp::PLUS;
    uplus_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = uplus_stmt;
    $$->loc = @$;
}
| MINUS Expr %prec UMINUS {
    auto uminus_stmt = new SyntaxTree::UnaryExpr();
    uminus_stmt->op = SyntaxTree::UnaryOp::MINUS;
    uminus_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = uminus_stmt;
    $$->loc = @$;
}
| NOT Expr {
    auto not_stmt = new SyntaxTree::UnaryCondExpr();
    not_stmt->op = SyntaxTree::UnaryCondOp::NOT;
    not_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = not_stmt;
    $$->loc = @$;
} 
| Expr LT Expr {
    auto lt_stmt = new SyntaxTree::BinaryCondExpr();
    lt_stmt->op = SyntaxTree::BinaryCondOp::LT;
    lt_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    lt_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = lt_stmt;
    $$->loc = @$;
}
| Expr GT Expr {
    auto gt_stmt = new SyntaxTree::BinaryCondExpr();
    gt_stmt->op = SyntaxTree::BinaryCondOp::GT;
    gt_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    gt_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = gt_stmt;
    $$->loc = @$;
} 
| Expr LTE Expr {
    auto lte_stmt = new SyntaxTree::BinaryCondExpr();
    lte_stmt->op = SyntaxTree::BinaryCondOp::LTE;
    lte_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    lte_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = lte_stmt;
    $$->loc = @$;
}
| Expr GTE Expr {
    auto gte_stmt = new SyntaxTree::BinaryCondExpr();
    gte_stmt->op = SyntaxTree::BinaryCondOp::GTE;
    gte_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    gte_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = gte_stmt;
    $$->loc = @$;
}
| Expr EQ Expr {
    auto eq_stmt = new SyntaxTree::BinaryCondExpr();
    eq_stmt->op = SyntaxTree::BinaryCondOp::EQ;
    eq_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    eq_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = eq_stmt;
    $$->loc = @$;
}
| Expr NEQ Expr {
    auto neq_stmt = new SyntaxTree::BinaryCondExpr();
    neq_stmt->op = SyntaxTree::BinaryCondOp::NEQ;
    neq_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    neq_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = neq_stmt;
    $$->loc = @$;
}
| Expr LAND Expr {
    auto land_stmt = new SyntaxTree::BinaryCondExpr();
    land_stmt->op = SyntaxTree::BinaryCondOp::LAND;
    land_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    land_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = land_stmt;
    $$->loc = @$;
}
| Expr LOR Expr {
    auto lor_stmt = new SyntaxTree::BinaryCondExpr();
    lor_stmt->op = SyntaxTree::BinaryCondOp::LOR;
    lor_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    lor_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = lor_stmt;
    $$->loc = @$;
}
| Expr PLUS Expr {
    auto plus_stmt = new SyntaxTree::BinaryExpr();
    plus_stmt->op = SyntaxTree::BinOp::PLUS;
    plus_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    plus_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = plus_stmt;
    $$->loc = @$;
}
| Expr MINUS Expr {
    auto minus_stmt = new SyntaxTree::BinaryExpr();
    minus_stmt->op = SyntaxTree::BinOp::MINUS;
    minus_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    minus_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = minus_stmt;
    $$->loc = @$;
}
| Expr MULTIPLY Expr {
    auto mul_stmt = new SyntaxTree::BinaryExpr();
    mul_stmt->op = SyntaxTree::BinOp::MULTIPLY;
    mul_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    mul_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = mul_stmt;
    $$->loc = @$;
}
| Expr DIVIDE Expr {
    auto div_stmt = new SyntaxTree::BinaryExpr();
    div_stmt->op = SyntaxTree::BinOp::DIVIDE;
    div_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    div_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = div_stmt;
    $$->loc = @$;
}
| Expr MODULO Expr {
    auto mod_stmt = new SyntaxTree::BinaryExpr();
    mod_stmt->op = SyntaxTree::BinOp::MODULO;
    mod_stmt->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    mod_stmt->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = mod_stmt;
    $$->loc = @$;
}
| IDENTIFIER L_PARENTHESE R_PARENTHESE {
    auto tmp = new SyntaxTree::FuncCallStmt();
    tmp->name = $1;
    $$ = tmp;
    $$->loc = @$;
}
| IDENTIFIER L_PARENTHESE FuncRefParamList R_PARENTHESE {
    auto tmp = new SyntaxTree::FuncCallStmt();
    tmp->name = $1;
    tmp->params = $3;
    $$ = tmp;
    $$->loc = @$;
};


%%

// Register errors to the driver:
void yy::sysyfParser::error (const location_type& l,
                          const std::string& m)
{
    driver.error(l, m);
}