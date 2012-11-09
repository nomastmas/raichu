#ifdef __cplusplus
#include <stdlib.h>

/**
 * This file defines where C++ new and delete operators go
 * to handle memory allocation and deallocation.
 */


void* operator new(size_t s)
{
	return malloc(s);
}
void operator delete(void* p)
{
	free(p);
}
void *operator new[] (size_t s)
{
	return malloc(s);
}

void operator delete[] (void* p)
{
	free(p);
}


#endif
