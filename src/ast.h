#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace plat {

/**
 * Represents a one-based source position for diagnostics and AST nodes.
 */
class SourceLocation {
private:
    int line_ = 1;
    int column_ = 1;

public:
    /**
     * Creates a source location.
     *
     * @param line One-based source line.
     * @param column One-based source column.
     */
    SourceLocation(int line = 1, int column = 1)
        : line_(line), column_(column) {}

    /**
     * Returns the one-based source line.
     *
     * @return Source line.
     */
    int line() const { return line_; }

    /**
     * Returns the one-based source column.
     *
     * @return Source column.
     */
    int column() const { return column_; }
};

/**
 * Base class for every AST node.
 */
class Node {
private:
    SourceLocation location_;

public:
    /**
     * Creates an AST node at a source location.
     *
     * @param location Source location associated with the node.
     */
    explicit Node(SourceLocation location) : location_(location) {}

    /**
     * Destroys the AST node.
     */
    virtual ~Node() = default;

    /**
     * Returns the source location associated with this node.
     *
     * @return Source location.
     */
    const SourceLocation &location() const { return location_; }
};

/**
 * Base class for all expression AST nodes.
 */
class Expr : public Node {
public:
    using Node::Node;
};

/**
 * Base class for all statement AST nodes.
 */
class Stmt : public Node {
public:
    using Node::Node;
};

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

/**
 * Represents a numeric literal expression.
 */
class NumberExpr final : public Expr {
private:
    double value_;

public:
    /**
     * Creates a numeric literal expression.
     *
     * @param location Source location of the literal.
     * @param value Numeric value.
     */
    NumberExpr(SourceLocation location, double value)
        : Expr(location), value_(value) {}

    /**
     * Returns the numeric value.
     *
     * @return Numeric value.
     */
    double value() const { return value_; }
};

/**
 * Represents a string literal expression.
 */
class StringExpr final : public Expr {
private:
    std::string value_;

public:
    /**
     * Creates a string literal expression.
     *
     * @param location Source location of the literal.
     * @param value String value.
     */
    StringExpr(SourceLocation location, std::string value)
        : Expr(location), value_(std::move(value)) {}

    /**
     * Returns the string value.
     *
     * @return String value.
     */
    const std::string &value() const { return value_; }
};

/**
 * Represents a boolean literal expression.
 */
class BoolExpr final : public Expr {
private:
    bool value_;

public:
    /**
     * Creates a boolean literal expression.
     *
     * @param location Source location of the literal.
     * @param value Boolean value.
     */
    BoolExpr(SourceLocation location, bool value)
        : Expr(location), value_(value) {}

    /**
     * Returns the boolean value.
     *
     * @return Boolean value.
     */
    bool value() const { return value_; }
};

/**
 * Represents the `niks` literal expression.
 */
class NilExpr final : public Expr {
public:
    /**
     * Creates a `niks` literal expression.
     *
     * @param location Source location of the literal.
     */
    explicit NilExpr(SourceLocation location) : Expr(location) {}
};

/**
 * Represents a variable or symbol reference expression.
 */
class IdentifierExpr final : public Expr {
private:
    std::string name_;

public:
    /**
     * Creates an identifier expression.
     *
     * @param location Source location of the identifier.
     * @param name Referenced name.
     */
    IdentifierExpr(SourceLocation location, std::string name)
        : Expr(location), name_(std::move(name)) {}

    /**
     * Returns the referenced name.
     *
     * @return Identifier name.
     */
    const std::string &name() const { return name_; }
};

/**
 * Represents a unary operator expression.
 */
class UnaryExpr final : public Expr {
private:
    std::string op_;
    ExprPtr operand_;

public:
    /**
     * Creates a unary expression.
     *
     * @param location Source location of the operator.
     * @param op Operator spelling.
     * @param operand Operand expression.
     */
    UnaryExpr(SourceLocation location, std::string op, ExprPtr operand)
        : Expr(location), op_(std::move(op)), operand_(std::move(operand)) {}

    /**
     * Returns the operator spelling.
     *
     * @return Operator spelling.
     */
    const std::string &op() const { return op_; }

    /**
     * Returns the operand expression.
     *
     * @return Operand expression.
     */
    const Expr &operand() const { return *operand_; }
};

/**
 * Represents a binary operator expression.
 */
