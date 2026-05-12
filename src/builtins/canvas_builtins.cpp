#include "builtins/canvas_builtins.h"

#include "builtins/builtin_registry.h"
#include "canvas/canvas_service.h"

#include <array>
#include <cctype>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace plat {
namespace {

constexpr std::string_view kCanvasName = "canvas";
constexpr std::string_view kClearName = "canvas_wis";
constexpr std::string_view kPresentName = "canvas_toon";
constexpr std::string_view kWaitName = "canvas_wach";
constexpr std::string_view kPauseName = "canvas_pauze";
constexpr std::string_view kIsOpenName = "canvas_ope";
constexpr std::string_view kCloseName = "canvas_sloet";
constexpr std::string_view kLineName = "canvas_lien";
constexpr std::string_view kRectName = "canvas_rechhook";
constexpr std::string_view kCircleName = "canvas_sirkel";
constexpr std::string_view kTextName = "canvas_teks";

CanvasColor black() {
    return CanvasColor{0, 0, 0, 255};
}

std::string value_kind(const Value &value) {
    if (value.is_nil()) {
        return "niks";
    }
    if (value.is_number()) {
        return "nómmer";
    }
    if (value.is_bool()) {
        return "woar";
    }
    if (value.is_string()) {
        return "teks";
    }
    if (value.is_table()) {
        return "portefeuil";
    }
    if (value.is_native_handle()) {
        return value.as_native_handle()->type_name();
    }
    return "waarde";
}

[[noreturn]] void runtime_error(DiagnosticReporter &diagnostics,
                                SourceLocation location,
                                const std::string &message) {
    diagnostics.fatal(DiagnosticId::RuntimeError, location,
                      {DiagnosticArg("message", message)});
}

void require_arity_range(DiagnosticReporter &diagnostics,
                         const std::string &name,
                         const std::vector<Value> &args,
                         std::size_t minimum, std::size_t maximum,
                         SourceLocation location) {
    if (args.size() >= minimum && args.size() <= maximum) {
        return;
    }

    std::ostringstream expected;
    expected << minimum;
    if (minimum != maximum) {
        expected << "-" << maximum;
    }

    diagnostics.fatal(
        DiagnosticId::ArityMismatch, location,
        {DiagnosticArg("name", name),
         DiagnosticArg("expected", expected.str()),
         DiagnosticArg("actual", builtin_count_to_string(args.size()))});
}

double expect_number(BuiltinContext &context, const Value &value,
                     const std::string &name, std::size_t index,
                     SourceLocation location) {
    if (value.is_number()) {
        return value.as_number();
    }

    std::ostringstream message;
    message << "function '" << name << "' argument " << (index + 1)
            << " must be a number, got " << value_kind(value);
    runtime_error(context.diagnostics(), location, message.str());
}

std::string expect_string(BuiltinContext &context, const Value &value,
                          const std::string &name, std::size_t index,
                          SourceLocation location) {
    if (value.is_string()) {
        return *value.as_string();
    }

    std::ostringstream message;
    message << "function '" << name << "' argument " << (index + 1)
            << " must be text, got " << value_kind(value);
    runtime_error(context.diagnostics(), location, message.str());
}

const NativeHandleValue &expect_canvas(BuiltinContext &context,
                                       const Value &value,
                                       const std::string &name,
                                       SourceLocation location) {
    if (value.is_native_handle() &&
        value.as_native_handle()->kind() == "canvas") {
        return *value.as_native_handle();
    }

    std::ostringstream message;
    message << "function '" << name << "' argument 1 must be a canvas handle, got "
            << value_kind(value);
    runtime_error(context.diagnostics(), location, message.str());
}

int hex_digit(char value) {
    if (value >= '0' && value <= '9') {
        return value - '0';
    }
    if (value >= 'a' && value <= 'f') {
        return value - 'a' + 10;
    }
    if (value >= 'A' && value <= 'F') {
        return value - 'A' + 10;
    }
    return -1;
}

std::uint8_t hex_byte(char high, char low, DiagnosticReporter &diagnostics,
                      SourceLocation location, const std::string &source) {
    const int first = hex_digit(high);
    const int second = hex_digit(low);
    if (first < 0 || second < 0) {
        runtime_error(diagnostics, location,
                      "invalid canvas color '" + source + "'");
    }

    return static_cast<std::uint8_t>((first << 4) | second);
}

CanvasColor parse_color(DiagnosticReporter &diagnostics,
                        const std::string &source, SourceLocation location) {
    static const std::unordered_map<std::string_view, CanvasColor> colors = {
        {"black", {0, 0, 0, 255}},
        {"white", {255, 255, 255, 255}},
        {"red", {255, 0, 0, 255}},
        {"green", {0, 128, 0, 255}},
        {"blue", {0, 0, 255, 255}},
        {"gray", {128, 128, 128, 255}},
        {"grey", {128, 128, 128, 255}},
        {"lightblue", {173, 216, 230, 255}},
        {"navy", {0, 0, 128, 255}},
        {"tomato", {255, 99, 71, 255}},
        {"yellow", {255, 255, 0, 255}},
        {"orange", {255, 165, 0, 255}},
        {"purple", {128, 0, 128, 255}},
        {"pink", {255, 192, 203, 255}},
        {"transparent", {0, 0, 0, 0}},
    };

    const auto found = colors.find(source);
    if (found != colors.end()) {
        return found->second;
    }

    if (source.size() == 7 && source[0] == '#') {
        return CanvasColor{
            hex_byte(source[1], source[2], diagnostics, location, source),
            hex_byte(source[3], source[4], diagnostics, location, source),
            hex_byte(source[5], source[6], diagnostics, location, source),
            255};
    }

    runtime_error(diagnostics, location, "unknown canvas color '" + source + "'");
}

std::string key_to_string(const TableKey &key) {
    if (std::holds_alternative<std::string>(key.value())) {
        return std::get<std::string>(key.value());
    }

    std::ostringstream out;
    out << std::get<double>(key.value());
    return out.str();
}

CanvasDrawOptions parse_options(BuiltinContext &context, const Value *value,
                                const std::string &name,
                                SourceLocation location) {
    CanvasDrawOptions options;
    options.width = 1.0;
    options.size = 16.0;

    if (value == nullptr || value->is_nil()) {
        return options;
    }

    if (!value->is_table()) {
        runtime_error(context.diagnostics(), location,
                      "function '" + name +
                          "' options argument must be a portefeuil");
    }

    for (const auto &[key, entry] : value->as_table()->entries()) {
        if (!std::holds_alternative<std::string>(key.value())) {
            runtime_error(context.diagnostics(), location,
                          "unknown canvas option '" + key_to_string(key) + "'");
        }

        const std::string option = std::get<std::string>(key.value());
        if (option == "vulling" || option == "rand" || option == "kleur") {
            const std::string color =
                expect_string(context, entry, name, 0, location);
            const CanvasColor parsed =
                parse_color(context.diagnostics(), color, location);
            if (option == "vulling") {
                options.fill = parsed;
            } else if (option == "rand") {
                options.stroke = parsed;
            } else {
                options.color = parsed;
            }
            continue;
        }

        if (option == "breide") {
            options.width = expect_number(context, entry, name, 0, location);
            continue;
        }

        if (option == "greutte") {
            options.size = expect_number(context, entry, name, 0, location);
            continue;
        }

        runtime_error(context.diagnostics(), location,
                      "unknown canvas option '" + option + "'");
    }

    return options;
}

Value call_canvas(CanvasService &canvas_service, BuiltinContext &context,
                  const std::string &name,
                  const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 3, location);

