cmake_minimum_required(VERSION 3.31.6)
project(kfs-lang VERSION 0.7.0 LANGUAGES C)
option(KFS_CURL_OPTION "compile with rest request support, lib curl" on )
option(KFS_THREAD_OPTION "compile with thread support, lib pthread" on )
set(CMAKE_C_STANDARD 11)
include(GNUInstallDirs)

find_package(Git QUIET)

string(TIMESTAMP NOW "%Y-%m-%d_%H:%M:%S")
execute_process(COMMAND git tag -fa ${PROJECT_VERSION} -m "tag ${PROJECT_VERSION} - ${NOW}" )
#execute_process(COMMAND git push origin tag -fa ${PROJECT_VERSION} )
FILE(WRITE ${CMAKE_BINARY_DIR}/version.h
			"\#define VERSION \"tag ${PROJECT_VERSION} compiled ${NOW}\"\n"
)

set(SRC_DIR         "src")
set(PARSER_DIR      "${CMAKE_CURRENT_BINARY_DIR}")
set(LEXER_OUT       "${PARSER_DIR}/lexer.c")
set(PARSER_OUT      "${PARSER_DIR}/parser.c")
set(JSON_LEXER_OUT  "${PARSER_DIR}/json_lexer.c")
set(JSON_PARSER_OUT "${PARSER_DIR}/json_parser.c")
set(ENV_LEXER_OUT   "${PARSER_DIR}/env_lexer.c")
set(ENV_PARSER_OUT  "${PARSER_DIR}/env_parser.c")
set(KFS_SOURCES ${SRC_DIR}/rt/expression.c
				${SRC_DIR}/rt/value.c
				${SRC_DIR}/rt/value_ops.c
				${SRC_DIR}/rt/kfs_lang_env.c
				${SRC_DIR}/rt/kfs_dict.c
				${SRC_DIR}/rt/json.c
				${SRC_DIR}/rt/options.c
				${SRC_DIR}/rt/env.c
				${LEXER_OUT}
				${PARSER_OUT}
				${JSON_LEXER_OUT}
				${JSON_PARSER_OUT}
				${ENV_LEXER_OUT}
				${ENV_PARSER_OUT}
			)

if (KFS_CURL_OPTION)
	message("compile with rest")
	set(KFS_SOURCES ${KFS_SOURCES}  ${SRC_DIR}/rt/request.c )
	link_libraries(curl)
else()
	message("compile w/o rest")
	set(KFS_SOURCES ${KFS_SOURCES}  ${SRC_DIR}/dummy/request.c )
endif()
unset(KFS_CURL_OPTION CACHE)

if (KFS_THREAD_OPTION)
	message("compile with thread")
	set(KFS_SOURCES ${KFS_SOURCES}  ${SRC_DIR}/rt/thread.c )
	link_libraries(pthread)
else()
	message("compile w/o thread")
	set(KFS_SOURCES ${KFS_SOURCES}  ${SRC_DIR}/dummy/thread.c )
endif()
unset(KFS_THREAD_OPTION CACHE)


find_package(BISON)
find_package(FLEX)

flex_target(LEXER "${SRC_DIR}/lexer.l" "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/lexer.h")
flex_target(JSON_LEXER "${SRC_DIR}/json.l" "${JSON_LEXER_OUT}" COMPILE_FLAGS "-P zz" DEFINES_FILE "${PARSER_DIR}/json_lexer.h")
flex_target(ENV_LEXER "${SRC_DIR}/env.l" "${ENV_LEXER_OUT}" COMPILE_FLAGS "-P env" DEFINES_FILE "${PARSER_DIR}/env_lexer.h")
bison_target(PARSER "${SRC_DIR}/parser.y" "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.h")
bison_target(JSON_PARSER "${SRC_DIR}/json.y" "${JSON_PARSER_OUT}" COMPILE_FLAGS "-p zz" DEFINES_FILE "${PARSER_DIR}/json_parser.h")
bison_target(ENV_PARSER "${SRC_DIR}/env.y" "${ENV_PARSER_OUT}" COMPILE_FLAGS "-p env" DEFINES_FILE "${PARSER_DIR}/env_parser.h")
add_flex_bison_dependency(LEXER PARSER JSON_LEXER JSON_PARSER)

include_directories(${SRC_DIR} ${SRC_DIR}/rt ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SRC_DIR}/rt/main.c ${KFS_SOURCES})
add_executable(kfs_lang_test   test/test.c          ${KFS_SOURCES})
add_executable(kfs_lang_pg     test/pg.c            ${KFS_SOURCES})

link_libraries(c)

enable_testing()

add_test(NAME test-1 COMMAND kfs_lang_test )
add_test(NAME test-pg COMMAND kfs_lang_pg --version --verbose -e ../test/test.env -s pako -s pako2)
add_test(NAME test-main COMMAND sh -c "$<TARGET_FILE:${PROJECT_NAME}> -i --verbose --version -e ../test/test.env < ../test/simple-test.kl" )
add_test(NAME test-main2 COMMAND $<TARGET_FILE:${PROJECT_NAME}> --version --verbose --dump -e ../test/test.env -f ../test/simple-test.kl -f ../test/simple-test-2.kl -s "return \"ahoj {{USER}}\";")
add_test(NAME test-dump COMMAND $<TARGET_FILE:${PROJECT_NAME}> -s "a=1;" -s "dump;" -s "a=a*100+2;" -s "dump;" -s "a=a*100+3;" -s "return a==10203;" -s "dump; print (a);")
add_test(NAME test-json COMMAND $<TARGET_FILE:${PROJECT_NAME}> -s "a = json('[ 2, 3, 11, 0 ]'); print (a);" -f ../test/test_json.kl )


install(TARGETS ${PROJECT_NAME} DESTINATION usr/bin )

include(CPack)
set(CPACK_SOURCE_GENERATOR "TGZ")