class BinaryExpr final : public Expr {
private:
    ExprPtr left_;
    std::string op_;
    ExprPtr right_;

public:
    /**
     * Creates a binary expression.
     *
     * @param location Source location of the operator.
     * @param left Left-hand expression.
     * @param op Operator spelling.
     * @param right Right-hand expression.
     */
    BinaryExpr(SourceLocation location, ExprPtr left, std::string op,
               ExprPtr right)
        : Expr(location),
          left_(std::move(left)),
          op_(std::move(op)),
          right_(std::move(right)) {}

    /**
     * Returns the left-hand expression.
     *
     * @return Left-hand expression.
     */
    const Expr &left() const { return *left_; }

    /**
     * Returns the operator spelling.
     *
     * @return Operator spelling.
     */
    const std::string &op() const { return op_; }

    /**
     * Returns the right-hand expression.
     *
     * @return Right-hand expression.
     */
    const Expr &right() const { return *right_; }
};

/**
 * Represents a global function call expression.
 */
class CallExpr final : public Expr {
private:
    std::string callee_;
    std::vector<ExprPtr> args_;

public:
    /**
     * Creates a function call expression.
     *
     * @param location Source location of the call.
     * @param callee Called function name.
     * @param args Call arguments.
     */
    CallExpr(SourceLocation location, std::string callee,
             std::vector<ExprPtr> args)
        : Expr(location), callee_(std::move(callee)), args_(std::move(args)) {}

    /**
     * Returns the called function name.
     *
     * @return Called function name.
     */
    const std::string &callee() const { return callee_; }

    /**
     * Returns the call arguments.
     *
     * @return Call arguments.
     */
    const std::vector<ExprPtr> &args() const { return args_; }
};

/**
 * Represents table index access.
 */
class IndexExpr final : public Expr {
private:
    ExprPtr object_;
    ExprPtr index_;

public:
    /**
     * Creates an index expression.
     *
     * @param location Source location of the index operator.
     * @param object Object being indexed.
     * @param index Index expression.
     */
    IndexExpr(SourceLocation location, ExprPtr object, ExprPtr index)
        : Expr(location), object_(std::move(object)), index_(std::move(index)) {}

    /**
     * Returns the indexed object.
     *
     * @return Indexed object.
     */
    const Expr &object() const { return *object_; }

    /**
     * Returns the index expression.
     *
     * @return Index expression.
     */
    const Expr &index() const { return *index_; }
};

/**
 * Represents table field access with dot syntax.
 */
class FieldExpr final : public Expr {
private:
    ExprPtr object_;
    std::string field_;

public:
    /**
     * Creates a field access expression.
     *
     * @param location Source location of the dot operator.
     * @param object Object being accessed.
     * @param field Field name.
     */
    FieldExpr(SourceLocation location, ExprPtr object, std::string field)
        : Expr(location), object_(std::move(object)), field_(std::move(field)) {}

    /**
     * Returns the accessed object.
     *
     * @return Accessed object.
     */
    const Expr &object() const { return *object_; }

    /**
     * Returns the field name.
     *
     * @return Field name.
     */
    const std::string &field() const { return field_; }
};

/**
 * Represents one field inside a table literal.
 */
class TableField {
private:
    SourceLocation location_;
    std::optional<ExprPtr> key_;
    ExprPtr value_;

public:
    /**
     * Creates a table field.
     *
     * @param location Source location of the field.
     * @param key Optional explicit key.
     * @param value Field value.
     */
    TableField(SourceLocation location, std::optional<ExprPtr> key,
               ExprPtr value)
        : location_(location), key_(std::move(key)), value_(std::move(value)) {}

    /**
     * Returns the source location of the field.
     *
     * @return Source location.
     */
    const SourceLocation &location() const { return location_; }

    /**
     * Returns whether this field has an explicit key.
     *
     * @return True when an explicit key is present.
     */
    bool has_key() const { return key_.has_value(); }

    /**
     * Returns the explicit key expression.
     *
     * @return Explicit key expression.
     */
    const Expr &key() const { return **key_; }

    /**
     * Returns the field value expression.
     *
     * @return Field value expression.
     */
    const Expr &value() const { return *value_; }
};

/**
 * Represents a table literal expression.
 */
