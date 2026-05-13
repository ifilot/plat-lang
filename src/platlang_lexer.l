%option noyywrap yylineno noinput nounput 8bit
%{
#include "platlang_parser.h"

#include <cstdlib>
#include <string>

/**
 * Tracks the one-based column for the next token start.
 *
 * Flex maintains yylineno for us, but column tracking is local to this scanner.
 */
static int yycolumn = 1;

/**
 * Copies the current token span into Bison's location object before each rule
 * action runs.
 */
#define YY_USER_ACTION                                  \
  yylloc.first_line = yylloc.last_line = yylineno;      \
  yylloc.first_column = yycolumn;                       \
  yylloc.last_column = yycolumn + yyleng - 1;           \
  yycolumn += yyleng;

/**
 * Decodes a quoted source string into the value stored in the AST.
 *
 * @param text Matched token text, including the surrounding quotes.
 * @param length Number of bytes in the matched token.
 * @return Unescaped string value.
 */
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
  /* Whitespace and comments do not reach the parser, but still update source
     locations for diagnostics. */
[ \t]+                    {}
\r\n                      { yycolumn = 1; }
\n                        { yycolumn = 1; }
\r                        { ++yylineno; yycolumn = 1; }
"#"[^\r\n]*               {}

  /* Plat keyword tokens. The lexer accepts both spellings of `trok` for source
     compatibility. */
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
"kwatsj"                  { return KWATSJ; }
"en"                      { return AND; }
"of"                      { return OR; }

  /* Multi-character operators must be matched before their single-character
     prefixes below. */
"=="                      { return EQ; }
"!="                      { return NEQ; }
"<="                      { return LE; }
">="                      { return GE; }

  /* Literal and identifier rules allocate semantic values owned by the parser
     until the matching grammar action or destructor consumes them. */
[0-9]+(\.[0-9]+)?         { yylval.number = std::strtod(yytext, nullptr); return NUMBER; }
\"([^\\\"\r\n]|\\[nt\"\\])*\" {
                            yylval.text = new std::string(unescape_string(yytext, yyleng));
                            return STRING;
                          }
[A-Za-z_\200-\377][A-Za-z0-9_\200-\377]* {
                            yylval.text = new std::string(yytext);
                            return IDENTIFIER;
                          }

  /* Single-character tokens are returned as their character code, which keeps
     the grammar readable for punctuation and arithmetic operators. */
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

  /* Let the parser report unexpected characters with the precise source
     location captured by YY_USER_ACTION. */
.                         { return yytext[0]; }

%%
