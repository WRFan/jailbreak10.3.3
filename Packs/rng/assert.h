/**
 * assert.h
 *
 * Copyright (C) 2018 Nickolas Burr <nickolasburr@gmail.com>
 */

#undef assert

//#ifdef NDEBUG # conflicts with cmake
	//#define assert(e) ((void)0)
//#else
	#include "except.h"
	extern void assert(int e);
	#define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))
//#endif
