# CMake generated Testfile for 
# Source directory: C:/Users/Alina/Downloads/BEJ_Parser_C/tests
# Build directory: C:/Users/Alina/Downloads/BEJ_Parser_C/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(bej_tests "C:/Users/Alina/Downloads/BEJ_Parser_C/build/tests/Debug/bej_tests.exe")
  set_tests_properties(bej_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;6;add_test;C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(bej_tests "C:/Users/Alina/Downloads/BEJ_Parser_C/build/tests/Release/bej_tests.exe")
  set_tests_properties(bej_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;6;add_test;C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(bej_tests "C:/Users/Alina/Downloads/BEJ_Parser_C/build/tests/MinSizeRel/bej_tests.exe")
  set_tests_properties(bej_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;6;add_test;C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(bej_tests "C:/Users/Alina/Downloads/BEJ_Parser_C/build/tests/RelWithDebInfo/bej_tests.exe")
  set_tests_properties(bej_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;6;add_test;C:/Users/Alina/Downloads/BEJ_Parser_C/tests/CMakeLists.txt;0;")
else()
  add_test(bej_tests NOT_AVAILABLE)
endif()