class TableExpr final : public Expr {
private:
    std::vector<TableField> fields_;

public:
    /**
     * Creates a table literal expression.
     *
     * @param location Source location of the opening brace.
     * @param fields Table fields.
     */
    TableExpr(SourceLocation location, std::vector<TableField> fields)
        : Expr(location), fields_(std::move(fields)) {}

    /**
     * Returns the table fields.
     *
     * @return Table fields.
     */
    const std::vector<TableField> &fields() const { return fields_; }
};

/**
 * Represents an `es` conditional statement.
 */
class IfStmt final : public Stmt {
private:
    ExprPtr condition_;
    std::vector<StmtPtr> then_body_;
    std::vector<StmtPtr> else_body_;

public:
    /**
     * Creates an `es` conditional statement.
     *
     * @param location Source location of the `es` keyword.
     * @param condition Condition expression.
     * @param then_body Statements evaluated when the condition is truthy.
     * @param else_body Statements evaluated when the condition is falsy.
     */
    IfStmt(SourceLocation location, ExprPtr condition,
           std::vector<StmtPtr> then_body, std::vector<StmtPtr> else_body)
        : Stmt(location),
          condition_(std::move(condition)),
          then_body_(std::move(then_body)),
          else_body_(std::move(else_body)) {}

    /**
     * Returns the condition expression.
     *
     * @return Condition expression.
     */
    const Expr &condition() const { return *condition_; }

    /**
     * Returns the truthy branch body.
     *
     * @return Truthy branch body.
     */
    const std::vector<StmtPtr> &then_body() const { return then_body_; }

    /**
     * Returns the falsy branch body.
     *
     * @return Falsy branch body.
     */
    const std::vector<StmtPtr> &else_body() const { return else_body_; }
};

/**
 * Represents a `loat` variable declaration.
 */
class VarDeclStmt final : public Stmt {
private:
    std::string name_;
    ExprPtr initializer_;

public:
    /**
     * Creates a variable declaration statement.
     *
     * @param location Source location of the `loat` keyword.
     * @param name Declared variable name.
     * @param initializer Initial value expression.
     */
    VarDeclStmt(SourceLocation location, std::string name, ExprPtr initializer)
        : Stmt(location),
          name_(std::move(name)),
          initializer_(std::move(initializer)) {}

    /**
     * Returns the declared variable name.
     *
     * @return Variable name.
     */
    const std::string &name() const { return name_; }

    /**
     * Returns the initializer expression.
     *
     * @return Initializer expression.
     */
    const Expr &initializer() const { return *initializer_; }
};

/**
 * Represents an assignment statement.
 */
class AssignStmt final : public Stmt {
private:
    ExprPtr target_;
    ExprPtr value_;

public:
    /**
     * Creates an assignment statement.
     *
     * @param location Source location of the assignment target.
     * @param target Assignment target.
     * @param value Assigned value expression.
     */
    AssignStmt(SourceLocation location, ExprPtr target, ExprPtr value)
        : Stmt(location), target_(std::move(target)), value_(std::move(value)) {}

    /**
     * Returns the assignment target.
     *
     * @return Assignment target.
     */
    const Expr &target() const { return *target_; }

    /**
     * Returns the assigned value expression.
     *
     * @return Assigned value expression.
     */
    const Expr &value() const { return *value_; }
};

/**
 * Represents an expression used as a statement.
 */
class ExprStmt final : public Stmt {
private:
    ExprPtr expr_;

public:
    /**
     * Creates an expression statement.
     *
     * @param location Source location of the expression.
     * @param expr Expression being evaluated.
     */
    ExprStmt(SourceLocation location, ExprPtr expr)
        : Stmt(location), expr_(std::move(expr)) {}

    /**
     * Returns the statement expression.
     *
     * @return Statement expression.
     */
    const Expr &expr() const { return *expr_; }
};

/**
 * Represents a `trok` return statement.
 */
class ReturnStmt final : public Stmt {
private:
    ExprPtr value_;

public:
    /**
     * Creates a return statement.
     *
     * @param location Source location of the `trok` keyword.
     * @param value Returned value expression.
     */
    ReturnStmt(SourceLocation location, ExprPtr value)
        : Stmt(location), value_(std::move(value)) {}

    /**
     * Returns the returned value expression.
     *
     * @return Returned value expression.
     */
    const Expr &value() const { return *value_; }
};

/**
 * Represents an `aafbraeke` loop-exit statement.
 */
