# plat-lang C++ Code Style

The `plat-lang` implementation follows the Google C++ Style Guide as its base,
with the project-specific rules listed below taking precedence when they differ.

## Files

Use `.h` for C++ headers and `.cpp` for C++ source files.

File names use `lower_case_with_underscores`.

Examples:

```text
ast_node.h
ast_node.cpp
source_location.h
source_location.cpp
```

Bison and Flex source files are allowed to use their standard extensions:
`.y` for grammar files and `.l` for lexer files. Embedded C++ code inside those
files should still follow the project style where practical.

## Indentation

Use 4 spaces for indentation.

Do not use tab characters.

## Naming

Classes use `PascalCase`.

```cpp
class SourceLocation {
    ...
};
```

Functions use `lower_case_with_underscores`.

```cpp
void report_error();
```

Variables should follow the Google C++ Style Guide unless a more specific
project rule is introduced later.

## Class Layout

Classes are ordered as follows:

1. Private member variables
2. Public member functions
3. Private member functions

Example:

```cpp
class Parser {
private:
    std::string source_name_;

public:
    /** Parses the current source file and returns the resulting AST. */
    Program parse();

private:
    /** Parses a single statement from the token stream. */
    Stmt parse_statement();
};
```

## Documentation

All public classes and functions must have clear Doxygen-style comments.

Class comments should explain the role of the class in the implementation.
Function comments should explain what the function does, its important inputs,
and any meaningful error behavior.

Put the Doxygen comment on the declaration that introduces the public API. For a
function declared in a `.h` file and defined in a `.cpp` file, document the
declaration in the header and do not duplicate that same Doxygen block above the
definition. If the implementation needs extra explanation, use a normal comment
near the relevant code instead.

Private helper functions may use Doxygen comments when they clarify intent, but
short implementation details do not need formal documentation blocks.

Example:

```cpp
/**
 * Represents a source position used for parser and runtime diagnostics.
 */
class SourceLocation {
private:
    int line_;
    int column_;

public:
    /**
     * Creates a source location.
     *
     * @param line One-based line number.
     * @param column One-based column number.
     */
    SourceLocation(int line, int column);
};
```

Prefer useful comments over mechanical repetition. A Doxygen comment should make
the surrounding code easier to understand, not simply restate the declaration.
