cmake_minimum_required(VERSION 3.31)
project(kfs_lang C)

set(CMAKE_C_STANDARD 11)

set(SRC_DIR "src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(LEXER_OUT  "${PARSER_DIR}/lexer.c")
set(PARSER_OUT "${PARSER_DIR}/parser.c")
set(JSON_LEXER_OUT  "${PARSER_DIR}/json_lexer.c")
set(JSON_PARSER_OUT "${PARSER_DIR}/json_parser.c")
set(KFS_SOURCES
		${SRC_DIR}/rt/expression.c
		${SRC_DIR}/rt/value.c
		${SRC_DIR}/rt/value_ops.c
		${SRC_DIR}/rt/kfs_lang_env.c
		${SRC_DIR}/rt/kfs_dict.c
		${SRC_DIR}/rt/json.c
		${LEXER_OUT}
		${PARSER_OUT}
		${JSON_LEXER_OUT}
		${JSON_PARSER_OUT}
	)

find_package(BISON)
find_package(FLEX)

flex_target(LEXER "${SRC_DIR}/lexer.l" "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/lexer.h")
flex_target(JSON_LEXER "${SRC_DIR}/json.l" "${JSON_LEXER_OUT}" COMPILE_FLAGS "-P zz" DEFINES_FILE "${PARSER_DIR}/json_lexer.h")
bison_target(PARSER "${SRC_DIR}/parser.y" "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.h")
bison_target(JSON_PARSER "${SRC_DIR}/json.y" "${JSON_PARSER_OUT}" COMPILE_FLAGS "-p zz" DEFINES_FILE "${PARSER_DIR}/json_parser.h")
add_flex_bison_dependency(LEXER PARSER JSON_LEXER JSON_PARSER)

include_directories(${SRC_DIR} ${SRC_DIR}/rt ${CMAKE_CURRENT_BINARY_DIR})

add_executable(kfs-lang      ${SRC_DIR}/rt/main.c ${KFS_SOURCES})
add_executable(kfs_lang_test test/test.c          ${KFS_SOURCES})
add_executable(kfs_lang_pg   test/pg.c            ${KFS_SOURCES})

target_include_directories(kfs-lang      PRIVATE ${PARSER_DIR} ${SRC_DIR})
target_include_directories(kfs_lang_test PRIVATE ${PARSER_DIR} ${SRC_DIR})
target_include_directories(kfs_lang_pg   PRIVATE ${PARSER_DIR} ${SRC_DIR})

enable_testing()

add_test(NAME test-1 COMMAND kfs_lang_test )
add_test(NAME test-pg COMMAND kfs_lang_pg )
add_test(NAME test-main COMMAND sh -c "$<TARGET_FILE:kfs-lang> < ../test/simple-test.kl" )