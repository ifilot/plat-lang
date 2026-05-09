#pragma once

#include "ast.h"

#include <exception>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace plat {

/**
 * Identifies a supported user-facing diagnostic language.
 */
enum class Language {
    English,
    Limburgish,
};

/**
 * Identifies the severity of a diagnostic message.
 */
enum class DiagnosticSeverity {
    Error,
    Warning,
    Note,
};

/**
 * Stable identifier for a user-facing diagnostic.
 */
enum class DiagnosticId {
    Usage,
    InvalidLanguage,
    FileOpenFailed,
    SyntaxError,
    UnexpectedToken,
    UndeclaredVariable,
    RedeclaredName,
    ProtectedName,
    InvalidTableKey,
    UnknownFunction,
    ArityMismatch,
    ReturnOutsideFunction,
    BreakOutsideLoop,
    ContinueOutsideLoop,
    FunctionDeclarationInBlock,
    FunctionRedefinition,
    InvalidAssignmentTarget,
    InvalidOperand,
    DivisionByZero,
    InternalError,
};

/**
 * Provides one named formatting argument for a diagnostic template.
 */
class DiagnosticArg {
private:
    std::string name_;
    std::string value_;

public:
    /**
     * Creates a diagnostic formatting argument.
     *
     * @param name Placeholder name without braces.
     * @param value Replacement value.
     */
    DiagnosticArg(std::string name, std::string value);

    /**
     * Returns the placeholder name.
     *
     * @return Placeholder name.
     */
    const std::string &name() const;

    /**
     * Returns the replacement value.
     *
     * @return Replacement value.
     */
    const std::string &value() const;
};

/**
 * Stores one rendered diagnostic.
 */
class Diagnostic {
private:
    DiagnosticSeverity severity_;
    DiagnosticId id_;
    std::optional<SourceLocation> location_;
    std::string message_;

public:
    /**
     * Creates a rendered diagnostic.
     *
     * @param severity Diagnostic severity.
     * @param id Stable diagnostic identifier.
     * @param location Optional source location.
     * @param message Rendered user-facing message.
     */
    Diagnostic(DiagnosticSeverity severity, DiagnosticId id,
               std::optional<SourceLocation> location, std::string message);

    /**
     * Returns the diagnostic severity.
     *
     * @return Diagnostic severity.
     */
    DiagnosticSeverity severity() const;

    /**
     * Returns the stable diagnostic identifier.
     *
     * @return Diagnostic identifier.
     */
    DiagnosticId id() const;

    /**
     * Returns the optional source location.
     *
     * @return Optional source location.
     */
    const std::optional<SourceLocation> &location() const;

    /**
     * Returns the rendered user-facing message.
     *
     * @return Rendered message.
     */
    const std::string &message() const;
};

/**
 * Exception used internally to stop execution after a fatal diagnostic.
 */
class PlatlangError final : public std::exception {
private:
    DiagnosticId id_;
    std::string message_;

public:
    /**
     * Creates an internal fatal error wrapper.
     *
     * @param id Stable diagnostic identifier.
     * @param message Rendered diagnostic message.
     */
    PlatlangError(DiagnosticId id, std::string message);

    /**
     * Returns the rendered diagnostic message.
     *
     * @return Null-terminated diagnostic message.
     */
    const char *what() const noexcept override;

    /**
     * Returns the stable diagnostic identifier.
     *
     * @return Diagnostic identifier.
     */
    DiagnosticId id() const;
};

/**
 * Renders diagnostics in the selected language and records emitted messages.
 */
class DiagnosticReporter {
private:
    Language language_;
    std::ostream *stream_;
    std::vector<Diagnostic> diagnostics_;
    bool has_error_ = false;

public:
    /**
     * Creates a diagnostic reporter.
     *
     * @param language User-facing diagnostic language.
     * @param stream Output stream for rendered diagnostics.
     */
    DiagnosticReporter(Language language, std::ostream &stream);

    /**
     * Returns the active diagnostic language.
     *
     * @return Active language.
     */
    Language language() const;

    /**
     * Emits a diagnostic.
     *
     * @param severity Diagnostic severity.
     * @param id Stable diagnostic identifier.
     * @param location Optional source location.
     * @param args Formatting arguments.
     */
    void report(DiagnosticSeverity severity, DiagnosticId id,
                std::optional<SourceLocation> location = std::nullopt,
                std::vector<DiagnosticArg> args = {});

    /**
     * Emits an error diagnostic.
     *
     * @param id Stable diagnostic identifier.
     * @param location Optional source location.
     * @param args Formatting arguments.
     */
    void error(DiagnosticId id,
               std::optional<SourceLocation> location = std::nullopt,
               std::vector<DiagnosticArg> args = {});

    /**
     * Emits an error diagnostic and throws a PlatlangError.
     *
     * @param id Stable diagnostic identifier.
     * @param location Optional source location.
     * @param args Formatting arguments.
     */
    [[noreturn]] void fatal(DiagnosticId id,
                            std::optional<SourceLocation> location = std::nullopt,
                            std::vector<DiagnosticArg> args = {});

    /**
     * Returns whether any error diagnostic has been emitted.
     *
     * @return True if an error was emitted.
     */
    bool has_error() const;

    /**
     * Returns all emitted diagnostics.
     *
     * @return Emitted diagnostics.
     */
    const std::vector<Diagnostic> &diagnostics() const;
};

/**
 * Parses a command-line language tag.
 *
 * @param tag User-provided language tag.
 * @return Parsed language when the tag is supported.
 */
std::optional<Language> parse_language_tag(std::string_view tag);

/**
 * Returns the canonical command-line tag for a language.
 *
 * @param language Diagnostic language.
 * @return Canonical language tag.
 */
std::string_view language_tag(Language language);

} // namespace plat
