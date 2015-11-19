/***************************************************************************
******************************* 15/Dec/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:      chronos.c                                                  *
*                                                                          *
* PURPOSE:      Machine indipendent timing function.                       *
*                                                                          *
* EXPORTS:      NewChronos                                                 *
*               StartChronos                                               *
*               StopChronos                                                *
*               ReadChronos                                                *
*               ResetChronos                                               *
*               RestartChronos                                             *
*   NOTES:                                                                 *
*                                                                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include <OList/chronos.h>
#include <stdlib.h>
#include <stdlib.h>
#include <limits.h>

/***************************************************************************
*                                                                          *
* Global Variables                                                         *
*                                                                          *
* These global variables are for supporting the use of a not local         *
* cronometer; User can do                                                  *
*                                                                          *
* StartChronos(USER_CHRONOS);                                              *
* StopChronos(USER_CHRONOS);                                               *
*                                                                          *
* without defining any Chronos variable.                                           *
*                                                                          *
***************************************************************************/

Chronos GlobalC=NULL;


/***************************************************************************
*                                                                          *
* FUNCTION:     NewChronos                                                 *
*                                                                          *
*  PURPOSE:     Create a new istance of a Chronos.                         *
*                                                                          *
*   PARAMS:     None                                                       *
*                                                                          *
*   RETURN:     The Created Chronos,                                       *
*               NULL if creation fails.                                    *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/
Chronos NewChronos()
{
 Chronos C=NULL;

 C=(Chronos)malloc(sizeof(struct cronotag));
 if(C)
   {
    C->Time=0;
    C->State=READY_CHRONOS;
    return C;
   }
 else return NULL;
}


/***************************************************************************
*                                                                          *
* FUNCTION:     ResetChronos                                               *
*                                                                          *
*  PURPOSE:     Reset a Chronos.                                                   *
*                                                                          *
*   PARAMS:     The pointer to Chronos to reset.                                   *
*                                                                          *
*   RETURN:     The created List if successful,                            *
*               NULL if creation goes wrong.                               *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/
void ResetChronos(Chronos C)
{
 if(C==USER_CHRONOS)
      {
       if(!GlobalC) GlobalC=NewChronos();
       C=GlobalC;
      }
 C->Time=0;
 C->State=READY_CHRONOS;
}


/***************************************************************************
*                                                                          *
* FUNCTION:     StartChronos                                               *
*                                                                          *
*  PURPOSE:     Start a Chronos.                                                   *
*                                                                          *
*   PARAMS:     The pointer to Chronos to start.                                   *
*                                                                          *
*   RETURN:     The created List if successful,                            *
*               NULL if creation goes wrong.                               *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/
void StartChronos(Chronos C)
{
 if(C==USER_CHRONOS)
      {
       if(!GlobalC) GlobalC=NewChronos();
       C=GlobalC;
      }

 if(!( (C->State==READY_CHRONOS || C->Time==0) ||
       (C->State==STOPPED_CHRONOS) ) ) ResetChronos(C);

 C->State=RUNNING_CHRONOS;

 #ifdef MSDOS
   ftime( &(C->tp1) );
   C->sec1=(double)C->tp1.time+(double)C->tp1.millitm/1000.0;
 #endif

 #ifdef ncube
   C->sec1=micclk();
 #endif

 #ifdef SUN
   getrusage(RUSAGE_SELF, &(C->r1));
 #endif
 
 #ifdef SVR5
   times(&(C->r1));
 #endif

 #ifdef RISC6000
   getrusage(RUSAGE_SELF, &(C->r1));
 #endif
}


/***************************************************************************
*                                                                          *
* FUNCTION:     StopChronos                                                *
*                                                                          *
*  PURPOSE:     Stop a Chronos.                                            *
*                                                                          *
*   PARAMS:     The pointer to Chronos to stop.                            *
*                                                                          *
*   RETURN:     The created List if successful,                            *
*               NULL if creation goes wrong.                               *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/
void StopChronos(Chronos C)
{
 if(C==USER_CHRONOS) C=GlobalC;

 C->State=STOPPED_CHRONOS;

 #ifdef MSDOS
   ftime( &(C->tp2) );

   C->sec2=(double)C->tp2.time+(double)C->tp2.millitm/1000.0;
   C->Time = C->sec2-C->sec1;
 #endif

 #ifdef ncube
   C->sec2=micclk();
   C->Time = (double)(C->sec2-C->sec1)/1000000.0;
 #endif

 #ifdef SUN
   getrusage(RUSAGE_SELF, &(C->r2));
   C->Time = C->r2.ru_utime.tv_sec + C->r2.ru_utime.tv_usec/1000000.0 - 
	      C->r1.ru_utime.tv_sec - C->r1.ru_utime.tv_usec/1000000.0;
 #endif
 
 #ifdef SVR5
  times(&(C->r2));
  C->Time = (double)(C->r2.tms_utime - C->r1.tms_utime)/CLK_TCK;
 #endif

 #ifdef RISC6000
   getrusage(RUSAGE_SELF, &(C->r2));
   C->Time = C->r2.ru_utime.tv_sec + C->r2.ru_utime.tv_usec/1000000.0 - 
	      C->r1.ru_utime.tv_sec - C->r1.ru_utime.tv_usec/1000000.0;
 #endif
}


/***************************************************************************
*                                                                          *
* FUNCTION:     ReadChronos                                                *
*                                                                          *
*  PURPOSE:     Read time on a Chronos.                                    *
*                                                                          *
*   PARAMS:     The pointer to Chronos to Read.                            *
*                                                                          *
*   RETURN:     A double that is the read time in seconds and fractions.   *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/
double ReadChronos(Chronos C)
{
 if(C==USER_CHRONOS) C=GlobalC;

 if(C->State==STOPPED_CHRONOS || C->State==READY_CHRONOS) return C->Time;
 else
   {
     #ifdef MSDOS
       ftime( &(C->tp3) );

       C->sec3=(double)C->tp3.time+(double)C->tp3.millitm/1000.0;
       return C->sec3-C->sec1;
     #endif

     #ifdef ncube
       C->sec3=micclk();
       return (double)(C->sec3-C->sec1)/1000000.0;
     #endif

     #ifdef SUN
       getrusage(RUSAGE_SELF, &(C->r3));
       return C->r3.ru_utime.tv_sec + C->r3.ru_utime.tv_usec/1000000.0 - 
	      C->r1.ru_utime.tv_sec - C->r1.ru_utime.tv_usec/1000000.0;
     #endif

     #ifdef SVR5 
      times(&(C->r3)); 
      return (double)(C->r3.tms_utime - C->r1.tms_utime)/CLK_TCK; 
     #endif 
 
     #ifdef RISC6000
       getrusage(RUSAGE_SELF, &(C->r3));
       C->Time = C->r3.ru_utime.tv_sec + C->r2.ru_utime.tv_usec/1000000.0 - 
	      C->r1.ru_utime.tv_sec - C->r3.ru_utime.tv_usec/1000000.0;
     #endif
   }
}

/***************************************************************************
*                                                                          *
* FUNCTION:     RestartChronos                                             *
*                                                                          *
*  PURPOSE:     Restart a Chronos.                                         *
*                                                                          *
*   PARAMS:     The pointer to Chronos to Restart.                         *
*                                                                          *
*   RETURN:     The created List if successful,                            *
*               NULL if creation goes wrong.                               *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/

void RestartChronos(Chronos C)
{
 ResetChronos(C);
 StartChronos(C);
}

/***************************************************************************
*                                                                          *
* FUNCTION:     ReadSystemChronos                                          *
*                                                                          *
*  PURPOSE:     Read System time on a Chronos.                             *
*                                                                          *
*   PARAMS:     The pointer to Chronos to Read.                            *
*                                                                          *
*   RETURN:     A double that is the read time in seconds and fractions.   *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:                                                                *
*                                                                          *
***************************************************************************/
double ReadSystemChronos(Chronos C)
{
 if(C==USER_CHRONOS) C=GlobalC;

 if(C->State==STOPPED_CHRONOS || C->State==READY_CHRONOS) return C->Time;
 else
   {
     #ifdef MSDOS
       return 0; /* On a MSDOS machine there is NO System Time */
     #endif

     #ifdef ncube
       C->sec3=micclk();
       return (double)(C->sec3-C->sec1)/1000000.0;
     #endif

     #ifdef SUN
       getrusage(RUSAGE_SELF, &(C->r3));
       return C->r3.ru_utime.tv_sec + C->r3.ru_utime.tv_usec/1000000.0 - 
	      C->r1.ru_utime.tv_sec - C->r1.ru_utime.tv_usec/1000000.0;
     #endif

     #ifdef SVR5 
      times(&(C->r3)); 
      return (double)(C->r3.tms_stime - C->r1.tms_stime)/CLK_TCK; 
     #endif 
 
     #ifdef RISC6000
       getrusage(RUSAGE_SELF, &(C->r3));
       C->Time = C->r3.ru_utime.tv_sec + C->r2.ru_utime.tv_usec/1000000.0 - 
	      C->r1.ru_utime.tv_sec - C->r3.ru_utime.tv_usec/1000000.0;
     #endif
   }
}
