#include "builtins/canvas_builtins.h"

#include "builtins/builtin_registry.h"
#include "canvas/canvas_service.h"

#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
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
constexpr std::string_view kPathName = "canvas_pad";
constexpr std::string_view kTextName = "canvas_teks";

/**
 * Returns the default opaque black canvas color.
 */
CanvasColor black() {
    return CanvasColor{0, 0, 0, 255};
}

/**
 * Returns the user-facing runtime kind for a value used in diagnostics.
 */
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
        return "tabel";
    }
    if (value.is_native_handle()) {
        return value.as_native_handle()->type_name();
    }
    return "waarde";
}

/**
 * Reports a canvas built-in runtime error.
 *
 * @throws DiagnosticFatal through the diagnostic reporter.
 */
[[noreturn]] void runtime_error(DiagnosticReporter &diagnostics,
                                SourceLocation location,
                                const std::string &message) {
    diagnostics.fatal(DiagnosticId::RuntimeError, location,
                      {DiagnosticArg("message", message)});
}

/**
 * Validates that a built-in argument count falls within an inclusive range.
 */
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

/**
 * Extracts a numeric argument or reports a typed runtime error.
 */
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

/**
 * Extracts a string argument or reports a typed runtime error.
 */
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

/**
 * Extracts a canvas handle argument or reports a typed runtime error.
 */
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

/**
 * Converts one hexadecimal color digit to its numeric value.
 */
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

/**
 * Parses two hexadecimal color digits into one byte.
 *
 * @throws DiagnosticFatal when either digit is invalid.
 */
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

/**
 * Parses a named color or #RRGGBB literal into a canvas color.
 *
 * @throws DiagnosticFatal when the color string is unknown or malformed.
 */
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

/**
 * Converts a table key into text for diagnostics.
 */
std::string key_to_string(const TableKey &key) {
    if (std::holds_alternative<std::string>(key.value())) {
        return std::get<std::string>(key.value());
    }

    std::ostringstream out;
    out << std::get<double>(key.value());
    return out.str();
}

/**
 * Parses the optional canvas drawing options table.
 *
 * @throws DiagnosticFatal when the options value or an option entry is invalid.
 */
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
                          "' options argument must be a tabel");
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

        if (option == "stappe") {
            options.curve_steps =
                expect_number(context, entry, name, 0, location);
            continue;
        }

        runtime_error(context.diagnostics(), location,
                      "unknown canvas option '" + option + "'");
    }

    return options;
}

class SvgPathParser {
private:
    BuiltinContext &context_;
    const std::string &name_;
    const std::string &source_;
    SourceLocation location_;
    std::size_t offset_ = 0;
    char command_ = '\0';
    CanvasPoint current_;
    CanvasPoint subpath_start_;
    CanvasPoint last_cubic_control_;
    CanvasPoint last_quadratic_control_;
    bool has_last_cubic_control_ = false;
    bool has_last_quadratic_control_ = false;
    std::vector<CanvasPath> paths_;
    CanvasPath current_path_;
    int curve_steps_;

public:
    SvgPathParser(BuiltinContext &context, const std::string &name,
                  const std::string &source, SourceLocation location,
                  double curve_steps)
        : context_(context),
          name_(name),
          source_(source),
          location_(location),
          curve_steps_(std::max(1, static_cast<int>(std::lround(curve_steps)))) {}

    std::vector<CanvasPath> parse() {
        skip_separators();

        while (!at_end()) {
            if (is_command(peek())) {
                command_ = source_[offset_++];
            } else if (command_ == '\0') {
                fail("path must start with a command");
            }

            switch (command_) {
            case 'M':
            case 'm':
                parse_move(command_ == 'm');
                break;
            case 'L':
            case 'l':
                parse_line(command_ == 'l');
                break;
            case 'H':
            case 'h':
                parse_horizontal(command_ == 'h');
                break;
            case 'V':
            case 'v':
                parse_vertical(command_ == 'v');
                break;
            case 'C':
            case 'c':
                parse_cubic(command_ == 'c');
                break;
            case 'S':
            case 's':
                parse_smooth_cubic(command_ == 's');
                break;
            case 'Q':
            case 'q':
                parse_quadratic(command_ == 'q');
                break;
            case 'T':
            case 't':
                parse_smooth_quadratic(command_ == 't');
                break;
            case 'Z':
            case 'z':
                close_path();
                command_ = '\0';
                break;
            default:
                fail(std::string("unsupported path command '") + command_ + "'");
            }

            skip_separators();
        }

        finish_path();
        return paths_;
    }

private:
    bool at_end() const {
        return offset_ >= source_.size();
    }

