cmake_minimum_required(VERSION 3.20)

project(platlang LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
enable_testing()

option(PLATLANG_ENABLE_CANVAS "Build canvas host-capability support" ON)
option(PLATLANG_ENABLE_SDL_CANVAS "Build SDL2 canvas backend" OFF)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(PkgConfig)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/builtin_registry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/canvas_builtins.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/builtins/core_builtins.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/capabilities/capability_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/canvas/canvas_service.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/canvas/recording_canvas_backend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/diagnostics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/environment.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/value.cpp
)

target_compile_definitions(platlang_core PUBLIC
  PLATLANG_ENABLE_CANVAS=$<BOOL:${PLATLANG_ENABLE_CANVAS}>
  PLATLANG_ENABLE_SDL_CANVAS=$<BOOL:${PLATLANG_ENABLE_SDL_CANVAS}>
)

if(PLATLANG_ENABLE_SDL_CANVAS)
  if(NOT PkgConfig_FOUND)
    message(FATAL_ERROR "PkgConfig is required for PLATLANG_ENABLE_SDL_CANVAS")
  endif()
  pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

  if(MINGW)
    get_target_property(PLATLANG_SDL2_COMPILE_OPTIONS PkgConfig::SDL2 INTERFACE_COMPILE_OPTIONS)
    if(PLATLANG_SDL2_COMPILE_OPTIONS)
      list(REMOVE_ITEM PLATLANG_SDL2_COMPILE_OPTIONS -Dmain=SDL_main)
      set_property(TARGET PkgConfig::SDL2 PROPERTY
        INTERFACE_COMPILE_OPTIONS ${PLATLANG_SDL2_COMPILE_OPTIONS})
    endif()

    get_target_property(PLATLANG_SDL2_LINK_OPTIONS PkgConfig::SDL2 INTERFACE_LINK_OPTIONS)
    if(PLATLANG_SDL2_LINK_OPTIONS)
      set(PLATLANG_SDL2_FILTERED_LINK_OPTIONS)
      foreach(PLATLANG_SDL2_LINK_OPTION IN LISTS PLATLANG_SDL2_LINK_OPTIONS)
        if(NOT PLATLANG_SDL2_LINK_OPTION MATCHES "^-mwindows$"
           AND NOT PLATLANG_SDL2_LINK_OPTION MATCHES "(^|[/\\\\])lib(SDL2main|mingw32)\\.a$"
           AND NOT PLATLANG_SDL2_LINK_OPTION MATCHES "^-l(SDL2main|mingw32)$")
          list(APPEND PLATLANG_SDL2_FILTERED_LINK_OPTIONS "${PLATLANG_SDL2_LINK_OPTION}")
        endif()
      endforeach()
      set_property(TARGET PkgConfig::SDL2 PROPERTY
        INTERFACE_LINK_OPTIONS ${PLATLANG_SDL2_FILTERED_LINK_OPTIONS})
    endif()

    get_target_property(PLATLANG_SDL2_LINK_LIBRARIES PkgConfig::SDL2 INTERFACE_LINK_LIBRARIES)
    if(PLATLANG_SDL2_LINK_LIBRARIES)
      set(PLATLANG_SDL2_FILTERED_LINK_LIBRARIES)
      foreach(PLATLANG_SDL2_LIBRARY IN LISTS PLATLANG_SDL2_LINK_LIBRARIES)
        if(NOT PLATLANG_SDL2_LIBRARY MATCHES "(^|[/\\\\])lib(SDL2main|mingw32)\\.a$"
           AND NOT PLATLANG_SDL2_LIBRARY MATCHES "^(SDL2main|mingw32)$"
           AND NOT PLATLANG_SDL2_LIBRARY MATCHES "^-l(SDL2main|mingw32)$"
           AND NOT PLATLANG_SDL2_LIBRARY MATCHES "^-mwindows$")
          list(APPEND PLATLANG_SDL2_FILTERED_LINK_LIBRARIES "${PLATLANG_SDL2_LIBRARY}")
        endif()
      endforeach()
      set_property(TARGET PkgConfig::SDL2 PROPERTY
        INTERFACE_LINK_LIBRARIES ${PLATLANG_SDL2_FILTERED_LINK_LIBRARIES})
    endif()
  endif()

  target_sources(platlang_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canvas/sdl_canvas_backend.cpp
  )
  target_compile_definitions(platlang_core PRIVATE SDL_MAIN_HANDLED)
  target_link_libraries(platlang_core PUBLIC PkgConfig::SDL2)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/capabilities
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/builtin_registry_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/canvas_test.cpp
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
add_platlang_e2e_test(canvas_unavailable)
add_platlang_e2e_test(import_canvas_basic)
add_platlang_e2e_test(import_unknown)
add_platlang_e2e_test(import_canvas_protected_name)
