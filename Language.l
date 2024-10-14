%{
#include "Language.tab.h"
void yyerror (char *s);
int yylex();
%}
%%
\{                      {return '{';}
\}                      {return '}';}
\(                      {return '(';}
\)                      {return ')';}
\[                      {return '[';}   
\]                      {return ']';}
\:                      {return ':';}
\+                      {return PLUS;}
\-                      {return MINUS;}
\*                      {return MULTIPLY;}
\/                      {return DIVIDE;}
\&                      {return AND;}
\|                      {return OR;}
\>                      {return GREATER;}
\<                      {return LESS;}
\==                     {return EQUAL;}
\>=                     {return GREATER_EQUAL;} 
\<=                     {return LESS_EQUAL;}
\!                      {return NOT;}
int                     {yylval.str = strdup(yytext); return t_INTEGER_CMD;} 
str                     {yylval.str = strdup(yytext); return t_STRING_CMD;}  
set                     {yylval.str = strdup(yytext); return t_SET_CMD;}
collection              {yylval.str = strdup(yytext); return t_COLLECTION_CMD;}
output                  {yylval.str = strdup(yytext); return t_OUTPUT_CMD;}
input                   {yylval.str = strdup(yytext); return t_INPUT_CMD; }
if                      {yylval.str = strdup(yytext); return t_IF_CMD;}
else                    {yylval.str = strdup(yytext); return t_ELSE_CMD;}
while                   {yylval.str = strdup(yytext); return t_WHILE_CMD;}
for                     {yylval.str = strdup(yytext); return t_FOR_CMD;}
0|([-]?[1-9][0-9]*)     { yylval.num = atoi(yytext); return t_INTEGER; }
\"[^\"]*\"              {yylval.str = strdup(yytext); return t_STRING;} 
[a-zA-Z][a-zA-Z0-9]*    {yylval.str = strdup(yytext); return t_ID;} 
[=;,]                   {return yytext[0];}
[ \t\n]                 {}
.                       {ECHO; yyerror ("unexpected character");}

%%
int yywrap (void) {return 1;}
