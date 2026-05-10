#pragma once

#include "ast.h"
#include "diagnostics.h"
#include "environment.h"
#include "value.h"

#include <memory>
#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace plat {

/**
 * Tree-walking interpreter for plat-lang programs.
 */
class Interpreter {
private:
    DiagnosticReporter *diagnostics_;
    std::istream *input_;
    std::ostream *output_;
    std::shared_ptr<const std::unordered_set<std::string>> protected_names_;
    std::shared_ptr<Environment> global_environment_;
    std::shared_ptr<Environment> environment_;
    std::unordered_map<std::string, const FunctionDecl *> functions_;
    int function_depth_ = 0;
    int loop_depth_ = 0;

public:
    /**
     * Creates an interpreter.
     *
     * @param diagnostics Diagnostic reporter for runtime errors.
     * @param input Input stream for built-in functions.
     * @param output Output stream for built-in functions.
     */
    Interpreter(DiagnosticReporter &diagnostics, std::istream &input,
                std::ostream &output);

    /**
     * Executes a complete program.
     *
     * @param program Program AST to execute.
     */
    void execute_program(const Program &program);

private:
    /**
     * Registers all top-level function declarations before execution.
     *
     * @param program Program AST to scan.
     */
    void register_top_level_functions(const Program &program);

    /**
     * Executes a statement.
     *
     * @param stmt Statement to execute.
     */
    void execute_stmt(const Stmt &stmt);

    /**
     * Executes a sequence of statements in a child scope.
     *
     * @param statements Statements to execute.
     */
    void execute_scoped_block(const std::vector<StmtPtr> &statements);

    /**
     * Executes a sequence of statements in the current scope.
     *
     * @param statements Statements to execute.
     */
    void execute_current_block(const std::vector<StmtPtr> &statements);

    /**
     * Evaluates an expression.
     *
     * @param expr Expression to evaluate.
     * @return Runtime value.
     */
    Value evaluate_expr(const Expr &expr);

    /**
     * Registers a global function declaration.
     *
     * @param function Function declaration.
     */
    void register_function(const FunctionDecl &function);

    /**
     * Calls a built-in or user-defined function.
     *
     * @param call Function call expression.
     * @return Function return value.
     */
    Value call_function(const CallExpr &call);

    /**
     * Calls the `aafdrokke` built-in.
     *
     * @param name Name used at the call site.
     * @param args Evaluated function arguments.
     * @param location Source location of the call.
     * @return Built-in return value.
     */
    Value call_print_builtin(const std::string &name,
                             const std::vector<Value> &args,
                             SourceLocation location);

    /**
     * Calls the `invuier` built-in.
     *
     * @param name Name used at the call site.
     * @param args Evaluated function arguments.
     * @param location Source location of the call.
     * @return Read line, or `niks` at end of input.
     */
    Value call_input_builtin(const std::string &name,
                             const std::vector<Value> &args,
                             SourceLocation location);

    /**
     * Calls the `waatis` built-in.
     *
     * @param name Name used at the call site.
     * @param args Evaluated function arguments.
     * @param location Source location of the call.
     * @return `niks` after printing the runtime type name.
     */
    Value call_type_builtin(const std::string &name,
                            const std::vector<Value> &args,
                            SourceLocation location);

    /**
     * Evaluates all call arguments.
     *
     * @param args Argument expression nodes.
     * @return Evaluated arguments.
     */
    std::vector<Value> evaluate_arguments(const std::vector<ExprPtr> &args);

    /**
     * Assigns a value to an assignment target.
     *
     * @param target Assignment target expression.
     * @param value Value to assign.
     * @param location Source location of the assignment.
     */
    void assign_target(const Expr &target, Value value, SourceLocation location);

    /**
     * Evaluates an expression as a table key.
     *
     * @param expr Key expression.
     * @return Table key.
     */
    TableKey evaluate_table_key(const Expr &expr);

    /**
     * Ensures a value is numeric.
     *
     * @param value Value to check.
     * @param location Source location for diagnostics.
     * @param op Operator requiring a number.
     * @return Numeric value.
     */
    double expect_number(const Value &value, SourceLocation location,
                         const std::string &op);

    /**
     * Returns whether a name is protected.
     *
     * @param name Name to check.
     * @return True when protected.
     */
    bool is_protected(const std::string &name) const;
};

} // namespace plat
