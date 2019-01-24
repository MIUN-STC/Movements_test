#pragma once

#include "debug.h"
#include <inttypes.h>


#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))
#define COUNTE(x) (sizeof(x) / sizeof(*(x)))

uint32_t str_u32 (char const * str)
{
	uintmax_t value;
	value = strtoumax (str, NULL, 10);
	ASSERT_F (value != UINTMAX_MAX && errno != ERANGE, "strtoumax error%s", "");
	return (uint32_t) value;
}


intmax_t str_imax (char const * str)
{
	intmax_t value;
	value = strtoimax (str, NULL, 10);
	ASSERT_F (value != INTMAX_MAX && errno != ERANGE, "strtoumax error%s", "");
	return value;
}
