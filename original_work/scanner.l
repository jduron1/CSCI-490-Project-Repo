%{
    #include <stdio.h>
    #include <string.h>
    #include "parser.tab.h"
%}

%option noyywrap

SEA                 (?i:sea)
CARACTER            (?i:car)
BOOLEANO            (?i:bool)
ENTERO              (?i:ent)
FLOTANTE            (?i:flot)
CADENA              (?i:cadena)
VACIO               (?i:vacio)
FUNCION             (?i:func)
CLASE               (?i:clase)
CIERTO              (?i:cierto)
FALSO               (?i:falso)
SI                  (?i:si)
SINO                (?i:sino)
Y                   (?i:y)
O                   (?i:o)
POR                 (?i:por)
MIENTRAS            (?i:mientras)
ESTA                (?i:esta)
EN                  (?i:en)
REGRESA             (?i:regresa)

IDENTIFIER          [a-zA-Z][a-zA-Z0-9_]*

INTEGER             [0-9]+
REAL                [0-9]+(\.[0-9]+)([eE][+-]?[0-9]+)?

CHARACTER           \'([^\n]|\\\n)\'
STRING              \"([^\n]|\\\n)*\"

SINGLE_LINE         \#[^*].*
MULTI_LINE          \#\*([^*]|\*+[^*#])*\*+\#

WHITESPACE          [\ \f\r\t\v]+

%%

{SINGLE_LINE}       { }
{MULTI_LINE}        { }

{CHARACTER}         { yylval.character = yytext[1]; return CHARACTER; }
{STRING}            { yylval.string = strdup(yytext); return STRING; }

{SEA}               { yylval.string = yytext; return SEA; }
{CARACTER}          { yylval.string = yytext; return CARACTER; }
{BOOLEANO}          { yylval.string = yytext; return BOOLEANO; }
{ENTERO}            { yylval.string = yytext; return ENTERO; }
{FLOTANTE}          { yylval.string = yytext; return FLOTANTE; }
{CADENA}            { yylval.string = yytext; return CADENA; }
{VACIO}             { yylval.string = yytext; return VACIO; }
{FUNCION}           { yylval.string = yytext; return FUNCION; }
{CLASE}             { yylval.string = yytext; return CLASE; }
{CIERTO}            { yylval.string = yytext; return CIERTO; }
{FALSO}             { yylval.string = yytext; return FALSO; }
{SI}                { yylval.string = yytext; return SI; }
{SINO}              { yylval.string = yytext; return SINO; }
{Y}                 { yylval.string = yytext; return Y; }
{O}                 { yylval.string = yytext; return O; }
{POR}               { yylval.string = yytext; return POR; }
{MIENTRAS}          { yylval.string = yytext; return MIENTRAS; }
{ESTA}              { yylval.string = yytext; return ESTA; }
{EN}                { yylval.string = yytext; return EN; }
{REGRESA}           { yylval.string = yytext; return REGRESA; }

{INTEGER}           { yylval.integer = atoi(yytext); return INTEGER; }
{REAL}              { yylval.real = atof(yytext); return REAL; }

\n                  { yylineno++; }
{WHITESPACE}        { }

"+"                 { return '+'; }
"-"                 { return '-'; }
"*"                 { return '*'; }
"/"                 { return '/'; }
"^"                 { return '^'; }
"%"                 { return '%'; }
"!"                 { return '!'; }
"+="                { return PLUS_EQUAL; }
"-="                { return MINUS_EQUAL; }
"*="                { return TIMES_EQUAL; }
"/="                { return DIVIDE_EQUAL; }
"%="                { return MOD_EQUAL; }
"="                 { return '='; }
"=="                { return EQUAL; }
"!="                { return NOT_EQUAL; }
"<"                 { return '<'; }
"<="                { return LE; }
">"                 { return '>'; }
">="                { return GE; }
"->"                { return ARROW; }
"("                 { return '('; }
")"                 { return ')'; }
"{"                 { return '{'; }
"}"                 { return '}'; }
"["                 { return '['; }
"]"                 { return ']'; }
";"                 { return ';'; }
","                 { return ','; }
"."                 { return '.'; }
"..."               { return ELLIPSIS; }
":"                 { return ':'; }

{IDENTIFIER}        { yylval.string = yytext; return IDENTIFIER; }

.                   { printf("Lexical error: unexpected character %s\n", yytext); }

%%
