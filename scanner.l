%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symbol_table.h"
    #include "semantics.h"
    #include "ast.h"
    #include "parser.tab.h"

    extern int yylineno;
    extern FILE *yyin;
    extern FILE *yyout;
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
DECIMAL             [0-9]+(\.[0-9]+)

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
{CIERTO}            { yylval.val.boolean = "true"; return CIERTO; }
{FALSO}             { yylval.val.boolean = "false"; return FALSO; }
{SI}                { return SI; }
{SINO}              { return SINO; }
{POR}               { return POR; }
{EN}                { return EN; }
{MIENTRAS}          { return MIENTRAS; }
{PARAR}             { return PARAR; }
{CONTINUAR}         { return CONTINUAR; }
{REGRESAR}          { return REGRESAR; }

"+"                 { yylval.val.integer = OP_ADD; return ADD; }
"-"                 { yylval.val.integer = OP_SUB; return SUB; }
"*"                 { yylval.val.integer = OP_MUL; return MUL; }
"/"                 { yylval.val.integer = OP_DIV; return DIV; }
"%"                 { yylval.val.integer = OP_MOD; return MOD; }
"^"                 { yylval.val.integer = OP_EXP; return EXP; }
"+="                { yylval.val.integer = OP_ADD_ASSIGN; return ADD_ASSIGN; }
"-="                { yylval.val.integer = OP_SUB_ASSIGN; return SUB_ASSIGN; }
"*="                { yylval.val.integer = OP_MUL_ASSIGN; return MUL_ASSIGN; }
"/="                { yylval.val.integer = OP_DIV_ASSIGN; return DIV_ASSIGN; }
"%="                { yylval.val.integer = OP_MOD_ASSIGN; return MOD_ASSIGN; }
"^="                { yylval.val.integer = OP_EXP_ASSIGN; return EXP_ASSIGN; }
"||"                { yylval.val.integer = OP_OR; return OR; }
"&&"                { yylval.val.integer = OP_AND; return AND; }
"!"                 { yylval.val.integer = OP_NOT; return NOT; }
"=="                { yylval.val.integer = OP_EQUAL; return EQ; }
"!="                { yylval.val.integer = OP_NOT_EQUAL; return NE; }
"<"                 { yylval.val.integer = OP_LESS; return LESS; }
"<="                { yylval.val.integer = OP_LE; return LE; }
">"                 { yylval.val.integer = OP_GREAT; return GREAT; }
">="                { yylval.val.integer = OP_GE; return GE; }
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

{IDENTIFIER}        {
                        if (lookup(yytext) == NULL) {
                            insert(strdup(yytext), strlen(yytext), UNDEF, yylineno);
                        }
                        
                        yylval.item = lookup(yytext);
                        return IDENTIFIER;
                    }

{INTEGER}           { yylval.val.integer = atoi(yytext); return INTEGER; }
{DECIMAL}           { yylval.val.real = atof(yytext); return DECIMAL; }
{CHARACTER}         { yylval.val.character = yytext[1]; return CHARACTER; }
{STRING}            { addToMessages(yytext); yylval.val.string = strdup(yytext); return STRING; }

\n                  { yylineno++; }
{WHITESPACE}        { }

.                   { fprintf(stderr, "Unrecognized character at line %d: %s\n", yylineno, yytext); exit(1); }

%%
