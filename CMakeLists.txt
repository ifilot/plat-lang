cmake_minimum_required(VERSION 3.20)

project(platlang LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
enable_testing()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(
  PlatLangParser
  ${CMAKE_CURRENT_SOURCE_DIR}/src/platlang_parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/platlang_parser.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/platlang_parser.h
)

FLEX_TARGET(
  PlatLangLexer
  ${CMAKE_CURRENT_SOURCE_DIR}/src/platlang_lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/platlang_lexer.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/platlang_lexer.h
)

ADD_FLEX_BISON_DEPENDENCY(PlatLangLexer PlatLangParser)

add_library(platlang_core
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ast_printer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/diagnostics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/environment.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/value.cpp
)

target_include_directories(platlang_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(platlang_core PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W3>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall>
)

add_executable(platlang
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${BISON_PlatLangParser_OUTPUTS}
  ${FLEX_PlatLangLexer_OUTPUTS}
)

target_link_libraries(platlang PRIVATE platlang_core)

target_include_directories(platlang PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(platlang PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W3>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall>
)

add_executable(platlang_unit_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/diagnostics_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/test_main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/value_test.cpp
)

target_link_libraries(platlang_unit_tests PRIVATE platlang_core)

target_include_directories(platlang_unit_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit
)

add_test(NAME unit_tests COMMAND platlang_unit_tests)

function(add_platlang_e2e_test name)
  add_test(
    NAME e2e_${name}
    COMMAND
      bash
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/e2e/run_golden_test.sh
      $<TARGET_FILE:platlang>
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/e2e/${name}
  )
endfunction()

add_platlang_e2e_test(hello)
add_platlang_e2e_test(fibonacci)
add_platlang_e2e_test(tables)
add_platlang_e2e_test(loops)
add_platlang_e2e_test(mutual_functions)
add_platlang_e2e_test(input)
add_platlang_e2e_test(input_crlf)
add_platlang_e2e_test(type_builtin)
add_platlang_e2e_test(logical_operators)
add_platlang_e2e_test(source_crlf)
add_platlang_e2e_test(source_cr)
add_platlang_e2e_test(return_diacritic_alias)
add_platlang_e2e_test(builtin_diacritic_alias)
add_platlang_e2e_test(builtin_decomposed_diacritic_alias)
add_platlang_e2e_test(protected_diacritic_alias)
add_platlang_e2e_test(protected_number_type_alias)
add_platlang_e2e_test(protected_composite_type)
add_platlang_e2e_test(wrong_arity)
add_platlang_e2e_test(protected_name)
add_platlang_e2e_test(redeclared_name)
add_platlang_e2e_test(invalid_table_key)
add_platlang_e2e_test(invalid_assignment_target)
add_platlang_e2e_test(return_outside_function)
add_platlang_e2e_test(break_outside_loop)
add_platlang_e2e_test(continue_outside_loop)
add_platlang_e2e_test(division_by_zero)
add_platlang_e2e_test(nested_function)
add_platlang_e2e_test(undeclared_variable_li)
