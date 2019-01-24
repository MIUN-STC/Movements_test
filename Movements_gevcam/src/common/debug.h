#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ANSIC.h"

//https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#define __RELATIVE_FILE__ (strrchr("/" __FILE__, '/') + 1)

//https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html
#define ASSERT_F(A, F, ...) if (!(A)) {assert_format (__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, #A, F, ## __VA_ARGS__);}
#define TRACE_F(F, ...) trace_format (__COUNTER__, __RELATIVE_FILE__, __LINE__, __func__, F, ## __VA_ARGS__)


__attribute__ ((__unused__))
static void assert_format (int id, char const * file, int line, char const * fn, char const * exp, char const * fmt, ...)
{
	va_list list;
	va_start (list, fmt);
	fprintf	
	(
		stderr,
		ANSIC (ANSIC_BOLD, ANSIC_RED, ANSIC_DEFAULT) "ASSERT" ANSIC_RESET " "
		"%04i "
		ANSIC (ANSIC_BOLD, ANSIC_BLUE, ANSIC_DEFAULT) "%s" ANSIC_RESET ":"
		ANSIC (ANSIC_BOLD, ANSIC_BLUE, ANSIC_DEFAULT) "%04i" ANSIC_RESET " in "
		ANSIC (ANSIC_NORMAL, ANSIC_YELLOW , ANSIC_DEFAULT) "%s" ANSIC_RESET " () " 
		ANSIC (ANSIC_BOLD, ANSIC_BLACK, ANSIC_RED) "[%s]" ANSIC_RESET " "
		ANSIC (ANSIC_BOLD, ANSIC_RED , ANSIC_DEFAULT) "[%04i:%s]: " ANSIC_RESET,
		id, 
		file, 
		line, 
		fn, 
		exp,
		errno,
		strerror (errno)
	);
	vfprintf (stderr, fmt, list);
	fprintf (stderr, "\n");
	va_end (list);
	abort ();
}

__attribute__ ((__unused__))
static void trace_format (int id, char const * file, int line, char const * fn, char const * fmt, ...)
{
	va_list list;
	va_start (list, fmt);
	fprintf	
	(
		stderr,
		"TRACE %04i [%s:%04i in %s ()]: ",
		id, 
		file, 
		line, 
		fn
	);
	vfprintf (stderr, fmt, list);
	fprintf (stderr, "\n");
	va_end (list);
}
