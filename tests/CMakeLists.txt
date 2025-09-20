add_executable(bej_tests
  unity.c
  test_varints.c)

target_link_libraries(bej_tests bej_core)
add_test(NAME bej_tests COMMAND bej_tests)
