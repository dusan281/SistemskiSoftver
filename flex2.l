%{
#include "bison2.tab.h"
  int line_num = 1;
%}





LSQB  "["
RSQB  "]"
PLUS  "+"
COMMA ","
TWO_PUNKT ":"


%%

\.global      { return GLOBAL; }
\.extern      { return EXTERN; }
\.section     { return SECTION; }
\.word        { return WORD; }
\.skip        { return SKIP; }
\.end         { return END; }

halt         { return HALT; }
int          { return INT; }
iret         { return IRET; }
call         { return CALL; }
ret          { return RET; }
jmp          { return JMP; }
beq          { return BEQ; }
bne          { return BNE; }
bgt          { return BGT; }
push         { return PUSH; }
pop          { return POP; }
xchg         { return XCHG; }
add          { return ADD; }
sub          { return SUB; }
mul          { return MUL; }
div          { return DIV; }
not          { return NOT; }
and          { return AND; }
or           { return OR; }
xor          { return XOR; }
shl          { return SHL; }
shr          { return SHR; }
ld           { return LD; }
st           { return ST; }
csrrd        { return CSR_RD; }
csrwr        { return CSR_WR; }

%status|%handler|%cause {
    if (strcmp(yytext, "%status") == 0){
        yylval.reg = 0;
    }
    else if (strcmp(yytext, "%handler") == 0){
        yylval.reg = 1;
    }
    else yylval.reg = 2;
     return CSR; }

$[0-9]+         { yylval.literal = atoi(yytext + 1); return LITERAL; }
$0[xX][0-9a-fA-F]+ { yylval.literal = strtol(yytext + 1, NULL, 16); return LITERAL; }
[0-9]+           { yylval.literal = atoi(yytext); return MEM_LITERAL; }
0[xX][0-9a-fA-F]+ { yylval.literal = strtol(yytext, NULL, 16); return MEM_LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.symbol = strdup(yytext); return SYMBOL_MEM; }
$[a-zA-Z_][a-zA-Z0-9_]* { yylval.symbol = strdup(yytext+1); return SYMBOL; }

%r[0-9]|%r1[0-5]|%sp|%pc  { 
    if (strcmp(yytext, "%sp") == 0) {
        yylval.reg = 14;
    } else if (strcmp(yytext, "%pc") == 0) {
        yylval.reg = 15;
    } else {
        yylval.reg = atoi(yytext + 2);
    }
    return REG_VALUE; 
}

#.*      { return COMMENT; }

{LSQB}          { return LSQB; }
{RSQB}            { return RSQB;}
{PLUS}            {return PLUS;}
{COMMA}           { return COMMA;}
{TWO_PUNKT}       { return TWO_PUNKT;}
[ \t\r]+      { /* Ignoriši bele karaktere */ }
\n             { line_num++;return ENDL;}

.               {}

%%

int yywrap() {
    return 1;
}