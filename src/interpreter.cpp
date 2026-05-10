#include "interpreter.h"

#include <cmath>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace plat {
namespace {

inline constexpr std::string_view kPrintBuiltinAscii = "aafdrokke";
inline constexpr std::string_view kPrintBuiltinComposed = "aafdrökke";
inline constexpr std::string_view kPrintBuiltinDecomposed = "aafdro\xCC\x88kke";
inline constexpr std::string_view kInputBuiltin = "invuier";
inline constexpr std::string_view kNumberTypeAscii = "nommer";
inline constexpr std::string_view kNumberTypeComposed = "nómmer";
inline constexpr std::string_view kNumberTypeDecomposed = "no\xCC\x81mmer";

/**
 * Internal signal used to implement `trok`.
 */
class ReturnSignal {
private:
    Value value_;

public:
    /**
     * Creates a return signal.
     *
     * @param value Returned value.
     */
    explicit ReturnSignal(Value value) : value_(std::move(value)) {}

    /**
     * Returns the carried value.
     *
     * @return Returned value.
     */
    const Value &value() const { return value_; }
};

/**
 * Internal signal used to implement `aafbraeke`.
 */
class BreakSignal {};

/**
 * Internal signal used to implement `euversjlaon`.
 */
class ContinueSignal {};

/**
 * Creates the protected name set for the language.
 *
 * @return Protected names.
 */
std::shared_ptr<const std::unordered_set<std::string>> make_protected_names() {
    return std::make_shared<const std::unordered_set<std::string>>(
        std::unordered_set<std::string>{
            "aafbraeke", std::string(kPrintBuiltinAscii),
            std::string(kPrintBuiltinComposed),
            std::string(kPrintBuiltinDecomposed), "angesj", "enj", "es",
            "funksie", std::string(kInputBuiltin), "loat", "portefeuil",
            "neetwoar", "niks", std::string(kNumberTypeAscii),
            std::string(kNumberTypeComposed),
            std::string(kNumberTypeDecomposed), "teks", "trok", "veur",
            "euversjlaon", "woar", "zolang"});
}

/**
 * Returns whether a name refers to the print built-in.
 *
 * @param name Candidate function name.
 * @return True when the name is a supported alias.
 */
bool is_print_builtin_name(std::string_view name) {
    return name == kPrintBuiltinAscii || name == kPrintBuiltinComposed ||
           name == kPrintBuiltinDecomposed;
}

/**
 * Returns whether a name refers to the input built-in.
 *
 * @param name Candidate function name.
 * @return True when the name is a supported alias.
 */
bool is_input_builtin_name(std::string_view name) {
    return name == kInputBuiltin;
}

/**
 * Converts a number to a string for diagnostics.
 *
 * @param value Number value.
 * @return String representation.
 */
std::string number_to_string(std::size_t value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

} // namespace

Interpreter::Interpreter(DiagnosticReporter &diagnostics, std::istream &input,
                         std::ostream &output)
    : diagnostics_(&diagnostics),
      input_(&input),
      output_(&output),
      protected_names_(make_protected_names()) {
    global_environment_ =
        std::make_shared<Environment>(protected_names_, diagnostics);
    environment_ = global_environment_;
}

void Interpreter::execute_program(const Program &program) {
    try {
        register_top_level_functions(program);

        for (const StmtPtr &stmt : program.statements()) {
            if (dynamic_cast<const FunctionDecl *>(stmt.get()) == nullptr) {
                execute_stmt(*stmt);
            }
        }
    } catch (const ReturnSignal &) {
        diagnostics_->fatal(DiagnosticId::ReturnOutsideFunction,
                            program.location());
    } catch (const BreakSignal &) {
        diagnostics_->fatal(DiagnosticId::BreakOutsideLoop, program.location());
    } catch (const ContinueSignal &) {
        diagnostics_->fatal(DiagnosticId::ContinueOutsideLoop,
                            program.location());
    }
}

void Interpreter::register_top_level_functions(const Program &program) {
    for (const StmtPtr &stmt : program.statements()) {
        if (const auto *function = dynamic_cast<const FunctionDecl *>(stmt.get())) {
            register_function(*function);
        }
    }
}

void Interpreter::execute_stmt(const Stmt &stmt) {
    if (const auto *node = dynamic_cast<const FunctionDecl *>(&stmt)) {
        diagnostics_->fatal(DiagnosticId::FunctionDeclarationInBlock,
                            node->location());
    }

    if (const auto *node = dynamic_cast<const VarDeclStmt *>(&stmt)) {
        if (environment_ == global_environment_ && functions_.contains(node->name())) {
            diagnostics_->fatal(DiagnosticId::RedeclaredName, node->location(),
                                {DiagnosticArg("name", node->name())});
        }

        environment_->define(node->name(), evaluate_expr(node->initializer()),
                             node->location());
        return;
    }

    if (const auto *node = dynamic_cast<const AssignStmt *>(&stmt)) {
        assign_target(node->target(), evaluate_expr(node->value()),
                      node->location());
        return;
    }

    if (const auto *node = dynamic_cast<const ExprStmt *>(&stmt)) {
        evaluate_expr(node->expr());
        return;
    }

    if (const auto *node = dynamic_cast<const ReturnStmt *>(&stmt)) {
        if (function_depth_ == 0) {
            diagnostics_->fatal(DiagnosticId::ReturnOutsideFunction,
                                node->location());
        }

        throw ReturnSignal(evaluate_expr(node->value()));
    }

    if (const auto *node = dynamic_cast<const BreakStmt *>(&stmt)) {
        if (loop_depth_ == 0) {
            diagnostics_->fatal(DiagnosticId::BreakOutsideLoop, node->location());
        }

        throw BreakSignal();
    }

    if (const auto *node = dynamic_cast<const ContinueStmt *>(&stmt)) {
        if (loop_depth_ == 0) {
            diagnostics_->fatal(DiagnosticId::ContinueOutsideLoop,
                                node->location());
        }

        throw ContinueSignal();
    }

    if (const auto *node = dynamic_cast<const IfStmt *>(&stmt)) {
        if (evaluate_expr(node->condition()).is_truthy()) {
            execute_scoped_block(node->then_body());
        } else {
            execute_scoped_block(node->else_body());
        }
        return;
    }

    if (const auto *node = dynamic_cast<const WhileStmt *>(&stmt)) {
        ++loop_depth_;
        try {
            while (evaluate_expr(node->condition()).is_truthy()) {
                try {
                    execute_scoped_block(node->body());
                } catch (const ContinueSignal &) {
                    continue;
                } catch (const BreakSignal &) {
                    break;
                }
            }
        } catch (...) {
            --loop_depth_;
            throw;
        }
        --loop_depth_;
        return;
    }

    if (const auto *node = dynamic_cast<const ForStmt *>(&stmt)) {
        const double begin = expect_number(evaluate_expr(node->begin()),
                                           node->begin().location(), "veur");
        const double end = expect_number(evaluate_expr(node->end()),
                                         node->end().location(), "veur");

        ++loop_depth_;
        try {
            for (double i = begin; i < end; ++i) {
                auto loop_environment = environment_->make_child();
                auto previous = environment_;
                environment_ = loop_environment;
                environment_->define(node->name(), Value(i), node->location());

                try {
                    execute_current_block(node->body());
                } catch (const ContinueSignal &) {
                    environment_ = previous;
                    continue;
                } catch (const BreakSignal &) {
                    environment_ = previous;
                    break;
                } catch (...) {
                    environment_ = previous;
                    throw;
                }

                environment_ = previous;
            }
        } catch (...) {
            --loop_depth_;
            throw;
        }
        --loop_depth_;
        return;
    }

    throw std::logic_error("unknown statement node");
}

void Interpreter::execute_scoped_block(const std::vector<StmtPtr> &statements) {
    auto previous = environment_;
    environment_ = environment_->make_child();

    try {
        execute_current_block(statements);
    } catch (...) {
        environment_ = previous;
        throw;
    }

    environment_ = previous;
}

void Interpreter::execute_current_block(const std::vector<StmtPtr> &statements) {
    for (const StmtPtr &stmt : statements) {
        execute_stmt(*stmt);
    }
}

Value Interpreter::evaluate_expr(const Expr &expr) {
    if (const auto *node = dynamic_cast<const NumberExpr *>(&expr)) {
        return Value(node->value());
    }

    if (const auto *node = dynamic_cast<const StringExpr *>(&expr)) {
        return Value(node->value());
    }

    if (const auto *node = dynamic_cast<const BoolExpr *>(&expr)) {
        return Value(node->value());
    }

    if (dynamic_cast<const NilExpr *>(&expr) != nullptr) {
        return Value();
    }

    if (const auto *node = dynamic_cast<const IdentifierExpr *>(&expr)) {
        return environment_->lookup(node->name(), node->location());
    }

    if (const auto *node = dynamic_cast<const UnaryExpr *>(&expr)) {
        const Value value = evaluate_expr(node->operand());
        if (node->op() == "-") {
            return Value(-expect_number(value, node->location(), node->op()));
        }

        if (node->op() == "!") {
            return Value(!value.is_truthy());
        }
    }

    if (const auto *node = dynamic_cast<const BinaryExpr *>(&expr)) {
        if (node->op() == "en") {
            return Value(evaluate_expr(node->left()).is_truthy() &&
                         evaluate_expr(node->right()).is_truthy());
        }

        if (node->op() == "of") {
            return Value(evaluate_expr(node->left()).is_truthy() ||
                         evaluate_expr(node->right()).is_truthy());
        }

        const Value left = evaluate_expr(node->left());
        const Value right = evaluate_expr(node->right());

        if (node->op() == "==") {
            return Value(values_equal(left, right));
        }

        if (node->op() == "!=") {
            return Value(!values_equal(left, right));
        }

        if (node->op() == "+") {
            if (left.is_string() || right.is_string()) {
                return Value(left.to_string() + right.to_string());
            }

            return Value(expect_number(left, node->location(), "+") +
                         expect_number(right, node->location(), "+"));
        }

        if (node->op() == "-") {
            return Value(expect_number(left, node->location(), "-") -
                         expect_number(right, node->location(), "-"));
        }

        if (node->op() == "*") {
            return Value(expect_number(left, node->location(), "*") *
                         expect_number(right, node->location(), "*"));
        }

        if (node->op() == "/") {
            const double divisor = expect_number(right, node->location(), "/");
            if (divisor == 0.0) {
                diagnostics_->fatal(DiagnosticId::DivisionByZero,
                                    node->location());
            }

            return Value(expect_number(left, node->location(), "/") / divisor);
        }

        if (node->op() == "%") {
            const double divisor = expect_number(right, node->location(), "%");
            if (divisor == 0.0) {
                diagnostics_->fatal(DiagnosticId::DivisionByZero,
                                    node->location());
            }

            return Value(std::fmod(expect_number(left, node->location(), "%"),
                                   divisor));
        }

        if (node->op() == "<") {
            return Value(expect_number(left, node->location(), "<") <
                         expect_number(right, node->location(), "<"));
        }

        if (node->op() == ">") {
            return Value(expect_number(left, node->location(), ">") >
                         expect_number(right, node->location(), ">"));
        }

        if (node->op() == "<=") {
            return Value(expect_number(left, node->location(), "<=") <=
                         expect_number(right, node->location(), "<="));
        }

        if (node->op() == ">=") {
            return Value(expect_number(left, node->location(), ">=") >=
                         expect_number(right, node->location(), ">="));
        }
    }

    if (const auto *node = dynamic_cast<const CallExpr *>(&expr)) {
        return call_function(*node);
    }

    if (const auto *node = dynamic_cast<const TableExpr *>(&expr)) {
        auto table = std::make_shared<TableValue>();
        double index = 0.0;

        for (const TableField &field : node->fields()) {
            TableKey key;
            if (field.has_key()) {
                key = evaluate_table_key(field.key());
            } else {
                key = TableKey(index);
                index += 1.0;
            }

            table->set(key, evaluate_expr(field.value()));
        }

        return Value(table);
    }

    if (const auto *node = dynamic_cast<const IndexExpr *>(&expr)) {
        const Value object = evaluate_expr(node->object());
        if (!object.is_table()) {
            diagnostics_->fatal(DiagnosticId::InvalidOperand, node->location(),
                                {DiagnosticArg("operator", "[]")});
        }

        return object.as_table()->get(evaluate_table_key(node->index()));
    }

    if (const auto *node = dynamic_cast<const FieldExpr *>(&expr)) {
        const Value object = evaluate_expr(node->object());
        if (!object.is_table()) {
            diagnostics_->fatal(DiagnosticId::InvalidOperand, node->location(),
                                {DiagnosticArg("operator", ".")});
        }

        return object.as_table()->get(TableKey(node->field()));
    }

    throw std::logic_error("unknown expression node");
}

void Interpreter::register_function(const FunctionDecl &function) {
    if (is_protected(function.name())) {
        diagnostics_->fatal(DiagnosticId::ProtectedName, function.location(),
                            {DiagnosticArg("name", function.name())});
    }

    if (functions_.contains(function.name()) ||
        global_environment_->contains_current(function.name())) {
        diagnostics_->fatal(DiagnosticId::FunctionRedefinition,
                            function.location(),
                            {DiagnosticArg("name", function.name())});
    }

    functions_[function.name()] = &function;
}

Value Interpreter::call_function(const CallExpr &call) {
    std::vector<Value> args = evaluate_arguments(call.args());

    if (is_print_builtin_name(call.callee())) {
        return call_print_builtin(call.callee(), args, call.location());
    }

    if (is_input_builtin_name(call.callee())) {
        return call_input_builtin(call.callee(), args, call.location());
    }

    const auto found = functions_.find(call.callee());
    if (found == functions_.end()) {
        diagnostics_->fatal(DiagnosticId::UnknownFunction, call.location(),
                            {DiagnosticArg("name", call.callee())});
    }

    const FunctionDecl &function = *found->second;
    if (args.size() != function.params().size()) {
        diagnostics_->fatal(
            DiagnosticId::ArityMismatch, call.location(),
            {DiagnosticArg("name", function.name()),
             DiagnosticArg("expected", number_to_string(function.params().size())),
             DiagnosticArg("actual", number_to_string(args.size()))});
    }

    auto previous = environment_;
    auto function_environment = std::make_shared<Environment>(global_environment_);
    environment_ = function_environment;
    ++function_depth_;

    for (std::size_t i = 0; i < args.size(); ++i) {
        environment_->define(function.params()[i], args[i], function.location());
    }

    try {
        execute_current_block(function.body());
    } catch (const ReturnSignal &signal) {
        --function_depth_;
        environment_ = previous;
        return signal.value();
    } catch (...) {
        --function_depth_;
        environment_ = previous;
        throw;
    }

    --function_depth_;
    environment_ = previous;
    return Value();
}

Value Interpreter::call_print_builtin(const std::string &name,
                                      const std::vector<Value> &args,
                                      SourceLocation location) {
    if (args.size() != 1) {
        diagnostics_->fatal(
            DiagnosticId::ArityMismatch, location,
            {DiagnosticArg("name", name),
             DiagnosticArg("expected", "1"),
             DiagnosticArg("actual", number_to_string(args.size()))});
    }

    *output_ << args[0].to_string() << '\n';
    return Value();
}

Value Interpreter::call_input_builtin(const std::string &name,
                                      const std::vector<Value> &args,
                                      SourceLocation location) {
    if (!args.empty()) {
        diagnostics_->fatal(
            DiagnosticId::ArityMismatch, location,
            {DiagnosticArg("name", name),
             DiagnosticArg("expected", "0"),
             DiagnosticArg("actual", number_to_string(args.size()))});
    }

    std::string line;
    if (std::getline(*input_, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        return Value(line);
    }

    return Value();
}

std::vector<Value> Interpreter::evaluate_arguments(
    const std::vector<ExprPtr> &args) {
    std::vector<Value> values;
    values.reserve(args.size());

    for (const ExprPtr &arg : args) {
        values.push_back(evaluate_expr(*arg));
    }

    return values;
}

void Interpreter::assign_target(const Expr &target, Value value,
                                SourceLocation location) {
    if (const auto *node = dynamic_cast<const IdentifierExpr *>(&target)) {
        environment_->assign(node->name(), std::move(value), location);
        return;
    }

    if (const auto *node = dynamic_cast<const FieldExpr *>(&target)) {
        const Value object = evaluate_expr(node->object());
        if (!object.is_table()) {
            diagnostics_->fatal(DiagnosticId::InvalidAssignmentTarget,
                                node->location());
        }

        object.as_table()->set(TableKey(node->field()), std::move(value));
        return;
    }

    if (const auto *node = dynamic_cast<const IndexExpr *>(&target)) {
        const Value object = evaluate_expr(node->object());
        if (!object.is_table()) {
            diagnostics_->fatal(DiagnosticId::InvalidAssignmentTarget,
                                node->location());
        }

        object.as_table()->set(evaluate_table_key(node->index()),
                               std::move(value));
        return;
    }

    diagnostics_->fatal(DiagnosticId::InvalidAssignmentTarget, location);
}

TableKey Interpreter::evaluate_table_key(const Expr &expr) {
    const Value value = evaluate_expr(expr);
    TableKey key;
    if (!value_to_table_key(value, key)) {
        diagnostics_->fatal(DiagnosticId::InvalidTableKey, expr.location());
    }

    return key;
}

double Interpreter::expect_number(const Value &value, SourceLocation location,
                                  const std::string &op) {
    if (!value.is_number()) {
        diagnostics_->fatal(DiagnosticId::InvalidOperand, location,
                            {DiagnosticArg("operator", op)});
    }

    return value.as_number();
}

bool Interpreter::is_protected(const std::string &name) const {
    return protected_names_->contains(name);
}

} // namespace plat
