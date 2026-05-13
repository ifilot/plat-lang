#include "diagnostics.h"

#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace plat {
namespace {

/**
 * Stores one localized diagnostic template.
 */
class DiagnosticTemplate {
private:
    DiagnosticId id_;
    std::string_view message_;

public:
    /**
     * Creates a localized diagnostic template.
     *
     * @param id Stable diagnostic identifier.
     * @param message Localized message template.
     */
    constexpr DiagnosticTemplate(DiagnosticId id, std::string_view message)
        : id_(id), message_(message) {}

    /**
     * Returns the stable diagnostic identifier.
     *
     * @return Diagnostic identifier.
     */
    constexpr DiagnosticId id() const { return id_; }

    /**
     * Returns the localized message template.
     *
     * @return Message template.
     */
    constexpr std::string_view message() const { return message_; }
};

constexpr DiagnosticTemplate kEnglishDiagnostics[] = {
    {DiagnosticId::Usage,
     "usage: platlang [--ast] [--version] [--lang {language}] [program.plat]"},
    {DiagnosticId::InvalidLanguage, "unsupported diagnostic language '{language}'"},
    {DiagnosticId::FileOpenFailed, "could not open file '{path}'"},
    {DiagnosticId::SyntaxError, "syntax error"},
    {DiagnosticId::UnexpectedToken, "unexpected token '{token}'"},
    {DiagnosticId::UndeclaredVariable, "undeclared variable '{name}'"},
    {DiagnosticId::RedeclaredName, "name '{name}' is already defined"},
    {DiagnosticId::ProtectedName, "name '{name}' is protected and cannot be redefined"},
    {DiagnosticId::InvalidTableKey, "table key must be a string or number"},
    {DiagnosticId::UnknownFunction, "unknown function '{name}'"},
    {DiagnosticId::ArityMismatch,
     "function '{name}' expects {expected} argument(s), but got {actual}"},
    {DiagnosticId::ReturnOutsideFunction, "'trok' can only be used inside a function"},
    {DiagnosticId::BreakOutsideLoop, "'aafbraeke' can only be used inside a loop"},
    {DiagnosticId::ContinueOutsideLoop, "'euversjlaon' can only be used inside a loop"},
    {DiagnosticId::FunctionDeclarationInBlock,
     "function declarations are only allowed at top level; move 'funksie' out of this block"},
    {DiagnosticId::FunctionRedefinition, "function '{name}' is already defined"},
    {DiagnosticId::InvalidAssignmentTarget, "invalid assignment target"},
    {DiagnosticId::InvalidOperand, "invalid operand for operator '{operator}'"},
    {DiagnosticId::DivisionByZero, "division by zero"},
    {DiagnosticId::RuntimeError, "{message}"},
    {DiagnosticId::InternalError, "internal error: {message}"},
};

constexpr DiagnosticTemplate kLimburgishDiagnostics[] = {
    {DiagnosticId::Usage,
     "gebroek: platlang [--ast] [--version] [--lang {language}] [program.plat]"},
    {DiagnosticId::InvalidLanguage, "diagnosetaol '{language}' weurt neet ondersteund"},
    {DiagnosticId::FileOpenFailed, "kin besjtand '{path}' neet opmaake"},
    {DiagnosticId::SyntaxError, "syntaxisfout"},
    {DiagnosticId::UnexpectedToken, "onverwach teike '{token}'"},
    {DiagnosticId::UndeclaredVariable, "variabele '{name}' is neet gedeclareerd"},
    {DiagnosticId::RedeclaredName, "naam '{name}' is al gedefinieerd"},
    {DiagnosticId::ProtectedName, "naam '{name}' is besjermd en kin neet opnieuw gedefinieerd waere"},
    {DiagnosticId::InvalidTableKey, "tabel-sjleutel moot eine teks of nómmer zien"},
    {DiagnosticId::UnknownFunction, "onbekende funksie '{name}'"},
    {DiagnosticId::ArityMismatch,
     "funksie '{name}' verwach {expected} argument(en), mer kreeg {actual}"},
    {DiagnosticId::ReturnOutsideFunction, "'trok' kin allein binne ein funksie gebroek waere"},
    {DiagnosticId::BreakOutsideLoop, "'aafbraeke' kin allein binne ein lus gebroek waere"},
    {DiagnosticId::ContinueOutsideLoop, "'euversjlaon' kin allein binne ein lus gebroek waere"},
    {DiagnosticId::FunctionDeclarationInBlock,
     "funksiedeclaraties zien allein toegestaon op topniveau; verplaats 'funksie' oet dit blok"},
    {DiagnosticId::FunctionRedefinition, "funksie '{name}' is al gedefinieerd"},
    {DiagnosticId::InvalidAssignmentTarget, "ongeljig doel veur toewiezing"},
    {DiagnosticId::InvalidOperand, "ongeljige operand veur operator '{operator}'"},
    {DiagnosticId::DivisionByZero, "deile door nul"},
    {DiagnosticId::RuntimeError, "{message}"},
    {DiagnosticId::InternalError, "interne fout: {message}"},
};

/**
 * Returns the template table for a language.
 *
 * @param language Diagnostic language.
 * @return Template table.
 */
std::span<const DiagnosticTemplate> templates_for(Language language) {
    switch (language) {
    case Language::English:
        return kEnglishDiagnostics;
    case Language::Limburgish:
        return kLimburgishDiagnostics;
    }

    return kEnglishDiagnostics;
}

/**
 * Finds the localized template for a diagnostic.
 *
 * @param language Diagnostic language.
 * @param id Stable diagnostic identifier.
 * @return Localized diagnostic template.
 */
std::string_view find_template(Language language, DiagnosticId id) {
    for (const DiagnosticTemplate &entry : templates_for(language)) {
        if (entry.id() == id) {
            return entry.message();
        }
    }

    for (const DiagnosticTemplate &entry : templates_for(Language::English)) {
        if (entry.id() == id) {
            return entry.message();
        }
    }

    return "unknown diagnostic";
}

/**
 * Converts a severity to a localized label.
 *
 * @param language Diagnostic language.
 * @param severity Diagnostic severity.
 * @return Localized severity label.
 */
std::string_view severity_label(Language language, DiagnosticSeverity severity) {
    switch (language) {
    case Language::English:
        switch (severity) {
        case DiagnosticSeverity::Error:
            return "error";
        case DiagnosticSeverity::Warning:
            return "warning";
        case DiagnosticSeverity::Note:
            return "note";
        }
        break;
    case Language::Limburgish:
        switch (severity) {
        case DiagnosticSeverity::Error:
            return "fout";
        case DiagnosticSeverity::Warning:
            return "waorsjuwing";
        case DiagnosticSeverity::Note:
            return "opmerking";
        }
        break;
    }

    return "error";
}

/**
 * Replaces all occurrences of a placeholder in a string.
 *
 * @param text Text to modify.
 * @param placeholder Placeholder including braces.
 * @param value Replacement value.
 */
void replace_all(std::string &text, std::string_view placeholder,
                 std::string_view value) {
    std::size_t pos = 0;
    while ((pos = text.find(placeholder, pos)) != std::string::npos) {
        text.replace(pos, placeholder.size(), value);
        pos += value.size();
    }
}

/**
 * Renders a localized diagnostic message.
 *
 * @param language Diagnostic language.
 * @param id Stable diagnostic identifier.
 * @param args Formatting arguments.
 * @return Rendered message.
 */
std::string render_message(Language language, DiagnosticId id,
                           const std::vector<DiagnosticArg> &args) {
    std::string rendered(find_template(language, id));

    for (const DiagnosticArg &arg : args) {
        const std::string placeholder = "{" + arg.name() + "}";
        replace_all(rendered, placeholder, arg.value());
    }

    return rendered;
}

} // namespace

