#pragma once

#include "diagnostics.h"
#include "value.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace plat {

/**
 * Lexical runtime environment for variable bindings.
 */
class Environment : public std::enable_shared_from_this<Environment> {
private:
    std::shared_ptr<Environment> parent_;
    std::unordered_map<std::string, Value> values_;
    std::shared_ptr<const std::unordered_set<std::string>> protected_names_;
    DiagnosticReporter *diagnostics_;

public:
    /**
     * Creates a root lexical environment.
     *
     * @param protected_names Names that cannot be declared or assigned.
     * @param diagnostics Diagnostic reporter for runtime errors.
     */
    Environment(std::shared_ptr<const std::unordered_set<std::string>> protected_names,
                DiagnosticReporter &diagnostics);

    /**
     * Creates a child lexical environment.
     *
     * @param parent Parent lexical environment.
     */
    explicit Environment(std::shared_ptr<Environment> parent);

    /**
     * Defines a variable in the current scope.
     *
     * @param name Variable name.
     * @param value Initial value.
     * @param location Source location of the declaration.
     */
    void define(const std::string &name, Value value, SourceLocation location);

    /**
     * Assigns an existing variable in this scope chain.
     *
     * @param name Variable name.
     * @param value New value.
     * @param location Source location of the assignment.
     */
    void assign(const std::string &name, Value value, SourceLocation location);

    /**
     * Looks up a variable in this scope chain.
     *
     * @param name Variable name.
     * @param location Source location of the lookup.
     * @return Runtime value.
     */
    Value lookup(const std::string &name, SourceLocation location) const;

    /**
     * Returns whether the current scope contains a variable.
     *
     * @param name Variable name.
     * @return True when the current scope contains the name.
     */
    bool contains_current(const std::string &name) const;

    /**
     * Returns whether any scope contains a variable.
     *
     * @param name Variable name.
     * @return True when any scope contains the name.
     */
    bool contains_anywhere(const std::string &name) const;

    /**
     * Returns whether a name is protected.
     *
     * @param name Name to check.
     * @return True when protected.
     */
    bool is_protected(std::string_view name) const;

    /**
     * Creates a child environment.
     *
     * @return Child environment.
     */
    std::shared_ptr<Environment> make_child();

private:
    /**
     * Reports a fatal protected-name error.
     *
     * @param name Protected name.
     * @param location Source location.
     */
    [[noreturn]] void report_protected(const std::string &name,
                                       SourceLocation location) const;
};

} // namespace plat
