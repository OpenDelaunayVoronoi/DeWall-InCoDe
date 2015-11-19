/***************************************************************************
******************************* 14/Sep/92 **********************************
****************************   Version 2.0   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	bubbles.c						   *
*									   *
* PURPOSE:	Creating bubble of sites in 3D				   *
*									   *
*									   *
* EXPORTS:	ProgramName						   *
*									   *
* IMPORTS:	Error							   *
*		ErrorNULL						   *
*		ErrorFALSE						   *
*									   *
* GLOBALS:	ProgramName						   *
*									   *
*   NOTES:	This file assume the declaration of external variable	   *
*		ProgramName containing a string with the program name.	   *
*									   *
*		This is a one day program rather unstructured and uncom-   *
*		mented but it does its work.				   *
*									   *
* HISTORY:								   *
*									   *
*    1.0  14/09/92	The first functioning bubbles.			   *
*									   *
*    1.1  08/10/92	Added [-2d|-3d] options.			   *
*			Added box filling for single bubble case.	   *
*									   *
*    2.0  21/02/94      Added [-d n] option for n dimensional case         *
*									   *
****************************************************************************
***************************************************************************/


#include "error.h"
#include "general.h"

#include<time.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<limits.h>

#ifndef MSDOS
#define rand() random()
#define srand(a) srandom(a)
#ifndef RAND_MAX
#define RAND_MAX LONG_MAX
#endif
#endif

#define STD   0
#define GEOM  1
#define QHULL 2
#define DETRI 3


#define USAGE "Usage: bubbles [-u|-n] [-c|-r|-v] [-d n] npoints [boxsize] [nbubbles]\n\
	-u   (default)	use uniform distribution			\n\
	-n		use an approximation of normal distribution	\n\
									\n\
	-c   (default)	Constant number of sites for bubble.		\n\
	-r		Random number of sites for bubble.		\n\
	-v		Number of sites proprortional to bubble volume. \n\
\n\
	-G		Out in Geomview format                          \n\
        -Q              Out in Qhull format                             \n\
        -D              Out in Detri format                             \n\
\n\
	-s n		Set Random seed = n	   	                \n\
        -d n            Sites in n dimensions (-d 2 default).           \n\
\n\
	npoints		total number of sites produced			\n\
	nbubbles	number of bubbles (clusters) of sites		\n\
	boxsize		size of box containing all sites		\n\
"

#define CONSTANT 0
#define RANDOM	 1    /* Constants for bubble site number */
#define VOLUME	 2

typedef struct Point3Struct
{
 double x;
 double y;
 double z;
} Point3;

typedef Point3 Vector3;