    char peek() const {
        return at_end() ? '\0' : source_[offset_];
    }

    static bool is_command(char value) {
        switch (value) {
        case 'M':
        case 'm':
        case 'L':
        case 'l':
        case 'H':
        case 'h':
        case 'V':
        case 'v':
        case 'C':
        case 'c':
        case 'S':
        case 's':
        case 'Q':
        case 'q':
        case 'T':
        case 't':
        case 'Z':
        case 'z':
            return true;
        default:
            return false;
        }
    }

    static bool begins_number(char value) {
        return std::isdigit(static_cast<unsigned char>(value)) || value == '-'
               || value == '+' || value == '.';
    }

    void fail(const std::string &message) const {
        runtime_error(context_.diagnostics(), location_,
                      "invalid canvas path for function '" + name_ + "': "
                          + message);
    }

    void skip_separators() {
        while (!at_end()) {
            char value = peek();
            if (std::isspace(static_cast<unsigned char>(value)) || value == ',') {
                ++offset_;
            } else {
                break;
            }
        }
    }

    bool has_number() {
        skip_separators();
        return begins_number(peek());
    }

    double number() {
        skip_separators();
        if (!begins_number(peek())) {
            fail("expected a number");
        }

        const char *start = source_.c_str() + offset_;
        char *end = nullptr;
        const double value = std::strtod(start, &end);
        if (end == start) {
            fail("expected a number");
        }

        offset_ = static_cast<std::size_t>(end - source_.c_str());
        return value;
    }

    CanvasPoint point(bool relative) {
        CanvasPoint result{number(), number()};
        if (relative) {
            result.x += current_.x;
            result.y += current_.y;
        }
        return result;
    }

    void start_path(CanvasPoint point) {
        finish_path();
        current_path_ = CanvasPath();
        current_ = point;
        subpath_start_ = point;
        current_path_.points.push_back(point);
        reset_controls();
    }

    void finish_path() {
        if (!current_path_.points.empty()) {
            paths_.push_back(current_path_);
            current_path_ = CanvasPath();
        }
    }

    void add_point(CanvasPoint point) {
        if (current_path_.points.empty()) {
            current_path_.points.push_back(current_);
        }
        current_path_.points.push_back(point);
        current_ = point;
    }

    void close_path() {
        if (!current_path_.points.empty()) {
            if (current_.x != subpath_start_.x || current_.y != subpath_start_.y) {
                current_path_.points.push_back(subpath_start_);
            }
            current_path_.closed = true;
            current_ = subpath_start_;
        }
        reset_controls();
        finish_path();
    }

    void reset_controls() {
        has_last_cubic_control_ = false;
        has_last_quadratic_control_ = false;
    }

    CanvasPoint reflect(CanvasPoint point) const {
        return CanvasPoint{current_.x * 2.0 - point.x,
                           current_.y * 2.0 - point.y};
    }

    void cubic_to(CanvasPoint c1, CanvasPoint c2, CanvasPoint end) {
        const CanvasPoint start = current_;
        for (int step = 1; step <= curve_steps_; ++step) {
            const double t = static_cast<double>(step) / curve_steps_;
            const double u = 1.0 - t;
            add_point(CanvasPoint{
                u * u * u * start.x + 3.0 * u * u * t * c1.x
                    + 3.0 * u * t * t * c2.x + t * t * t * end.x,
                u * u * u * start.y + 3.0 * u * u * t * c1.y
                    + 3.0 * u * t * t * c2.y + t * t * t * end.y});
        }
        last_cubic_control_ = c2;
        has_last_cubic_control_ = true;
        has_last_quadratic_control_ = false;
    }

