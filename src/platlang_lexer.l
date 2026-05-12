%option noyywrap yylineno noinput nounput 8bit
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
[ \t]+                    {}
\r\n                      { yycolumn = 1; }
\n                        { yycolumn = 1; }
\r                        { ++yylineno; yycolumn = 1; }
"#"[^\r\n]*               {}

"loat"                    { return LOAT; }
"funksie"                 { return FUNKSIE; }
"es"                      { return ES; }
"angesj"                  { return ANGESJ; }
"zolang"                  { return ZOLANG; }
"veur"                    { return VEUR; }
"enj"                     { return ENJ; }
"trok"                    { return TROK; }
"trök"                    { return TROK; }
"aafbraeke"               { return AAFBRAEKE; }
"euversjlaon"             { return EUVERSJLAON; }
"inlaaje"                 { return INLAAJE; }
"niks"                    { return NIKS; }
"woar"                    { return WOAR; }
"neetwoar"                { return NEETWOAR; }
"en"                      { return AND; }
"of"                      { return OR; }

"=="                      { return EQ; }
"!="                      { return NEQ; }
"<="                      { return LE; }
">="                      { return GE; }

[0-9]+(\.[0-9]+)?         { yylval.number = std::strtod(yytext, nullptr); return NUMBER; }
\"([^\\\"\r\n]|\\[nt\"\\])*\" {
                            yylval.text = new std::string(unescape_string(yytext, yyleng));
                            return STRING;
                          }
[A-Za-z_\200-\377][A-Za-z0-9_\200-\377]* {
                            yylval.text = new std::string(yytext);
                            return IDENTIFIER;
                          }

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
