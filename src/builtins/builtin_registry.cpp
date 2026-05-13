#include "builtins/builtin_registry.h"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace plat {

/**
 * Stores references to host services used by native built-ins.
 */
BuiltinContext::BuiltinContext(DiagnosticReporter &diagnostics,
                               std::istream &input, std::ostream &output)
    : diagnostics_(&diagnostics),
      input_(&input),
      output_(&output) {}

/**
 * Returns the diagnostic reporter used by built-ins.
 */
DiagnosticReporter &BuiltinContext::diagnostics() const {
    return *diagnostics_;
}

/**
 * Returns the input stream used by input-oriented built-ins.
 */
std::istream &BuiltinContext::input() const {
    return *input_;
}

/**
 * Returns the output stream used by output-oriented built-ins.
 */
std::ostream &BuiltinContext::output() const {
    return *output_;
}

/**
 * Registers a built-in callable under a protected source-level name.
 */
void BuiltinRegistry::register_function(std::string name,
                                        BuiltinFunction function) {
    functions_[std::move(name)] = std::move(function);
}

/**
 * Checks whether a built-in callable has been registered.
 */
bool BuiltinRegistry::contains(std::string_view name) const {
    return functions_.contains(std::string(name));
}

/**
 * Calls a registered built-in with the supplied runtime context and arguments.
 *
 * @throws std::logic_error when the built-in name is unknown.
 */
Value BuiltinRegistry::call(BuiltinContext &context, const std::string &name,
                            const std::vector<Value> &args,
                            SourceLocation location) const {
    const auto found = functions_.find(name);
    if (found == functions_.end()) {
        throw std::logic_error("unknown built-in function");
    }

    return found->second(context, name, args, location);
}

/**
 * Returns the names of all registered built-ins.
 */
std::vector<std::string> BuiltinRegistry::names() const {
    std::vector<std::string> result;
    result.reserve(functions_.size());

    for (const auto &[name, _] : functions_) {
        result.push_back(name);
    }

    return result;
}

/**
 * Converts an argument count to a string for diagnostics.
 */
std::string builtin_count_to_string(std::size_t value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

/**
 * Reports an arity diagnostic unless the argument count matches exactly.
 */
void require_arity(DiagnosticReporter &diagnostics, const std::string &name,
                   const std::vector<Value> &args, std::size_t expected,
                   SourceLocation location) {
    if (args.size() == expected) {
        return;
    }

    diagnostics.fatal(
        DiagnosticId::ArityMismatch, location,
        {DiagnosticArg("name", name),
         DiagnosticArg("expected", builtin_count_to_string(expected)),
         DiagnosticArg("actual", builtin_count_to_string(args.size()))});
}

} // namespace plat
