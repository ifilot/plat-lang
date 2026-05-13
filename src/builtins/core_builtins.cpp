#include "builtins/core_builtins.h"

#include "builtins/builtin_registry.h"

#include <istream>
#include <stdexcept>
#include <string_view>

namespace plat {
namespace {

inline constexpr std::string_view kPrintBuiltinAscii = "aafdrokke";
inline constexpr std::string_view kPrintBuiltinComposed = "aafdrökke";
inline constexpr std::string_view kPrintBuiltinDecomposed = "aafdro\xCC\x88kke";
inline constexpr std::string_view kInputBuiltin = "inveure";
inline constexpr std::string_view kTypeBuiltin = "waatis";
inline constexpr std::string_view kNumberTypeComposed = "nómmer";

/**
 * Returns the user-facing type name for a runtime value.
 *
 * @throws std::logic_error when the value variant is not recognized.
 */
std::string_view value_type_name(const Value &value) {
    if (value.is_nil()) {
        return "niks";
    }

    if (value.is_number()) {
        return kNumberTypeComposed;
    }

    if (value.is_bool()) {
        return "woar";
    }

    if (value.is_string()) {
        return "teks";
    }

    if (value.is_table()) {
        return "tabel";
    }

    if (value.is_native_handle()) {
        return value.as_native_handle()->type_name();
    }

    throw std::logic_error("unknown value type");
}

/**
 * Implements the print built-in.
 *
 * Validates that one argument is present and writes its string form to output.
 */
Value call_print_builtin(BuiltinContext &context, const std::string &name,
                         const std::vector<Value> &args,
                         SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 1, location);

    context.output() << args[0].to_string() << '\n';
    return Value();
}

/**
 * Implements the input built-in.
 *
 * Reads one line from input and strips a trailing carriage return when present.
 */
Value call_input_builtin(BuiltinContext &context, const std::string &name,
                         const std::vector<Value> &args,
                         SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 0, location);

    std::string line;
    if (std::getline(context.input(), line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        return Value(line);
    }

    return Value();
}

/**
 * Implements the type-inspection built-in.
 *
 * Writes the user-facing type name of the argument to output.
 */
Value call_type_builtin(BuiltinContext &context, const std::string &name,
                        const std::vector<Value> &args,
                        SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 1, location);

    context.output() << value_type_name(args[0]) << '\n';
    return Value();
}

} // namespace

void register_core_builtins(BuiltinRegistry &registry) {
    registry.register_function(std::string(kPrintBuiltinAscii),
                               call_print_builtin);
    registry.register_function(std::string(kPrintBuiltinComposed),
                               call_print_builtin);
    registry.register_function(std::string(kPrintBuiltinDecomposed),
                               call_print_builtin);
    registry.register_function(std::string(kInputBuiltin), call_input_builtin);
    registry.register_function(std::string(kTypeBuiltin), call_type_builtin);
}

} // namespace plat
