cmake_minimum_required(VERSION 3.20)

project(platlang LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

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

add_executable(platlang
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ast_printer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/diagnostics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${BISON_PlatLangParser_OUTPUTS}
  ${FLEX_PlatLangLexer_OUTPUTS}
)

target_include_directories(platlang PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(platlang PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W3>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall>
)
