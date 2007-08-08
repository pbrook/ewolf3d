#ifndef _INTEGER
#define _INTEGER

typedef signed int		INT;
typedef unsigned int	UINT;

/* These types are assumed as 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types are assumed as 16-bit integer */
typedef signed short	SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;

/* These types are assumed as 32-bit integer */
typedef signed int		LONG;
typedef unsigned int	ULONG;
typedef unsigned int	DWORD;

/* Boolean type */
typedef enum { FALSE = 0, TRUE } BOOL;

#endif
