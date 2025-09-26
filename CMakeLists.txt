cmake_minimum_required(VERSION 3.13)
project(bej_parser C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Werror=return-type)
endif()
if (WIN32)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(bej_core
  src/bej.c
  src/bej_dict.c
  src/bitbuf.c
  src/json_writer.c
  src/util.c)

add_executable(bej2json src/main.c)
target_link_libraries(bej2json bej_core)

enable_testing()
add_subdirectory(tests)

find_package(Doxygen QUIET)
if (DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

add_executable(make_demo_bej src/demo_encode.c)