/***************************************************************************
*									   *
* FUNCTION:	main							   *
*									   *
*  PURPOSE:	Doing all the work.					   *
*									   *
*   PARAMS:	...							   *
*									   *
*   RETURN:	0 if succesful.						   *
*									   *
*  IMPORTS:	Nothing.						   *
*									   *
*    NOTES:	This function creates the randomly clusterd sites.	   *
*									   *
*    Usage: bubbles [-u|-n] [-c|-r|-v] [-d n] npoints [boxsize] [nbubbles] *
*									   *
*		where:							   *
*									   *
*	-u   (default)	use uniform distribution			   *
*	-n		use an approximation of normal distribution	   *
*									   *
*	-c   (default)	Constant number of sites for bubble.		   *
*	-r		Random number of sites for bubble.		   *
*	-v		Number of sites proprortional to bubble volume.    *
*	-G		Out in Geomview format                             *
*	-s n		Set Random seed = n	   	                   *
*       -d n            Sites in n dimensions (-d 2 default).              *
*									   *
*	npoints		total number of sites produced			   *
*	nbubbles	number of bubbles (clusters) of sites		   *
*	boxsize		size of box containing all sites		   *
*									   *
*	Assumptions							   *
*									   *
*	The size of bubbles depends from their number. In a box of side r  *
*	and volume r3 the number nball(k) of ball of diameter 1/k r is:    *
*									   *
*		     1	    3		  nball(1/2) = 4		   *
*	      nball( - ) = k  * 1/2   so  nball(1/3) = 14		   *
*		     k			  nball(1/4) = 32		   *
*									   *
*	This formula appear clear if you think to 3dchecker a cube with k  *
*	tile for side; nball is the number of black cube.		   *
*	inverting this formula we have that the radius of n ball contained *
*	in a cube is:							   *
*		   _______						   *
*		3 /							   *
*	    k = \/ 2 nball						   *
*									   *
*	Because we interest to the max radius we multipy 1/k per 1.5 so    *
*	the bubbles are not too little. 				   *
*									   *
*									   *
*	When Normal distribution is used then the median is the center of  *
*	each bubble.							   *
*									   *
*	Implementative Notes						   *
*									   *
*	To approximate Normal Distribution we use:			   *
*									   *
*				     .- - +inf				   *
*			 Note that   |					   *
*	   1		 it's rather |	1				   *
*	  --- atan(x)	 correct     | --- atan(x) dx = 1		   *
*	   PI		 because:    |	PI				   *
*				     |					   *
*				    -'	-inf				   *
*									   *
*									   *
*									   *
*	So if -1 < x < +1 is generated with uniform density then	   *
*	tan(x*PI/2) is distribuited approximately with normal distribution.*
*									   *
*									   *
*									   *
***************************************************************************/

