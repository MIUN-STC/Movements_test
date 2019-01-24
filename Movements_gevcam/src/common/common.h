#pragma once

#include "debug.h"
#include <inttypes.h>
#include <time.h>
#include <stdio.h>



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


void print_time (FILE * f, time_t s)
{
	struct tm * t = localtime (&s);
	fprintf 
	(
		f,
		"%04i-%02i-%02i %02i:%02i:%02i ", 
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec
	);
}
