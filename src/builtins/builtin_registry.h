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
    BuiltinContext(DiagnosticReporter &diagnostics, std::istream &input,
                   std::ostream &output);

    DiagnosticReporter &diagnostics() const;
    std::istream &input() const;
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
    void register_function(std::string name, BuiltinFunction function);
    bool contains(std::string_view name) const;
    Value call(BuiltinContext &context, const std::string &name,
               const std::vector<Value> &args, SourceLocation location) const;
    std::vector<std::string> names() const;
};

/**
 * Converts a count to a diagnostic-friendly string.
 */
std::string builtin_count_to_string(std::size_t value);

/**
 * Validates an exact built-in arity.
 */
void require_arity(DiagnosticReporter &diagnostics, const std::string &name,
                   const std::vector<Value> &args, std::size_t expected,
                   SourceLocation location);

} // namespace plat
