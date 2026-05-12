#include "ast.h"
#include "ast_printer.h"
#include "diagnostics.h"
#include "interpreter.h"
#include "version.h"

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

extern FILE *yyin;
extern std::unique_ptr<plat::Program> platlang_program;
extern plat::DiagnosticReporter *platlang_diagnostics;

/**
 * Parses the current lexer input and populates the global parsed program.
 *
 * @return Zero on parse success, non-zero on parse failure.
 */
int yyparse();

/**
 * Stores command-line options for the executable.
 */
class CommandLineOptions {
private:
    plat::Language language_ = plat::Language::English;
    const char *input_path_ = nullptr;
    bool print_ast_ = false;
    bool print_version_ = false;

public:
    /**
     * Sets the diagnostic language.
     *
     * @param language Selected diagnostic language.
     */
    void set_language(plat::Language language) { language_ = language; }

    /**
     * Sets the optional input path.
     *
     * @param input_path Source file path, or null for stdin.
     */
    void set_input_path(const char *input_path) { input_path_ = input_path; }

    /**
     * Enables or disables AST printing.
     *
     * @param print_ast Whether to print the parsed AST.
     */
    void set_print_ast(bool print_ast) { print_ast_ = print_ast; }

    /**
     * Enables or disables version printing.
     *
     * @param print_version Whether to print the version.
     */
    void set_print_version(bool print_version) { print_version_ = print_version; }

    /**
     * Returns the selected diagnostic language.
     *
     * @return Diagnostic language.
     */
    plat::Language language() const { return language_; }

    /**
     * Returns the optional input path.
     *
     * @return Source file path, or null for stdin.
     */
    const char *input_path() const { return input_path_; }

    /**
     * Returns whether the parsed AST should be printed.
     *
     * @return True when AST printing is enabled.
     */
    bool print_ast() const { return print_ast_; }

    /**
     * Returns whether the executable should print its version.
     *
     * @return True when version printing is enabled.
     */
    bool print_version() const { return print_version_; }
};

/**
 * Parses command-line arguments.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument values.
 * @param reporter Diagnostic reporter used for argument errors.
 * @return Parsed command-line options when arguments are valid.
 */
std::optional<CommandLineOptions> parse_command_line(
    int argc, char **argv, plat::DiagnosticReporter &reporter) {
    CommandLineOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];

        if (arg == "--lang") {
            if (i + 1 >= argc) {
                reporter.error(plat::DiagnosticId::Usage);
                return std::nullopt;
            }

            const std::string_view tag = argv[++i];
            const std::optional<plat::Language> language =
                plat::parse_language_tag(tag);
            if (!language.has_value()) {
                reporter.error(
                    plat::DiagnosticId::InvalidLanguage, std::nullopt,
                    {plat::DiagnosticArg("language", std::string(tag))});
                return std::nullopt;
            }

            options.set_language(*language);
            continue;
        }

        if (arg.starts_with("--lang=")) {
            const std::string_view tag = arg.substr(7);
            const std::optional<plat::Language> language =
                plat::parse_language_tag(tag);
            if (!language.has_value()) {
                reporter.error(
                    plat::DiagnosticId::InvalidLanguage, std::nullopt,
                    {plat::DiagnosticArg("language", std::string(tag))});
                return std::nullopt;
            }

            options.set_language(*language);
            continue;
        }

        if (arg == "--ast") {
            options.set_print_ast(true);
            continue;
        }

        if (arg == "--version") {
            options.set_print_version(true);
            continue;
        }

        if (arg.starts_with("--")) {
            reporter.error(plat::DiagnosticId::Usage);
            return std::nullopt;
        }

        if (options.input_path() != nullptr) {
            reporter.error(plat::DiagnosticId::Usage);
            return std::nullopt;
        }

        options.set_input_path(argv[i]);
    }

    return options;
}

std::vector<std::filesystem::path> capability_search_paths(
    const char *argv0, const char *input_path) {
    std::vector<std::filesystem::path> paths;

    if (input_path != nullptr) {
        paths.push_back(std::filesystem::absolute(input_path).parent_path() /
                        "capabilities");
    }

    paths.push_back(std::filesystem::current_path() / "capabilities");

    const std::filesystem::path executable_path(argv0);
    if (executable_path.has_parent_path()) {
        paths.push_back(std::filesystem::absolute(executable_path).parent_path() /
                        "capabilities");
    }

    return paths;
}

/**
 * Parses a plat-lang source file and reports the number of top-level AST nodes.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument values.
 * @return Zero on successful parse, non-zero on usage, file, or parser errors.
 */
int main(int argc, char **argv) {
    plat::DiagnosticReporter bootstrap_reporter(plat::Language::English,
                                                std::cerr);
    const std::optional<CommandLineOptions> options =
        parse_command_line(argc, argv, bootstrap_reporter);
    if (!options.has_value()) {
        return 2;
    }

    if (options->print_version()) {
        std::cout << "plat-lang " << plat::kVersion << '\n';
        return 0;
    }

    plat::DiagnosticReporter reporter(options->language(), std::cerr);
    platlang_diagnostics = &reporter;

    FILE *input = stdin;
    if (options->input_path() != nullptr) {
        input = std::fopen(options->input_path(), "r");
        if (input == nullptr) {
            reporter.error(
                plat::DiagnosticId::FileOpenFailed, std::nullopt,
                {plat::DiagnosticArg("path", options->input_path())});
            return 2;
        }
    }

    yyin = input;
    const int result = yyparse();

    if (input != stdin) {
        std::fclose(input);
    }

    if (result != 0) {
        return result;
    }

    if (options->print_ast()) {
        if (platlang_program != nullptr) {
            plat::AstPrinter printer(std::cout);
            printer.print_program(*platlang_program);
        }
    } else {
        try {
            if (platlang_program != nullptr) {
                plat::Interpreter interpreter(
                    reporter, std::cin, std::cout,
                    capability_search_paths(argv[0], options->input_path()));
                interpreter.execute_program(*platlang_program);
            }
        } catch (const plat::PlatlangError &) {
            return 1;
        } catch (const std::exception &error) {
            reporter.error(
                plat::DiagnosticId::InternalError, std::nullopt,
                {plat::DiagnosticArg("message", error.what())});
            return 1;
        }
    }

    return 0;
}