DiagnosticArg::DiagnosticArg(std::string name, std::string value)
    : name_(std::move(name)), value_(std::move(value)) {}

const std::string &DiagnosticArg::name() const {
    return name_;
}

const std::string &DiagnosticArg::value() const {
    return value_;
}

Diagnostic::Diagnostic(DiagnosticSeverity severity, DiagnosticId id,
                       std::optional<SourceLocation> location,
                       std::string message)
    : severity_(severity),
      id_(id),
      location_(location),
      message_(std::move(message)) {}

DiagnosticSeverity Diagnostic::severity() const {
    return severity_;
}

DiagnosticId Diagnostic::id() const {
    return id_;
}

const std::optional<SourceLocation> &Diagnostic::location() const {
    return location_;
}

const std::string &Diagnostic::message() const {
    return message_;
}

PlatlangError::PlatlangError(DiagnosticId id, std::string message)
    : id_(id), message_(std::move(message)) {}

const char *PlatlangError::what() const noexcept {
    return message_.c_str();
}

DiagnosticId PlatlangError::id() const {
    return id_;
}

DiagnosticReporter::DiagnosticReporter(Language language, std::ostream &stream)
    : language_(language), stream_(&stream) {}

Language DiagnosticReporter::language() const {
    return language_;
}

void DiagnosticReporter::report(DiagnosticSeverity severity, DiagnosticId id,
                                std::optional<SourceLocation> location,
                                std::vector<DiagnosticArg> args) {
    const std::string message = render_message(language_, id, args);

    if (severity == DiagnosticSeverity::Error) {
        has_error_ = true;
    }

    diagnostics_.emplace_back(severity, id, location, message);

    if (location.has_value()) {
        *stream_ << "Line " << location->line() << ", Column "
                 << location->column() << ": ";
    }

    *stream_ << severity_label(language_, severity) << ": " << message << '\n';
}

void DiagnosticReporter::error(DiagnosticId id,
                               std::optional<SourceLocation> location,
                               std::vector<DiagnosticArg> args) {
    report(DiagnosticSeverity::Error, id, location, std::move(args));
}

[[noreturn]] void DiagnosticReporter::fatal(
    DiagnosticId id, std::optional<SourceLocation> location,
    std::vector<DiagnosticArg> args) {
    const std::string message = render_message(language_, id, args);
    error(id, location, std::move(args));
    throw PlatlangError(id, message);
}

bool DiagnosticReporter::has_error() const {
    return has_error_;
}

const std::vector<Diagnostic> &DiagnosticReporter::diagnostics() const {
    return diagnostics_;
}

std::optional<Language> parse_language_tag(std::string_view tag) {
    if (tag == "en") {
        return Language::English;
    }

    if (tag == "li" || tag == "li-sit" || tag == "li-x-sittard") {
        return Language::Limburgish;
    }

    return std::nullopt;
}

std::string_view language_tag(Language language) {
    switch (language) {
    case Language::English:
        return "en";
    case Language::Limburgish:
        return "li";
    }

    return "en";
}

} // namespace plat
