/***************************************************************************
******************************* 05/Sep/92 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	general.h						   *
*									   *
* PURPOSE:	Define useful general purpose costants and macros.	   *
*									   *
*   NOTES:	It can be harmlessy included more than one time.	   *
*		All the macro are usually capitalized to individue	   *
*		them in code.						   *
*									   *
/***************************************************************************
***************************************************************************/

#ifndef GENERAL_H 	/* If GENERAL_H is already defined all this file  */
			/* must be skipped.                               */
#define GENERAL_H

/***********************/
/* one-argument macros */
/***********************/

/* Absolute value of a */
#ifndef ABS
 #define ABS(a)          (((a)<0) ? -(a) : (a))
#endif

/* Round a to nearest integer towards 0 */
#ifndef FLOOR
#define FLOOR(a)                ((a)>0 ? (int)(a) : -(int)(-a))
#endif

/* Round a to nearest integer away from 0 */
#ifndef CEILING
#define CEILING(a) \
((a)==(int)(a) ? (a) : (a)>0 ? 1+(int)(a) : -(1+(int)(-a)))
#endif

/* round a to nearest int */
#ifndef ROUND
#define ROUND(a)        ((a)>0 ? (int)(a+0.5) : -(int)(0.5-a))
#endif

/* take sign of a, either -1, 0, or 1 */
#ifndef ZSGN
#define ZSGN(a)         (((a)<0) ? -1 : (a)>0 ? 1 : 0)
#endif

/* take binary sign of a, either -1, or 1 if >= 0 */
#ifndef SGN
#define SGN(a)          (((a)<0) ? -1 : 0)
#endif

/* shout if something that should be true isn't */
/*
#ifndef ASSERT
#define ASSERT(x) \
if (!(x)) fprintf(stderr," Assert failed: x\n");
#endif
*/

/* square a */
#ifndef SQR
#define SQR(a)          ((a)*(a))
#endif


/***********************/
/* two-argument macros */
/***********************/

/* find minimum of a and b */
#ifndef MIN
#define MIN(a,b)        (((a)<(b))?(a):(b))
#endif

/* find maximum of a and b */
#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif

/* swap a and b (see Gem by Wyvill) */
#ifndef SWAP
#define SWAP(a,b)       { a^=b; b^=a; a^=b; }
#endif

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#ifndef LERP
#define LERP(a,l,h)     ((l)+(((h)-(l))*(a)))
#endif

/* clamp the input to the specified range */
#ifndef CLAMP
#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)
#endif

/****************************/
/* memory allocation macros */
/****************************/

/* create a new instance of a structure (see Gem by Hultquist) */
#define NEWSTRUCT(x)    (struct x *)(malloc((unsigned)sizeof(struct x)))

/* create a new instance of a type */
#define NEWTYPE(x)      (x *)(malloc((unsigned)sizeof(x)))

/********************/
/* useful constants */
/********************/

#ifndef PI
#define PI              3.141592        /* the venerable pi */
#endif
#ifndef PITIMES2
#define PITIMES2        6.283185        /* 2 * pi */
#endif

#ifndef PIOVER2
#define PIOVER2         1.570796        /* pi / 2 */
#endif

#ifndef theE
#define theE		 2.718282	 /* the venerable e */
#endif

#ifndef SQRT2
#define SQRT2           1.414214        /* sqrt(2) */
#endif

#ifndef SQRT3
#define SQRT3           1.732051        /* sqrt(3) */
#endif

#ifndef GOLDEN
#define GOLDEN          1.618034        /* the golden ratio */
#endif

#ifndef DTOR
#define DTOR            0.017453        /* convert degrees to radians */
#endif

#ifndef RTOD
#define RTOD            57.29578        /* convert radians to degrees */
#endif


/************/
/* booleans */
/************/

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifndef ON
#define ON              1
#endif

#ifndef OFF
#define OFF             0
#endif

#ifndef NULL
#define NULL		0
#endif

#ifndef boolean
typedef int boolean;			/* boolean data type */
#endif

#ifndef flag
typedef boolean flag;                   /* flag data type    */
#endif

#endif		/* this #endif is the brother of #ifndef GENERAL_H. */
		/* If GENERAL_H was already defined all this file   */
		/* must be skipped.                                 */