class BreakStmt final : public Stmt {
public:
    /**
     * Creates a break statement.
     *
     * @param location Source location of the `aafbraeke` keyword.
     */
    explicit BreakStmt(SourceLocation location) : Stmt(location) {}
};

/**
 * Represents a `euversjlaon` loop-continue statement.
 */
class ContinueStmt final : public Stmt {
public:
    /**
     * Creates a continue statement.
     *
     * @param location Source location of the `euversjlaon` keyword.
     */
    explicit ContinueStmt(SourceLocation location) : Stmt(location) {}
};

/**
 * Represents a `zolang` loop statement.
 */
class WhileStmt final : public Stmt {
private:
    ExprPtr condition_;
    std::vector<StmtPtr> body_;

public:
    /**
     * Creates a while-loop statement.
     *
     * @param location Source location of the `zolang` keyword.
     * @param condition Loop condition expression.
     * @param body Loop body statements.
     */
    WhileStmt(SourceLocation location, ExprPtr condition,
              std::vector<StmtPtr> body)
        : Stmt(location),
          condition_(std::move(condition)),
          body_(std::move(body)) {}

    /**
     * Returns the loop condition.
     *
     * @return Loop condition.
     */
    const Expr &condition() const { return *condition_; }

    /**
     * Returns the loop body.
     *
     * @return Loop body statements.
     */
    const std::vector<StmtPtr> &body() const { return body_; }
};

/**
 * Represents a half-open `veur` numeric loop statement.
 */
class ForStmt final : public Stmt {
private:
    std::string name_;
    ExprPtr begin_;
    ExprPtr end_;
    std::vector<StmtPtr> body_;

public:
    /**
     * Creates a numeric for-loop statement.
     *
     * @param location Source location of the `veur` keyword.
     * @param name Loop variable name.
     * @param begin Inclusive start expression.
     * @param end Exclusive end expression.
     * @param body Loop body statements.
     */
    ForStmt(SourceLocation location, std::string name, ExprPtr begin,
            ExprPtr end, std::vector<StmtPtr> body)
        : Stmt(location),
          name_(std::move(name)),
          begin_(std::move(begin)),
          end_(std::move(end)),
          body_(std::move(body)) {}

    /**
     * Returns the loop variable name.
     *
     * @return Loop variable name.
     */
    const std::string &name() const { return name_; }

    /**
     * Returns the inclusive start expression.
     *
     * @return Start expression.
     */
    const Expr &begin() const { return *begin_; }

    /**
     * Returns the exclusive end expression.
     *
     * @return End expression.
     */
    const Expr &end() const { return *end_; }

    /**
     * Returns the loop body.
     *
     * @return Loop body statements.
     */
    const std::vector<StmtPtr> &body() const { return body_; }
};

/**
 * Represents a global `funksie` declaration.
 */
class FunctionDecl final : public Stmt {
private:
    std::string name_;
    std::vector<std::string> params_;
    std::vector<StmtPtr> body_;

public:
    /**
     * Creates a function declaration.
     *
     * @param location Source location of the `funksie` keyword.
     * @param name Function name.
     * @param params Function parameter names.
     * @param body Function body statements.
     */
    FunctionDecl(SourceLocation location, std::string name,
                 std::vector<std::string> params, std::vector<StmtPtr> body)
        : Stmt(location),
          name_(std::move(name)),
          params_(std::move(params)),
          body_(std::move(body)) {}

    /**
     * Returns the function name.
     *
     * @return Function name.
     */
    const std::string &name() const { return name_; }

    /**
     * Returns the function parameter names.
     *
     * @return Function parameter names.
     */
    const std::vector<std::string> &params() const { return params_; }

    /**
     * Returns the function body.
     *
     * @return Function body statements.
     */
    const std::vector<StmtPtr> &body() const { return body_; }
};

/**
 * Represents a complete parsed plat-lang source file.
 */
class Program final : public Node {
private:
    std::vector<StmtPtr> statements_;

public:
    /**
     * Creates a parsed program.
     *
     * @param location Source location of the program.
     * @param statements Top-level statements and function declarations.
     */
    Program(SourceLocation location, std::vector<StmtPtr> statements)
        : Node(location), statements_(std::move(statements)) {}

    /**
     * Returns the top-level statements.
     *
     * @return Top-level statements.
     */
    const std::vector<StmtPtr> &statements() const { return statements_; }
};

} // namespace plat
