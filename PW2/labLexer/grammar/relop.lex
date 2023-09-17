

%{
#include "pl0.h"
#include <stdio.h>
%}

%%

"<"         {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return(lss);}
"<="         {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return(leq);}
">"         {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return(gtr);}
">="         {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return(geq);}
"="         {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return(eql);}
"<>"         {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return(neq);}
[\n|\r\n]  {
    if(cc > 0) printf("(other,%ld)", cc);
    cc = 0;
    return('\n');}
.          {cc++;}
%%
void getsym()
{
	sym = yylex();
}
