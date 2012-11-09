/**
 * Common definitions of variable with explicit size
 */


#ifndef _SYSDEFS_H_
#define _SYSDEFS_H_

typedef unsigned char U8;
typedef signed char N8;
typedef unsigned short U16;
typedef signed short N16;
typedef unsigned int U32;
typedef signed int N32;

typedef volatile U8 REG8;
typedef volatile U16 REG16;
typedef volatile U32 REG32;

#define pREG8  (REG8 *)
#define pREG16 (REG16 *)
#define pREG32 (REG32 *)

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define MINIMUM(x,y) ((x)<(y)?(x):(y))
#define MAXIMUM(x,y)((x)>(y)?(x):(y))
#define arrsizeof(x) ((sizeof (x))/(sizeof (x [0])))


#endif
