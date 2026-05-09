%option noyywrap yylineno noinput nounput
%{
#include "platlang_parser.h"

#include <cstdlib>
#include <string>

static int yycolumn = 1;

#define YY_USER_ACTION                                  \
  yylloc.first_line = yylloc.last_line = yylineno;      \
  yylloc.first_column = yycolumn;                       \
  yylloc.last_column = yycolumn + yyleng - 1;           \
  yycolumn += yyleng;

static std::string unescape_string(const char *text, int length) {
  std::string result;

  for (int i = 1; i < length - 1; ++i) {
    if (text[i] != '\\') {
      result.push_back(text[i]);
      continue;
    }

    ++i;
    switch (text[i]) {
    case 'n':
      result.push_back('\n');
      break;
    case 't':
      result.push_back('\t');
      break;
    case '"':
      result.push_back('"');
      break;
    case '\\':
      result.push_back('\\');
      break;
    default:
      result.push_back(text[i]);
      break;
    }
  }

  return result;
}
%}

%%
[ \t\r]+                  {}
\n                        { yycolumn = 1; }
"#".*                     {}

"loat"                    { return LOAT; }
"funksie"                 { return FUNKSIE; }
"es"                      { return ES; }
"angesj"                  { return ANGESJ; }
"zolang"                  { return ZOLANG; }
"veur"                    { return VEUR; }
"enj"                     { return ENJ; }
"trok"                    { return TROK; }
"aafbraeke"               { return AAFBRAEKE; }
"weier"                   { return WEIER; }
"niks"                    { return NIKS; }
"woar"                    { return WOAR; }
"neetwoar"                { return NEETWOAR; }
"and"                     { return AND; }
"or"                      { return OR; }

"=="                      { return EQ; }
"!="                      { return NEQ; }
"<="                      { return LE; }
">="                      { return GE; }

[0-9]+(\.[0-9]+)?         { yylval.number = std::strtod(yytext, nullptr); return NUMBER; }
\"([^\\\"\n]|\\[nt\"\\])*\" {
                            yylval.text = new std::string(unescape_string(yytext, yyleng));
                            return STRING;
                          }
[A-Za-z_][A-Za-z0-9_]*    { yylval.text = new std::string(yytext); return IDENTIFIER; }

"+"                       { return '+'; }
"-"                       { return '-'; }
"*"                       { return '*'; }
"/"                       { return '/'; }
"%"                       { return '%'; }
"!"                       { return '!'; }
"<"                       { return '<'; }
">"                       { return '>'; }
"="                       { return '='; }
"("                       { return '('; }
")"                       { return ')'; }
"{"                       { return '{'; }
"}"                       { return '}'; }
"["                       { return '['; }
"]"                       { return ']'; }
":"                       { return ':'; }
","                       { return ','; }
"."                       { return '.'; }

.                         { return yytext[0]; }

%%
