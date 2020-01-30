#include <alloc.h>
#include <stdio.h>
#include <stdlib.h>

// Define memory allocation functions.	If using DOS memory allocation
// functions, provided through DOSCALLS, then set the conditional compilation
// 'DOSALLOC' to true.  If using C compiler library function memory allocation
// set 'DOSALLOC' to zero.  MUST BE SET TO 1 for RECURSE, because most memory
// allocation has to be on a paragraph boundary!

#define DOSALLOC 0
// Redirect memory allocation to either DOS memory allocate functions located
// in DOSCALLS or to C library far memory allocation functions.
unsigned char far * far memalloc(long int siz)
{
	unsigned char far *mem;

	#if DOSALLOC
		mem = fmalloc(siz);  // DOS far memory allocation functions
	#else
		mem =(unsigned char far *) farmalloc(siz); // C's far memory allocation functions.
	#endif
	return(mem);
}

void far memfree(char far *memory)
{
	#if DOSALLOC
		ffree(memory);
	#else
		farfree(memory);
	#endif
}