    try {
        return Value(canvas_service.create(
            expect_number(context, args[0], name, 0, location),
            expect_number(context, args[1], name, 1, location),
            expect_string(context, args[2], name, 2, location)));
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_clear(CanvasService &canvas_service, BuiltinContext &context,
                 const std::string &name,
                 const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 2, location);

    try {
        canvas_service.clear(
            expect_canvas(context, args[0], name, location),
            parse_color(context.diagnostics(),
                        expect_string(context, args[1], name, 1, location),
                        location));
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_present(CanvasService &canvas_service, BuiltinContext &context,
                   const std::string &name,
                   const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 1, location);

    try {
        canvas_service.present(
            expect_canvas(context, args[0], name, location));
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_wait(CanvasService &canvas_service, BuiltinContext &context,
                const std::string &name,
                const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 1, location);

    try {
        canvas_service.wait_until_closed(
            expect_canvas(context, args[0], name, location));
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_pause(CanvasService &canvas_service, BuiltinContext &context,
                 const std::string &name,
                 const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 2, location);

    try {
        canvas_service.pause(
            expect_canvas(context, args[0], name, location),
            expect_number(context, args[1], name, 1, location));
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_is_open(CanvasService &canvas_service, BuiltinContext &context,
                   const std::string &name,
                   const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 1, location);

    try {
        return Value(canvas_service.is_open(
            expect_canvas(context, args[0], name, location)));
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_close(CanvasService &canvas_service, BuiltinContext &context,
                 const std::string &name,
                 const std::vector<Value> &args, SourceLocation location) {
    require_arity(context.diagnostics(), name, args, 1, location);

    try {
        canvas_service.close(
            expect_canvas(context, args[0], name, location));
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_line(CanvasService &canvas_service, BuiltinContext &context,
                const std::string &name,
                const std::vector<Value> &args, SourceLocation location) {
    require_arity_range(context.diagnostics(), name, args, 5, 6, location);

    try {
        CanvasDrawOptions options =
            parse_options(context, args.size() == 6 ? &args[5] : nullptr, name,
                          location);
        if (!options.color.has_value()) {
            options.color = black();
        }
        canvas_service.line(
            expect_canvas(context, args[0], name, location),
            expect_number(context, args[1], name, 1, location),
            expect_number(context, args[2], name, 2, location),
            expect_number(context, args[3], name, 3, location),
            expect_number(context, args[4], name, 4, location), options);
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_rect(CanvasService &canvas_service, BuiltinContext &context,
                const std::string &name,
                const std::vector<Value> &args, SourceLocation location) {
    require_arity_range(context.diagnostics(), name, args, 5, 6, location);

    try {
        CanvasDrawOptions options =
            parse_options(context, args.size() == 6 ? &args[5] : nullptr, name,
                          location);
        if (!options.fill.has_value() && !options.stroke.has_value()) {
            options.stroke = black();
        }
        canvas_service.rect(
            expect_canvas(context, args[0], name, location),
            expect_number(context, args[1], name, 1, location),
            expect_number(context, args[2], name, 2, location),
            expect_number(context, args[3], name, 3, location),
            expect_number(context, args[4], name, 4, location), options);
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_circle(CanvasService &canvas_service, BuiltinContext &context,
                  const std::string &name,
                  const std::vector<Value> &args, SourceLocation location) {
    require_arity_range(context.diagnostics(), name, args, 4, 5, location);

    try {
        CanvasDrawOptions options =
            parse_options(context, args.size() == 5 ? &args[4] : nullptr, name,
                          location);
        if (!options.fill.has_value() && !options.stroke.has_value()) {
            options.stroke = black();
        }
        canvas_service.circle(
            expect_canvas(context, args[0], name, location),
            expect_number(context, args[1], name, 1, location),
            expect_number(context, args[2], name, 2, location),
            expect_number(context, args[3], name, 3, location), options);
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

Value call_text(CanvasService &canvas_service, BuiltinContext &context,
                const std::string &name,
                const std::vector<Value> &args, SourceLocation location) {
    require_arity_range(context.diagnostics(), name, args, 4, 5, location);

    try {
        CanvasDrawOptions options =
            parse_options(context, args.size() == 5 ? &args[4] : nullptr, name,
                          location);
        if (!options.color.has_value()) {
            options.color = black();
        }
        canvas_service.text(
            expect_canvas(context, args[0], name, location),
            expect_number(context, args[1], name, 1, location),
            expect_number(context, args[2], name, 2, location),
            expect_string(context, args[3], name, 3, location), options);
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

} // namespace

void register_canvas_builtins(BuiltinRegistry &registry,
                              CanvasService &canvas_service) {
    registry.register_function(
        std::string(kCanvasName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_canvas(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kClearName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_clear(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kPresentName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_present(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kWaitName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_wait(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kPauseName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_pause(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kIsOpenName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_is_open(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kCloseName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_close(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kLineName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_line(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kRectName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_rect(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kCircleName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_circle(canvas_service, context, name, args, location);
        });
    registry.register_function(
        std::string(kTextName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_text(canvas_service, context, name, args, location);
        });
}

} // namespace plat
