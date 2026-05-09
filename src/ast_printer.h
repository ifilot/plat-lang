#pragma once

#include "ast.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace plat {

/**
 * Pretty-prints a parsed AST as an indented tree.
 */
class AstPrinter {
private:
    std::ostream *stream_;
    int indent_ = 0;

public:
    /**
     * Creates an AST pretty printer.
     *
     * @param stream Output stream for the printed tree.
     */
    explicit AstPrinter(std::ostream &stream);

    /**
     * Prints a complete parsed program.
     *
     * @param program Program AST to print.
     */
    void print_program(const Program &program);

private:
    /**
     * Prints a statement node.
     *
     * @param stmt Statement node to print.
     */
    void print_stmt(const Stmt &stmt);

    /**
     * Prints an expression node.
     *
     * @param expr Expression node to print.
     */
    void print_expr(const Expr &expr);

    /**
     * Prints a sequence of statements under the current indentation.
     *
     * @param label Section label.
     * @param statements Statements to print.
     */
    void print_stmt_list(const std::string &label,
                         const std::vector<StmtPtr> &statements);

    /**
     * Prints a source location suffix.
     *
     * @param location Source location to print.
     */
    void print_location(const SourceLocation &location);

    /**
     * Prints one indented line.
     *
     * @param text Text to print.
     */
    void print_line(const std::string &text);

    /**
     * Increases indentation for a nested tree section.
     */
    void push_indent();

    /**
     * Decreases indentation after a nested tree section.
     */
    void pop_indent();
};

} // namespace plat
