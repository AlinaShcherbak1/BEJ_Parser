/* Minimal Unity replacement: tiny asserts and runner. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tests_run = 0;
int tests_failed = 0;

#define TEST_ASSERT_TRUE(cond) do { tests_run++; if (!(cond)) { tests_failed++; fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); return; } } while (0)
#define TEST_ASSERT_EQ_U64(a,b) do { tests_run++; if ((unsigned long long)(a)!=(unsigned long long)(b)) { tests_failed++; fprintf(stderr, "Assertion failed: %s == %s (%llu != %llu) at %s:%d\n", #a,#b,(unsigned long long)(a),(unsigned long long)(b),__FILE__,__LINE__); return; } } while (0)
#define TEST_CASE(name) static void name(void)

void run(void (*fn)(void), const char *n) { fn(); fprintf(stderr, ". %s\n", n); }

int unity_finish(void) { if (tests_failed) { fprintf(stderr, "FAILED %d/%d tests\n", tests_failed, tests_run); return 1; } fprintf(stderr, "OK %d tests\n", tests_run); return 0; }
