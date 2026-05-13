#include "ast_printer.h"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

namespace plat {

AstPrinter::AstPrinter(std::ostream &stream) : stream_(&stream) {}

void AstPrinter::print_program(const Program &program) {
    print_line("Program");
    print_location(program.location());
    *stream_ << '\n';

    push_indent();
    print_stmt_list("statements", program.statements());
    pop_indent();
}

void AstPrinter::print_stmt(const Stmt &stmt) {
    if (const auto *node = dynamic_cast<const FunctionDecl *>(&stmt)) {
        print_line("FunctionDecl name=\"" + node->name() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("params");
        *stream_ << '\n';
        push_indent();
        for (const std::string &param : node->params()) {
            print_line(param);
            *stream_ << '\n';
        }
        pop_indent();
        print_stmt_list("body", node->body());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const VarDeclStmt *>(&stmt)) {
        print_line("VarDeclStmt name=\"" + node->name() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("initializer");
        *stream_ << '\n';
        push_indent();
        print_expr(node->initializer());
        pop_indent();
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const AssignStmt *>(&stmt)) {
        print_line("AssignStmt");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("target");
        *stream_ << '\n';
        push_indent();
        print_expr(node->target());
        pop_indent();
        print_line("value");
        *stream_ << '\n';
        push_indent();
        print_expr(node->value());
        pop_indent();
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const ExprStmt *>(&stmt)) {
        print_line("ExprStmt");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_expr(node->expr());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const ReturnStmt *>(&stmt)) {
        print_line("ReturnStmt");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_expr(node->value());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const BreakStmt *>(&stmt)) {
        print_line("BreakStmt");
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const ContinueStmt *>(&stmt)) {
        print_line("ContinueStmt");
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const ImportStmt *>(&stmt)) {
        print_line("ImportStmt module=\"" + node->module_name() + "\"");
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const WhileStmt *>(&stmt)) {
        print_line("WhileStmt");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("condition");
        *stream_ << '\n';
        push_indent();
        print_expr(node->condition());
        pop_indent();
        print_stmt_list("body", node->body());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const ForStmt *>(&stmt)) {
        print_line("ForStmt name=\"" + node->name() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("begin");
        *stream_ << '\n';
        push_indent();
        print_expr(node->begin());
        pop_indent();
        print_line("end");
        *stream_ << '\n';
        push_indent();
        print_expr(node->end());
        pop_indent();
        print_stmt_list("body", node->body());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const IfStmt *>(&stmt)) {
        print_line("IfStmt");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("condition");
        *stream_ << '\n';
        push_indent();
        print_expr(node->condition());
        pop_indent();
        print_stmt_list("then", node->then_body());
        print_stmt_list("else", node->else_body());
        pop_indent();
        return;
    }

    print_line("UnknownStmt");
    print_location(stmt.location());
    *stream_ << '\n';
}

void AstPrinter::print_expr(const Expr &expr) {
    if (const auto *node = dynamic_cast<const NumberExpr *>(&expr)) {
        std::ostringstream text;
        text << "NumberExpr value=" << node->value();
        print_line(text.str());
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const StringExpr *>(&expr)) {
        print_line("StringExpr value=\"" + node->value() + "\"");
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const BoolExpr *>(&expr)) {
        print_line(std::string("BoolExpr value=") + (node->value() ? "woar" : "kwatsj"));
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const NilExpr *>(&expr)) {
        print_line("NilExpr");
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const IdentifierExpr *>(&expr)) {
        print_line("IdentifierExpr name=\"" + node->name() + "\"");
        print_location(node->location());
        *stream_ << '\n';
        return;
    }

    if (const auto *node = dynamic_cast<const UnaryExpr *>(&expr)) {
        print_line("UnaryExpr op=\"" + node->op() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_expr(node->operand());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const BinaryExpr *>(&expr)) {
        print_line("BinaryExpr op=\"" + node->op() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("left");
        *stream_ << '\n';
        push_indent();
        print_expr(node->left());
        pop_indent();
        print_line("right");
        *stream_ << '\n';
        push_indent();
        print_expr(node->right());
        pop_indent();
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const CallExpr *>(&expr)) {
        print_line("CallExpr callee=\"" + node->callee() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        for (const ExprPtr &arg : node->args()) {
            print_expr(*arg);
        }
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const IndexExpr *>(&expr)) {
        print_line("IndexExpr");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_line("object");
        *stream_ << '\n';
        push_indent();
        print_expr(node->object());
        pop_indent();
        print_line("index");
        *stream_ << '\n';
        push_indent();
        print_expr(node->index());
        pop_indent();
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const FieldExpr *>(&expr)) {
        print_line("FieldExpr field=\"" + node->field() + "\"");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        print_expr(node->object());
        pop_indent();
        return;
    }

    if (const auto *node = dynamic_cast<const TableExpr *>(&expr)) {
        print_line("TableExpr");
        print_location(node->location());
        *stream_ << '\n';

        push_indent();
        for (const TableField &field : node->fields()) {
            print_line("TableField");
            print_location(field.location());
            *stream_ << '\n';

            push_indent();
            if (field.has_key()) {
                print_line("key");
                *stream_ << '\n';
                push_indent();
                print_expr(field.key());
                pop_indent();
            }
            print_line("value");
            *stream_ << '\n';
            push_indent();
            print_expr(field.value());
            pop_indent();
            pop_indent();
        }
        pop_indent();
        return;
    }

    print_line("UnknownExpr");
    print_location(expr.location());
    *stream_ << '\n';
}

void AstPrinter::print_stmt_list(const std::string &label,
                                 const std::vector<StmtPtr> &statements) {
    print_line(label);
    *stream_ << '\n';

    push_indent();
    for (const StmtPtr &stmt : statements) {
        print_stmt(*stmt);
    }
    pop_indent();
}

void AstPrinter::print_location(const SourceLocation &location) {
    *stream_ << " @" << location.line() << ":" << location.column();
}

void AstPrinter::print_line(const std::string &text) {
    for (int i = 0; i < indent_; ++i) {
        *stream_ << "    ";
    }

    *stream_ << text;
}

void AstPrinter::push_indent() {
    ++indent_;
}

void AstPrinter::pop_indent() {
    --indent_;
}

} // namespace plat