    void quadratic_to(CanvasPoint control, CanvasPoint end) {
        const CanvasPoint start = current_;
        for (int step = 1; step <= curve_steps_; ++step) {
            const double t = static_cast<double>(step) / curve_steps_;
            const double u = 1.0 - t;
            add_point(CanvasPoint{
                u * u * start.x + 2.0 * u * t * control.x + t * t * end.x,
                u * u * start.y + 2.0 * u * t * control.y + t * t * end.y});
        }
        last_quadratic_control_ = control;
        has_last_quadratic_control_ = true;
        has_last_cubic_control_ = false;
    }

    void parse_move(bool relative) {
        start_path(point(relative));
        while (has_number()) {
            add_point(point(relative));
        }
        command_ = relative ? 'l' : 'L';
    }

    void parse_line(bool relative) {
        do {
            add_point(point(relative));
        } while (has_number());
        reset_controls();
    }

    void parse_horizontal(bool relative) {
        do {
            double x = number();
            if (relative) {
                x += current_.x;
            }
            add_point(CanvasPoint{x, current_.y});
        } while (has_number());
        reset_controls();
    }

    void parse_vertical(bool relative) {
        do {
            double y = number();
            if (relative) {
                y += current_.y;
            }
            add_point(CanvasPoint{current_.x, y});
        } while (has_number());
        reset_controls();
    }

    void parse_cubic(bool relative) {
        do {
            CanvasPoint c1 = point(relative);
            CanvasPoint c2 = point(relative);
            CanvasPoint end = point(relative);
            cubic_to(c1, c2, end);
        } while (has_number());
    }

    void parse_smooth_cubic(bool relative) {
        do {
            CanvasPoint c1 = has_last_cubic_control_
                ? reflect(last_cubic_control_)
                : current_;
            CanvasPoint c2 = point(relative);
            CanvasPoint end = point(relative);
            cubic_to(c1, c2, end);
        } while (has_number());
    }

    void parse_quadratic(bool relative) {
        do {
            CanvasPoint control = point(relative);
            CanvasPoint end = point(relative);
            quadratic_to(control, end);
        } while (has_number());
    }

    void parse_smooth_quadratic(bool relative) {
        do {
            CanvasPoint control = has_last_quadratic_control_
                ? reflect(last_quadratic_control_)
                : current_;
            CanvasPoint end = point(relative);
            quadratic_to(control, end);
        } while (has_number());
    }
};

/**
 * Implements the canvas creation built-in.
 */
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

/**
 * Implements the canvas clear built-in.
 */
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

/**
 * Implements the canvas present built-in.
 */
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

/**
 * Implements the canvas wait-until-closed built-in.
 */
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

/**
 * Implements the canvas pause built-in.
 */
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

/**
 * Implements the canvas is-open built-in.
 */
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

/**
 * Implements the canvas close built-in.
 */
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

/**
 * Implements the canvas line drawing built-in.
 */
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

/**
 * Implements the canvas rectangle drawing built-in.
 */
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

/**
 * Implements the canvas circle drawing built-in.
 */
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

/**
 * Implements the canvas path drawing built-in.
 */
Value call_path(CanvasService &canvas_service, BuiltinContext &context,
                const std::string &name,
                const std::vector<Value> &args, SourceLocation location) {
    require_arity_range(context.diagnostics(), name, args, 2, 3, location);

    try {
        CanvasDrawOptions options =
            parse_options(context, args.size() == 3 ? &args[2] : nullptr, name,
                          location);
        if (!options.fill.has_value() && !options.stroke.has_value()) {
            options.stroke = black();
        }

        const std::string path_data =
            expect_string(context, args[1], name, 1, location);
        std::vector<CanvasPath> paths =
            SvgPathParser(context, name, path_data, location,
                          options.curve_steps)
                .parse();

        canvas_service.path(expect_canvas(context, args[0], name, location),
                            paths, options);
        return Value();
    } catch (const CanvasError &error) {
        runtime_error(context.diagnostics(), location, error.what());
    }
}

/**
 * Implements the canvas text drawing built-in.
 */
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
        std::string(kPathName),
        [&canvas_service](BuiltinContext &context, const std::string &name,
                          const std::vector<Value> &args,
                          SourceLocation location) {
            return call_path(canvas_service, context, name, args, location);
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
