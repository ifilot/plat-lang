%{
#include "ast.h"
#include "diagnostics.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

int yylex(void);
void yyerror(const char *msg);

std::unique_ptr<plat::Program> platlang_program;
plat::DiagnosticReporter *platlang_diagnostics = nullptr;

#define LOC(location) plat::SourceLocation{(location).first_line, (location).first_column}
%}

%code requires {
#include "ast.h"

#include <string>
#include <vector>
}

%locations

%union {
  double number;
  bool boolean;
  std::string *text;
  plat::Expr *expr;
  plat::Stmt *stmt;
  plat::TableField *table_field;
  std::vector<plat::ExprPtr> *exprs;
  std::vector<plat::StmtPtr> *stmts;
  std::vector<plat::TableField> *table_fields;
  std::vector<std::string> *names;
}

%token <number> NUMBER
%token <text> STRING
%token <text> IDENTIFIER
%token LOAT FUNKSIE ES ANGESJ ZOLANG VEUR ENJ TROK AAFBRAEKE WEIER
%token NIKS WOAR NEETWOAR
%token EQ NEQ LE GE AND OR

%type <stmt> top_level_stmt stmt function_decl var_decl assignment expr_stmt return_stmt break_stmt continue_stmt if_stmt while_stmt for_stmt
%type <expr> expr logical_or logical_and equality comparison term factor unary postfix primary literal table table_key
%type <stmts> top_level_list block maybe_else
%type <exprs> argument_list maybe_arguments
%type <table_fields> table_fields maybe_table_fields
%type <table_field> table_field
%type <names> param_list maybe_params

%left OR
%left AND
%left EQ NEQ
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UMINUS '!'

%destructor { delete $$; } <text>
%destructor { delete $$; } <expr>
%destructor { delete $$; } <stmt>
%destructor { delete $$; } <table_field>
%destructor { delete $$; } <exprs>
%destructor { delete $$; } <stmts>
%destructor { delete $$; } <table_fields>
%destructor { delete $$; } <names>

%%

program:
  top_level_list {
    platlang_program = std::make_unique<plat::Program>(LOC(@1), std::move(*$1));
    delete $1;
  }
;

top_level_list:
  /* empty */ { $$ = new std::vector<plat::StmtPtr>(); }
| top_level_list top_level_stmt {
    $1->emplace_back($2);
    $$ = $1;
  }
;

top_level_stmt:
  function_decl { $$ = $1; }
| stmt { $$ = $1; }
;

function_decl:
  FUNKSIE IDENTIFIER '(' maybe_params ')' ':' block ENJ {
    $$ = new plat::FunctionDecl(LOC(@1), *$2, std::move(*$4), std::move(*$7));
    delete $2;
    delete $4;
    delete $7;
  }
;

maybe_params:
  /* empty */ { $$ = new std::vector<std::string>(); }
| param_list { $$ = $1; }
;

param_list:
  IDENTIFIER {
    $$ = new std::vector<std::string>();
    $$->push_back(*$1);
    delete $1;
  }
| param_list ',' IDENTIFIER {
    $1->push_back(*$3);
    delete $3;
    $$ = $1;
  }
;

block:
  /* empty */ { $$ = new std::vector<plat::StmtPtr>(); }
| block stmt {
    $1->emplace_back($2);
    $$ = $1;
  }
;

stmt:
  var_decl { $$ = $1; }
| assignment { $$ = $1; }
| expr_stmt { $$ = $1; }
| return_stmt { $$ = $1; }
| break_stmt { $$ = $1; }
| continue_stmt { $$ = $1; }
| if_stmt { $$ = $1; }
| while_stmt { $$ = $1; }
| for_stmt { $$ = $1; }
;

var_decl:
  LOAT IDENTIFIER {
    $$ = new plat::VarDeclStmt(LOC(@1), *$2, std::make_unique<plat::NilExpr>(LOC(@1)));
    delete $2;
  }
| LOAT IDENTIFIER '=' expr {
    $$ = new plat::VarDeclStmt(LOC(@1), *$2, plat::ExprPtr($4));
    delete $2;
  }
;

assignment:
  postfix '=' expr {
    $$ = new plat::AssignStmt(LOC(@1), plat::ExprPtr($1), plat::ExprPtr($3));
  }
;

expr_stmt:
  expr { $$ = new plat::ExprStmt(LOC(@1), plat::ExprPtr($1)); }
;

return_stmt:
  TROK {
    $$ = new plat::ReturnStmt(LOC(@1), std::make_unique<plat::NilExpr>(LOC(@1)));
  }
| TROK expr {
    $$ = new plat::ReturnStmt(LOC(@1), plat::ExprPtr($2));
  }
;

break_stmt:
  AAFBRAEKE { $$ = new plat::BreakStmt(LOC(@1)); }
;

continue_stmt:
  WEIER { $$ = new plat::ContinueStmt(LOC(@1)); }
;

if_stmt:
  ES expr ':' block maybe_else ENJ {
    $$ = new plat::IfStmt(LOC(@1), plat::ExprPtr($2), std::move(*$4), std::move(*$5));
    delete $4;
    delete $5;
  }
;

while_stmt:
  ZOLANG expr ':' block ENJ {
    $$ = new plat::WhileStmt(LOC(@1), plat::ExprPtr($2), std::move(*$4));
    delete $4;
  }
;