main(int argc, char *argv[])
{
 int i=1,j,k;

 int Dim=3;
 #define MAXDIM 10
 #define RAND_MAX 0x8fffffff
 boolean UniformDistribution=TRUE;    /* Flags for command line arguments */
 int Format = STD;
 int SitePerBubble=CONSTANT;

 long npoints, nbubbles=1;
 double boxsize=1, boxradius;

 long *bubblesize;
 long total=0, total2=0;

 double x ;

 double *bubblecenter;
 double *bubbleradius;
 double bubblemaxradius;
 double coord[MAXDIM];
 double dist;
 SetProgramName("bubbles");
 srand( (unsigned)time( NULL ) );
 
 if(argc<2) Error(USAGE,EXIT);

 while(*argv[i]=='-')
    {
      switch((argv[i])[1])
	 {
	  case 'G': Format=GEOM;	break;
	  case 'Q': Format=QHULL;	break;
	  case 'D': Format=DETRI;	break;
	  case 's': if(argv[i][2]=='\0' && isdigit(argv[i+1][0]))
	                  srand( (unsigned)atoi(argv[++i])); 
  	            else  srand( (unsigned)atoi(argv[i]+2)); 
                   break;
	  case 'u': UniformDistribution=TRUE;	break;
	  case 'n': UniformDistribution=FALSE;	break;
	  case 'c': SitePerBubble=CONSTANT;	break;
	  case 'r': SitePerBubble=RANDOM;	break;
	  case 'v': SitePerBubble=VOLUME;	break;
          case 'd' : if(argv[i][2]=='\0' && isdigit(argv[i+1][0]))
                         Dim=atoi(argv[++i]);
                    else Dim=atoi(argv[i]+2);
                   break;
          default : Errorf(NO_EXIT,"Error: Unknown switch %s.\n",argv[i]);
	 }

      i++;
    }
 if(argc-i<1) Error(USAGE,EXIT);
 npoints=(long)atoi(argv[i++]);

 if(argc > i)
     boxsize=atof(argv[i++]);

 if(argc > i)
     nbubbles=(long)atoi(argv[i++]);
 if(nbubbles>npoints)
	Error("Bubble number must be less than point number\n",EXIT);

 bubblesize   =  (long *) calloc((size_t)nbubbles, sizeof(long));
 bubblecenter = (double *)calloc((size_t)nbubbles*MAXDIM, sizeof(double));
 bubbleradius = (double *)calloc((size_t)nbubbles, sizeof(double));

 if(!bubblecenter || !bubbleradius || !bubblesize)
				Error("Insufficient memory\n",EXIT);

 boxradius=boxsize/2;

 bubblemaxradius = boxradius / pow(2.0 * nbubbles,1/3) * 1.5 / 2.0;


 /* Define bubble centers and radii */

 if(nbubbles==1)		     /* If there is only one bubble */
  {				     /* it must fill the box.	    */
   for(k=0;k<Dim;k++)
     bubblecenter[k] = 0.0;
       
   bubbleradius[0]   = boxradius;
 }
 else 
   for(i=0;i<nbubbles;i++)
     {
       for(k=0;k<Dim;k++)
	 bubblecenter[i*MAXDIM+k] = 
	   ((double)rand()/(RAND_MAX/2.0) - 1.0)*boxradius;
       do
	 {
	   bubbleradius[i]=((double)rand()/(RAND_MAX))* bubblemaxradius;
	 }
       while(bubbleradius[i] < ((bubblemaxradius/nbubbles)/2));
       bubbleradius[i]*=6;
     }
 

 /* Defineo number of Sites per bubble */
 /* for RANDOM and VOLUME we need 2 pass */

 for(i=0;i<nbubbles;i++)
  {
  if(SitePerBubble==CONSTANT)
	bubblesize[i]=npoints*(i+1)/nbubbles - npoints*i/nbubbles;

  if(SitePerBubble==RANDOM)
    {
     bubblesize[i]=1+rand()%npoints;
     total+=bubblesize[i];
    }

  if(SitePerBubble==VOLUME)
    {
     bubblesize[i] = (long)(400.0 / 3.0* PI * pow(bubbleradius[i],3));
     total+=bubblesize[i];
    }
  }


 /* second pass */

 if(SitePerBubble==VOLUME || SitePerBubble==RANDOM)
   {
    for(i=0;i<nbubbles-1;i++)
       {
	
	bubblesize[i] = bubblesize[i]*npoints/total;
	total2+=bubblesize[i];
       }
    bubblesize[nbubbles-1]=npoints-total2;
   }
/*
 for(i=0;i<nbubbles;i++)
     printf("% bubblesize[%i] = %i %lf \n",i,bubblesize[i],bubbleradius[i] );
       */
 /* Finally we start to print! */
 /******************************************************************/
 if(Format == STD) printf("%i\n",npoints);
 if(Format == GEOM) 
   {
     printf("VECT\n%i %i 0\n",npoints,npoints);
     for(i=0;i<npoints;i++) printf("1 ");
     printf("\n");
     for(i=0;i<npoints;i++) printf("0 ");
   }
  if(Format == QHULL)  printf("3 %i\n",npoints);
  if(Format == DETRI)  printf("# %i\n# fix: 8.6",npoints);
 
 /******************************************************************/
 
 for(i=0;i<nbubbles;i++)
  {
   printf("\n");
   for(j=0;j<bubblesize[i];)
     {
       dist=0;
       for(k=0;k<Dim;k++)
         {
	   x=((double)random())/(RAND_MAX/2.0) - 1.0;
	   dist += x*x;
	   coord[k]=x;
	 }
       
       if(!UniformDistribution)
	 {
	   dist=pow(dist,.5);
	     for(k=0;k<Dim;k++)
	       coord[k]*=dist;
	 }
       dist=0;
       for(k=0;k<Dim;k++)
	 {
	   dist+=coord[k]*coord[k];
	   coord[k]*=bubbleradius[i];
	 }
       
       if(sqrt(dist)<=bubbleradius[i])
	 {
	   for(k=0;k<Dim;k++)
	     printf("%f ",coord[k]+ bubblecenter[i*MAXDIM+k] );
	   printf("\n");
	   j++;
	 }
 /*      else 
	 {printf("Wrong ! %lf ",sqrt(dist));
	   for(k=0;k<Dim;k++)
	     printf("%f ",coord[k]);
	   printf("\n");
	 }*/
     }
  }

 return 0;
}
