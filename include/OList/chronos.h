/***************************************************************************
******************************* 15/Dec/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:	chronos.h						   *
*                                                                          *
* PURPOSE:	Type definition and Prototypes for Chronos Functions	   *
*                                                                          *
*   NOTES:								   *
*                                                                          *
****************************************************************************
***************************************************************************/

#ifndef CHRONOS_H	/* If CHRONOS_H is already defined all this file */
			/* must be skipped.			       */
#define CHRONOS_H

#ifdef NOMACHINE
#define MSDOS
#endif

#ifdef SGI
#define SVR5
#endif

#ifdef HP
#define SVR5
#endif

#include <stdlib.h>
#include <stdio.h>

#ifdef MSDOS
 #include <sys\types.h>
 #include <sys\timeb.h>
#endif

#ifdef SUN
 #include <sys/time.h>
 #include <sys/resource.h>
#endif

#ifdef SVR5
 #include <unistd.h>
 #include <sys/times.h>
 #include <sys/param.h>
#endif
/***************************************************************************
*   CONST:	USER_CHRONOS						   *
*									   *
* PURPOSE:	Defining a null value for indicating the use of a global   *
*		Chronos instead of a local passed Chronos Pointer.	   *
*									   *
*   NOTES:	The user can use also NULL instead of this Constant	   *
*									   *
***************************************************************************/

#define USER_CHRONOS NULL

#define RUNNING_CHRONOS 2
#define STOPPED_CHRONOS 1
#define READY_CHRONOS	0



/***************************************************************************
*									   *
*    TYPE:	Chronos							   *
*									   *
* PURPOSE:	Defining a type for cronometering time used by a program   *
*		in machine indipendent way.				   *
*									   *
*   NOTES:								   *
*									   *
***************************************************************************/

struct cronotag
{
#ifdef MSDOS			       /* MS DOS */
 struct timeb tp1,tp2,tp3;
 double sec1,sec2,sec3;
#endif

#ifdef ncube			       /* Ncube O.S. */
 int sec1,sec2,sec3;
#endif

#ifdef SUN			       /* Sun Os 4.1. */
 struct rusage r1,r2,r3;
#endif

#ifdef SVR5
 struct tms r1,r2,r3;
#endif

 double Time;
 int State;
};

typedef struct cronotag* Chronos;


/***************************************************************************
*	Functions in chronos.c						   *
***************************************************************************/

Chronos  NewChronos();
void   StartChronos(Chronos c);
void   StopChronos(Chronos c);
double ReadChronos(Chronos c);
double ReadSystemChronos(Chronos c);
void   ResetChronos(Chronos c);
void   RestartChronos(Chronos c);


#endif		/* this #endif is the brother of #ifndef CHRONOS_H.*/
		/* If CHRONOS_H was already defined all this file  */
		/* must be skipped.                              */
