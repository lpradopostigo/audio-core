#pragma once
#include <stdio.h>

#define ASSERT(message, test) do {if ((test)){printf("[OK] %s\n",message);} else {return message;}} while (0)

#define TEST(name, body) static char* name() { printf("Test: %s\n",#name); body; return 0; }

#define RUN_TEST(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)

#define INFO(message) do { printf("[INFO] %s\n",message); } while (0)

#define CONCAT(a, b) (a b)

extern int tests_run;