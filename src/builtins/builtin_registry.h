#pragma once

#include "ast.h"
#include "diagnostics.h"
#include "value.h"

#include <functional>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace plat {

/**
 * Shared host services available to native built-ins.
 */
class BuiltinContext {
private:
    DiagnosticReporter *diagnostics_;
    std::istream *input_;
    std::ostream *output_;

public:
    /**
     * Creates a context over the host services used by native built-ins.
     *
     * @param diagnostics Reporter used for runtime diagnostics.
     * @param input Stream read by input-oriented built-ins.
     * @param output Stream written by output-oriented built-ins.
     */
    BuiltinContext(DiagnosticReporter &diagnostics, std::istream &input,
                   std::ostream &output);

    /**
     * Returns the diagnostic reporter for built-in runtime errors.
     */
    DiagnosticReporter &diagnostics() const;

    /**
     * Returns the input stream available to native built-ins.
     */
    std::istream &input() const;

    /**
     * Returns the output stream available to native built-ins.
     */
    std::ostream &output() const;
};

using BuiltinFunction =
    std::function<Value(BuiltinContext &, const std::string &,
                        const std::vector<Value> &, SourceLocation)>;

/**
 * Registry of protected native built-in functions.
 */
class BuiltinRegistry {
private:
    std::unordered_map<std::string, BuiltinFunction> functions_;

public:
    /**
     * Registers or replaces a native built-in function.
     *
     * @param name Protected source-level name of the built-in.
     * @param function Callable used when the built-in is invoked.
     */
    void register_function(std::string name, BuiltinFunction function);

    /**
     * Checks whether a built-in name is registered.
     *
     * @param name Name to look up.
     */
    bool contains(std::string_view name) const;

    /**
     * Invokes a registered built-in function.
     *
     * @param context Host services available to the built-in.
     * @param name Registered built-in name.
     * @param args Runtime arguments passed by the program.
     * @param location Source location used for diagnostics.
     *
     * @throws std::logic_error if the name is not registered.
     */
    Value call(BuiltinContext &context, const std::string &name,
               const std::vector<Value> &args, SourceLocation location) const;

    /**
     * Returns the registered built-in names.
     */
    std::vector<std::string> names() const;
};

/**
 * Converts a count to a diagnostic-friendly string.
 *
 * @param value Count to format.
 */
std::string builtin_count_to_string(std::size_t value);

/**
 * Validates an exact built-in arity.
 *
 * @param diagnostics Reporter used for the arity diagnostic.
 * @param name Built-in name being validated.
 * @param args Runtime arguments passed by the program.
 * @param expected Required argument count.
 * @param location Source location used for diagnostics.
 */
void require_arity(DiagnosticReporter &diagnostics, const std::string &name,
                   const std::vector<Value> &args, std::size_t expected,
                   SourceLocation location);

} // namespace plat
