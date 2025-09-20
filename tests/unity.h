#ifndef MINI_UNITY_H
#define MINI_UNITY_H

#include <stdio.h>

extern int tests_run;
extern int tests_failed;

#define TEST_ASSERT_TRUE(cond) do { tests_run++; if (!(cond)) { tests_failed++; fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); return; } } while (0)
#define TEST_ASSERT_EQ_U64(a,b) do { tests_run++; if ((unsigned long long)(a)!=(unsigned long long)(b)) { tests_failed++; fprintf(stderr, "Assertion failed: %s == %s (%llu != %llu) at %s:%d\n", #a,#b,(unsigned long long)(a),(unsigned long long)(b),__FILE__,__LINE__); return; } } while (0)
#define TEST_CASE(name) static void name(void)

void run(void (*fn)(void), const char *n);
int unity_finish(void);

#endif
