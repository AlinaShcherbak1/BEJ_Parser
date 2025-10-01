cmake_minimum_required(VERSION 3.12)
project(bej2json C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)

add_library(bej STATIC
    src/bej_decode.c
    src/dict.c
    src/io.c
)
target_include_directories(bej PUBLIC include)


find_package(Doxygen QUIET)
if (DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

add_executable(bej2json src/main.c)
target_link_libraries(bej2json PRIVATE bej)
target_compile_definitions(bej PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(bej2json PRIVATE _CRT_SECURE_NO_WARNINGS)
if(BUILD_TESTS)
  add_executable(test_nnint tests/test_nnint.c)
  target_link_libraries(test_nnint PRIVATE bej)
  add_test(NAME test_nnint COMMAND test_nnint)
  enable_testing()
endif()
