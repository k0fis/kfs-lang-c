cmake_minimum_required(VERSION 3.31)
project(kfs_lang C)

set(CMAKE_C_STANDARD 11)

set(SRC_DIR "src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(LEXER_OUT  "${PARSER_DIR}/lexer.c")
set(PARSER_OUT "${PARSER_DIR}/parser.c")

find_package(BISON)
find_package(FLEX)

flex_target(LEXER "${SRC_DIR}/lexer.l" "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/lexer.h")
bison_target(PARSER "${SRC_DIR}/parser.y" "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.h")
add_flex_bison_dependency(LEXER PARSER)

include_directories(${SRC_DIR} ${SRC_DIR}/rt ${CMAKE_CURRENT_BINARY_DIR})

add_executable(kfs_lang test/test.c
		${SRC_DIR}/rt/expression.c ${SRC_DIR}/rt/value.c ${SRC_DIR}/rt/value_ops.c
		${SRC_DIR}/rt/kfs_lang_env.c ${SRC_DIR}/rt/kfs_dict.c
        ${LEXER_OUT}
        ${PARSER_OUT})

target_include_directories(kfs_lang PRIVATE ${PARSER_DIR} ${SRC_DIR})
