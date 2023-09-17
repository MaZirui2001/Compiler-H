%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include <algorithm>
# include <iostream>
# include <map>
# include "sysyfDriver.h"
# include "sysyfParser.h"
%}

%{
#if defined __clang__
# define CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#endif

// Clang and ICC like to pretend they are GCC.
#if defined __GNUC__ && !defined __clang__ && !defined __ICC
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 6.4.0, 7.3.0 with -O3.
#if defined GCC_VERSION && 600 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

// This code uses Flex's C backend, yet compiles it as C++.
// So expect warnings about C style casts and NULL.
#if defined CLANG_VERSION && 500 <= CLANG_VERSION
# pragma clang diagnostic ignored "-Wold-style-cast"
# pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#elif defined GCC_VERSION && 407 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define FLEX_VERSION (YY_FLEX_MAJOR_VERSION * 100 + YY_FLEX_MINOR_VERSION)

// Old versions of Flex (2.5.35) generate an incomplete documentation comment.
//
//  In file included from src/scan-code-c.c:3:
//  src/scan-code.c:2198:21: error: empty paragraph passed to '@param' command
//        [-Werror,-Wdocumentation]
//   * @param line_number
//     ~~~~~~~~~~~~~~~~~^
//  1 error generated.
#if FLEX_VERSION < 206 && defined CLANG_VERSION
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

// Old versions of Flex (2.5.35) use 'register'.  Warnings introduced in
// GCC 7 and Clang 6.
#if FLEX_VERSION < 206
# if defined CLANG_VERSION && 600 <= CLANG_VERSION
#  pragma clang diagnostic ignored "-Wdeprecated-register"
# elif defined GCC_VERSION && 700 <= GCC_VERSION
#  pragma GCC diagnostic ignored "-Wregister"
# endif
#endif

#if FLEX_VERSION < 206
# if defined CLANG_VERSION
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wdocumentation"
#  pragma clang diagnostic ignored "-Wshorten-64-to-32"
#  pragma clang diagnostic ignored "-Wsign-conversion"
# elif defined GCC_VERSION
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
#endif
%}

%option noyywrap nounput noinput batch debug

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns(yyleng);
%}

/* Regex abbreviations: */

Identifier                  [_a-zA-Z][a-zA-Z0-9_]*
OIntConst                   ("0"[0-7]*)
DIntConst                   ([1-9][0-9]*)
HIntConst                   ("0"[xX][0-9a-fA-F]*)
FloatConst                  (([0-9]+([eE][-+]?[0-9]+))|(([0-9]*"."[0-9]+)|([0-9]+"."))([eE][-+]?[0-9]+)?)
Blank                       [ \t\r]
NewLine                     [\n]
InLineComment	            "//"[^\n]*
CrossLineComment	        "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"

%%
 /* keyword */
int         {return yy::sysyfParser::make_INT(loc);}
float       {return yy::sysyfParser::make_FLOAT(loc);}
void        {return yy::sysyfParser::make_VOID(loc);}
const       {return yy::sysyfParser::make_CONST(loc);}

while       {return yy::sysyfParser::make_WHILE(loc);}
break       {return yy::sysyfParser::make_BREAK(loc);}
continue    {return yy::sysyfParser::make_CONTINUE(loc);}

if          {return yy::sysyfParser::make_IF(loc);}
else        {return yy::sysyfParser::make_ELSE(loc);}

return      {return yy::sysyfParser::make_RETURN(loc);}

 /* token and action */
"="         {return yy::sysyfParser::make_ASSIGN(loc);}
","         {return yy::sysyfParser::make_COMMA(loc);}
";"         {return yy::sysyfParser::make_SEMICOLON(loc);}

"+"         {return yy::sysyfParser::make_PLUS(loc);}
"-"         {return yy::sysyfParser::make_MINUS(loc);}
"*"         {return yy::sysyfParser::make_MULTIPLY(loc);}
"/"         {return yy::sysyfParser::make_DIVIDE(loc);}
"%"         {return yy::sysyfParser::make_MODULO(loc);}
"&"         {return yy::sysyfParser::make_AND(loc);}
"|"         {return yy::sysyfParser::make_OR(loc);}

"("         {return yy::sysyfParser::make_L_PARENTHESE(loc);}
"["         {return yy::sysyfParser::make_L_BRACKET(loc);}
"]"         {return yy::sysyfParser::make_R_BRACKET(loc);}
")"         {return yy::sysyfParser::make_R_PARENTHESE(loc);}
"{"         {return yy::sysyfParser::make_L_BRACE(loc);}
"}"         {return yy::sysyfParser::make_R_BRACE(loc);}

"&&"        {return yy::sysyfParser::make_LAND(loc);}
"||"        {return yy::sysyfParser::make_LOR(loc);}
"!"         {return yy::sysyfParser::make_NOT(loc);}

"<"         {return yy::sysyfParser::make_LT(loc);}
">"         {return yy::sysyfParser::make_GT(loc);}
"<="        {return yy::sysyfParser::make_LTE(loc);}
">="        {return yy::sysyfParser::make_GTE(loc);}
"=="        {return yy::sysyfParser::make_EQ(loc);}
"!="        {return yy::sysyfParser::make_NEQ(loc);}




{Blank}+            {
    loc.step();
}
{NewLine}+          {
    loc.lines(yyleng); 
    loc.step();
}

{OIntConst}         {
    std::string oct_digits = yytext;
    int num = stoi(oct_digits, 0, 8);
    return yy::sysyfParser::make_CONST_INT(num, loc);
}

{DIntConst}         {
    std::string dec_digits = yytext;
    int num = std::stoi(dec_digits, 0, 10);
    return yy::sysyfParser::make_CONST_INT(num, loc);
}

{HIntConst}         {
    int num = 0;
    std::string hex_digits = yytext;
    int len = hex_digits.size();
    for(int i = 2; i < len; i++){       /* skip 0x */
        if     (hex_digits[i] >= '0' && hex_digits[i] <= '9') num = num * 16 + hex_digits[i] - '0';
        else if(hex_digits[i] >= 'a' && hex_digits[i] <= 'f') num = num * 16 + hex_digits[i] - 87; 
        else if(hex_digits[i] >= 'A' && hex_digits[i] <= 'F') num = num * 16 + hex_digits[i] - 55;
        else exit(-1);
    }
    return yy::sysyfParser::make_CONST_INT(num, loc);
}

{FloatConst}        {
    std::string float_digits = yytext;
    float num = std::stof(float_digits);
    return yy::sysyfParser::make_CONST_FLOAT(num, loc);
}

{Identifier}        {
    return yy::sysyfParser::make_IDENTIFIER(yytext, loc);
}

{InLineComment}     {
    // loc.lines(1); 
    loc.step();
}

{CrossLineComment}  {
    int final_line_loc = -1;
    std::string comment_txt = yytext;
    int len = comment_txt.size();
    for(int i = 0; i < len; i++) {
        if(comment_txt[i] == '\n'){
            loc.lines(1);
            final_line_loc = i;
        }
    }
    if(final_line_loc >= 0) loc.columns(len - final_line_loc - 1);
    loc.step();
}

<<EOF>>                     {return yy::sysyfParser::make_END(loc);}
.                           {std::cout << "Error in scanner!" << '\n'; exit(1);}
%%

int yyFlexLexer::yylex() {
    std::cerr << "'int yyFlexLexer::yylex()' should never be called." << std::endl;
    exit(1);
}