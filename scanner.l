%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "parser.tab.h"
%}

%option noyywrap
%x MULTI_LINE

CARACTER            (?i:car)
BOOLEANO            (?i:bool)
ENTERO              (?i:ent)
REAL                (?i:real)
CADENA              (?i:cad)
VACIO               (?i:vacio)
FUNCION             (?i:func)
CIERTO              (?i:cierto)
FALSO               (?i:falso)
SI                  (?i:si)
SINO                (?i:sino)
POR                 (?i:por)
EN                  (?i:en)
MIENTRAS            (?i:mientras)
PARAR               (?i:parar)
CONTINUAR           (?i:continuar)
REGRESAR            (?i:regresar)

IDENTIFIER          [a-zA-Z][a-zA-Z0-9_]*

INTEGER             [0-9]+
FLOAT               [0-9]+(\.[0-9]+)

CHARACTER           \'[^\']\'
STRING              \"[^\"]*\"

SINGLE_LINE         \#\#.*

WHITESPACE          [\ \f\r\t\v]+

%%

{SINGLE_LINE}       { }

"#*"                { BEGIN(MULTI_LINE); }
<MULTI_LINE>"*#"    { BEGIN(INITIAL); }
<MULTI_LINE>[^*\n]+ { }
<MULTI_LINE>"*"     { }
<MULTI_LINE>\n      { yylineno++; }

{CARACTER}          { return CARACTER; }
{BOOLEANO}          { return BOOLEANO; }
{ENTERO}            { return ENTERO; }
{REAL}              { return REAL; }
{CADENA}            { return CADENA; }
{VACIO}             { return VACIO; }
{FUNCION}           { return FUNCION; }
{CIERTO}            { return CIERTO; }
{FALSO}             { return FALSO; }
{SI}                { return SI; }
{SINO}              { return SINO; }
{POR}               { return POR; }
{EN}                { return EN; }
{MIENTRAS}          { return MIENTRAS; }
{PARAR}             { return PARAR; }
{CONTINUAR}         { return CONTINUAR; }
{REGRESAR}          { return REGRESAR; }

"+"                 { return ADD; }
"-"                 { return SUB; }
"*"                 { return MUL; }
"/"                 { return DIV; }
"%"                 { return MOD; }
"^"                 { return EXP; }
"+="                { return ADD_ASSIGN; }
"-="                { return SUB_ASSIGN; }
"*="                { return MUL_ASSIGN; }
"/="                { return DIV_ASSIGN; }
"%="                { return MOD_ASSIGN; }
"^="                { return EXP_ASSIGN; }
"||"                { return OR; }
"&&"                { return AND; }
"!"                 { return NOT; }
"=="                { return EQ; }
"!="                { return NE; }
"<"                 { return LESS; }
"<="                { return LE; }
">"                 { return GREAT; }
">="                { return GE; }
"->"                { return ARROW; }
"..."               { return ELLIPSIS; }

"("                 { return LPAREN; }
")"                 { return RPAREN; }
"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
"["                 { return LBRACKET; }
"]"                 { return RBRACKET; }
";"                 { return SEMICOLON; }
","                 { return COMMA; }
"="                 { return ASSIGN; }
"&"                 { return AMPERSAND; }

{IDENTIFIER}        { printf("Identifier: %s\n", yytext); return IDENTIFIER; }

{INTEGER}           { printf("Integer: %s\n", yytext); return INTEGER; }
{FLOAT}             { printf("Float: %s\n", yytext); return FLOAT; }
{CHARACTER}         { printf("Character: %s\n", yytext); return CHARACTER; }
{STRING}            { printf("String: %s\n", yytext); return STRING; }

\n                  { yylineno++; }
{WHITESPACE}        { }

.                   { fprintf(stderr, "Unrecognized character at line %d: %s\n", yylineno, yytext); exit(1); }

%%