for_stmt:
  VEUR IDENTIFIER '=' expr ',' expr ':' block ENJ {
    $$ = new plat::ForStmt(LOC(@1), *$2, plat::ExprPtr($4), plat::ExprPtr($6), std::move(*$8));
    delete $2;
    delete $8;
  }
;

expr:
  logical_or { $$ = $1; }
;

maybe_else:
  /* empty */ { $$ = new std::vector<plat::StmtPtr>(); }
| ANGESJ ':' block { $$ = $3; }
;

logical_or:
  logical_and { $$ = $1; }
| logical_or OR logical_and {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "or", plat::ExprPtr($3));
  }
;

logical_and:
  equality { $$ = $1; }
| logical_and AND equality {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "and", plat::ExprPtr($3));
  }
;

equality:
  comparison { $$ = $1; }
| equality EQ comparison {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "==", plat::ExprPtr($3));
  }
| equality NEQ comparison {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "!=", plat::ExprPtr($3));
  }
;

comparison:
  term { $$ = $1; }
| comparison '<' term {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "<", plat::ExprPtr($3));
  }
| comparison '>' term {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), ">", plat::ExprPtr($3));
  }
| comparison LE term {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "<=", plat::ExprPtr($3));
  }
| comparison GE term {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), ">=", plat::ExprPtr($3));
  }
;

term:
  factor { $$ = $1; }
| term '+' factor {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "+", plat::ExprPtr($3));
  }
| term '-' factor {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "-", plat::ExprPtr($3));
  }
;

factor:
  unary { $$ = $1; }
| factor '*' unary {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "*", plat::ExprPtr($3));
  }
| factor '/' unary {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "/", plat::ExprPtr($3));
  }
| factor '%' unary {
    $$ = new plat::BinaryExpr(LOC(@2), plat::ExprPtr($1), "%", plat::ExprPtr($3));
  }
;

unary:
  postfix { $$ = $1; }
| '-' unary %prec UMINUS {
    $$ = new plat::UnaryExpr(LOC(@1), "-", plat::ExprPtr($2));
  }
| '!' unary {
    $$ = new plat::UnaryExpr(LOC(@1), "!", plat::ExprPtr($2));
  }
;

postfix:
  primary { $$ = $1; }
| IDENTIFIER '(' maybe_arguments ')' {
    $$ = new plat::CallExpr(LOC(@1), *$1, std::move(*$3));
    delete $1;
    delete $3;
  }
| postfix '[' expr ']' {
    $$ = new plat::IndexExpr(LOC(@2), plat::ExprPtr($1), plat::ExprPtr($3));
  }
| postfix '.' IDENTIFIER {
    $$ = new plat::FieldExpr(LOC(@2), plat::ExprPtr($1), *$3);
    delete $3;
  }
;

maybe_arguments:
  /* empty */ { $$ = new std::vector<plat::ExprPtr>(); }
| argument_list { $$ = $1; }
;

argument_list:
  expr {
    $$ = new std::vector<plat::ExprPtr>();
    $$->emplace_back($1);
  }
| argument_list ',' expr {
    $1->emplace_back($3);
    $$ = $1;
  }
;

primary:
  literal { $$ = $1; }
| IDENTIFIER {
    $$ = new plat::IdentifierExpr(LOC(@1), *$1);
    delete $1;
  }
| '(' expr ')' { $$ = $2; }
| table { $$ = $1; }
;

literal:
  NUMBER { $$ = new plat::NumberExpr(LOC(@1), $1); }
| STRING {
    $$ = new plat::StringExpr(LOC(@1), *$1);
    delete $1;
  }
| WOAR { $$ = new plat::BoolExpr(LOC(@1), true); }
| NEETWOAR { $$ = new plat::BoolExpr(LOC(@1), false); }
| NIKS { $$ = new plat::NilExpr(LOC(@1)); }
;

table:
  '{' maybe_table_fields '}' {
    $$ = new plat::TableExpr(LOC(@1), std::move(*$2));
    delete $2;
  }
;

maybe_table_fields:
  /* empty */ { $$ = new std::vector<plat::TableField>(); }
| table_fields { $$ = $1; }
| table_fields ',' { $$ = $1; }
;

table_fields:
  table_field {
    $$ = new std::vector<plat::TableField>();
    $$->push_back(std::move(*$1));
    delete $1;
  }
| table_fields ',' table_field {
    $1->push_back(std::move(*$3));
    delete $3;
    $$ = $1;
  }
;

table_field:
  expr {
    $$ = new plat::TableField(LOC(@1), std::nullopt, plat::ExprPtr($1));
  }
| IDENTIFIER '=' expr {
    $$ = new plat::TableField(
        LOC(@1),
        std::optional<plat::ExprPtr>(std::make_unique<plat::StringExpr>(LOC(@1), *$1)),
        plat::ExprPtr($3));
    delete $1;
  }
| '[' table_key ']' '=' expr {
    $$ = new plat::TableField(LOC(@1), std::optional<plat::ExprPtr>(plat::ExprPtr($2)), plat::ExprPtr($5));
  }
;

table_key:
  expr { $$ = $1; }
;

%%

void yyerror(const char *msg) {
  if (platlang_diagnostics != nullptr) {
    platlang_diagnostics->error(
        plat::DiagnosticId::SyntaxError,
        plat::SourceLocation{yylloc.first_line, yylloc.first_column});
    return;
  }

  std::fprintf(stderr, "parse error: %s\n", msg);
}
