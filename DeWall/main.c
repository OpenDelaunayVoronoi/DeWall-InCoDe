/***************************************************************************
******************************* 17/Jul/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:      main.c                                                     *
*                                                                          *
* PURPOSE:	A Merge-First Divide and Conquer Delaunay 3d triangulator. *
*                                                                          *
* IMPORTS:      OList                                                      *
*                                                                          *
* EXPORTS:      ProgramName to error.c                                     *
*               Statistic variables to unifgrid.c                          *
*                                                                          *
*   NOTES:	This is the optimized version of the DeWall algorithm.	   *
*               It use hashing and Uniform Grid techniques to speed up     *
*               list management and tetrahedra construction.               *
*                                                                          *
*               This program uses the 0.9 release of OList for list        *
*               management.                                                *
*                                                                          *
*	17/Jul/93 	Added a cast to 				   *
*			int (*compar)(const void *, const void *) 	   *
*			on the qsort() call.				   *	
*                                                                          *
****************************************************************************
***************************************************************************/

#include <OList/general.h>
#include <OList/error.h>
#include <OList/olist.h>
#include <OList/chronos.h>

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/times.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>



#include "graphics.h"
#include "dewall.h"

#define USAGE_MESSAGE "\nUsage: dewall [-s[1|2]] [-u nnn] [-p] [-c] [-t] filein [fileout]\n\
	-s	Turn on statistic informations (descriptive format)\n\
	-s1	Turn on statistic informations (numerical only format)\n\
	-s2	Turn on statistic informations \n\
		(numerical+descriptive line format)\n\
	-u <x>	Set Uniform Grid scale ug size of <x>)\n\
	-p	print the number of tetrahedra built while processing\n\
	-c	Check every tetrahedron is a Delaunay one\n\
	-t	Check for double creating Tetrahedra (caused by num. errors)\n\
\n\
	filein	file of points to be triangulated\n\
	fileout triangulation output file\n\
"

/***************************************************************************
*									   *
* Global Variables:							   *
*									   *
*     -	Statistic Informations						   *
*     - Program Name (exported in error.c)				   *
*     -	Program Flags							   *
*									   *
***************************************************************************/


StatInfo SI;			/* Statistic Infomations */


				/************** Program Flags **************/

boolean CheckFlag	= OFF;	/* Whether checking each built tetrahedron */
				/* is a Delaunay one.			   */

boolean StatFlag	= OFF;	/* Whether printing Statistic Infomations. */

boolean NumStatFlag	= OFF;	/* Whether printing only numerical values  */
				/* of Statistic Infomations.		   */

boolean NumStatTitleFlag= OFF;	/* Whether add title to previuos Statistics*/

boolean UGScaleFlag	= OFF;	/* Whether UG size is user defined	   */
float	UGScale		= 1;	/* The UG size user proposed		   */

boolean UpdateFlag	= OFF;	/* Whether printing the increasing number  */
				/* of builded tetrahedra while processing. */

boolean SafeTetraFlag	= OFF;	/* Whether checking each new tetra is a    */
				/* real new tetra and it is not twice	   */
				/* twice inserted. This situation happens  */
				/* for numerical errors and cause loop the */
				/* whole algorithm.			   */


/***************************************************************************
*									   *
* CheckTetra								   *
*									   *
* Test that all the points are out of the sphere circumscribed to	   *
* tetrahedron t. This test need a O(n) time and is made only on explicit   *
* user request (-c parameter).						   *
*									   *
***************************************************************************/

void CheckTetra(Tetra *t, Point3 *v[], int n)
{
 int i;
 Point3 Center;
 double Radius;
 double d;

 if(!CalcSphereCenter(t->f[0]->v[0],
		      t->f[0]->v[1],
		      t->f[0]->v[2],
		      t->f[1]->v[1], &Center ))
			  Error("CheckTetra, Bad Tetrahedron, CalcSphereCenter Failed!\n",EXIT);

 Radius = V3DistanceBetween2Points(&Center, t->f[0]->v[0]);
 for(i=0;i<n;i++)
 {
   d=V3DistanceBetween2Points(&Center,v[i]);
   if( d < Radius - EPSILON)
     Error("CheckTetra, A not Delaunay Tetrahedron was built!\n",EXIT);
 }
}




/***************************************************************************
*									   *
* BuildTetra								   *
*									   *
* Given a face and a point return the tetrahedron built joining face to    *
* point. All the face (except the first) are outward oriented.		   *
*									   *
***************************************************************************/

Tetra *BuildTetra(Face *f, Point3 *p)
{
 Tetra *t;
 Face *f0, *f1,*f2,*f3;

 t = (Tetra *)malloc(sizeof(Tetra));
 f0 =(Face *)malloc(sizeof(Face));
 f1 =(Face *) malloc(sizeof(Face));
 f2 =(Face *) malloc(sizeof(Face));
 f3 =(Face *) malloc(sizeof(Face));

 if(!f0 || !f1 || !f2 || !f3 || !t)
    Error("BuildTetra, Not enough memory for a new tetrahedron\n",EXIT);

 t->f[0]=f0;
 t->f[1]=f1;
 t->f[2]=f2;
 t->f[3]=f3;

 f0->v[0]=f->v[0];
 f0->v[1]=f->v[1];
 f0->v[2]=f->v[2];

 f1->v[0]=f->v[0];
 f1->v[1]=p;
 f1->v[2]=f->v[2];

 f2->v[0]=f->v[2];
 f2->v[1]=p;
 f2->v[2]=f->v[1];

 f3->v[0]=f->v[1];
 f3->v[1]=p;
 f3->v[2]=f->v[0];

 return t;
}


/***************************************************************************
*									   *
* MakeTetra								   *
*									   *
* Given a face find the dd-nearest point to it and joining it to the face  *
* build a new Delaunay tetrahedron.					   *
*									   *
***************************************************************************/

Tetra *MakeTetra(Face *f,Point3 *v[], int n)
{
 Plane p,Mp;
 boolean found=FALSE;
 double Radius=BIGNUMBER,  rad;
 Line Lc;
 int pind=0;
 Tetra *t;
 Point3 Center, c;
 int i;

 if(!CalcPlane(f->v[0],f->v[1],f->v[2],&p))
		Error("MakeTetra, Face with collinar vertices!\n",EXIT);

 CalcLineofCenter(f->v[0],f->v[1],f->v[2],&Lc);

 for(i=0;i<n;i++)
 {
  if((v[i]!=f->v[0]) &&
     (v[i]!=f->v[1]) &&
     (v[i]!=f->v[2]) &&
     RightSide(&p,v[i]) )
		{
		 CalcMiddlePlane(v[i],f->v[0],&Mp);
		 if(CalcLinePlaneInter(&Lc,&Mp,&c))
		      {
			rad=V3SquaredDistanceBetween2Points(&c, v[i]);

			if(!RightSide(&p,&c)) rad=-rad;
			if(rad==Radius) Error("MakeTetra, Five cocircular points!\n",EXIT);
			if(rad<Radius)
				{
				 found=TRUE;
				 Radius=rad;
				 Center=c;
				 pind=i;
				}
		      }
		}
 }

 if(!found) return NULL;

 t=BuildTetra(f,v[pind]);

 if(CheckFlag)	CheckTetra(t,v,n);

 if(StatFlag)
 {Point3 C;
  CalcSphereCenter(f->v[0], f->v[1], f->v[2], v[pind], &C);
  SI.Radius+=V3DistanceBetween2Points(&C, v[pind]);
 }

 return t;
}

/***************************************************************************
*									   *
* FirstTetra								   *
*									   *
* Build the first Delaunay tetrahedron of the triangulation.		   *
*									   *
***************************************************************************/

Tetra *FirstTetra(Point3 *v[], int n)
{
 int i, MinIndex=0;
 double Radius, MinRadius=BIGNUMBER;

 Tetra *t;
 Plane p[3], Mp;
 Line Lc;
 Face f;
 Point3 c;

 boolean found=FALSE;

 f.v[0]=v[n/2-1];	/* The first point of the face is the	*/
			/* nearest to middle plane in negative	*/
			/* halfspace.				*/

			/* The 2nd point of the face is the	*/
			/* euclidean nearest to first point	*/
 for(i=n/2;i<n;i++)	/* that is in the positive halfspace	*/
    {
     Radius=V3SquaredDistanceBetween2Points(f.v[0], v[i]);
     if(Radius<MinRadius)
	{
	 MinRadius=Radius;
	 MinIndex=i;
	}
    }

 f.v[1]=v[MinIndex];
			/* The 3rd point is that with previous	*/
			/* ones builds the smallest circle.	*/

 CalcMiddlePlane(f.v[0],f.v[1], &(p[0]));

 MinRadius=BIGNUMBER;

 for(i=0;i<n;i++)
  if(v[i]!=f.v[0] && v[i]!=f.v[1])
   {
    CalcMiddlePlane(f.v[0], v[i],&(p[1]));
    if(CalcPlane(f.v[0],f.v[1],v[i],&(p[2])))
      if(CalcPlaneInter(&(p[0]), &(p[1]), &(p[2]), &c))
	{
	 Radius=V3DistanceBetween2Points(&c, f.v[0]);
	  if(Radius<MinRadius)
		{
		 MinRadius=Radius;
		 MinIndex=i;
		}
	}
   }

 f.v[2]=v[MinIndex];


 /* The first tetrahedron construction is analogous to normal */
 /* MakeTetra, only we dont limit search to an halfspace.     */

 MinRadius=BIGNUMBER;

 CalcPlane(f.v[0], f.v[1], f.v[2],&p[0]);
 CalcLineofCenter(f.v[0], f.v[1], f.v[2], &Lc);

 for(i=0;i<n;i++)
  if(v[i]!=f.v[0] && v[i]!=f.v[1] && v[i]!=f.v[2] )
   {
    CalcMiddlePlane(v[i], f.v[0], &Mp);
    if(CalcLinePlaneInter(&Lc, &Mp, &c))
	 {
	   Radius=V3SquaredDistanceBetween2Points(&c, v[i]);

	   if(MinRadius==Radius) Error("FirstTetra, Five cocircular points!\n",EXIT);
	   if(Radius<MinRadius)
		   {
		    found=TRUE;
		    MinRadius=Radius;
		    MinIndex=i;
		   }
	 }
   }

 if(!found) Error("FirstTetra, Planar dataset, unable to build first tetrahedron.\n",EXIT);

 if(!RightSide(&p[0],v[MinIndex]))	ReverseFace(&f);

 t=BuildTetra(&f, v[MinIndex]);

 CheckTetra(t,v,n);

 ReverseFace(t->f[0]); /* First Face in first Tetra   */
		       /* must be outward oriented    */
 return t;

}

/***************************************************************************
*									   *
*  XComp, YComp, ZComp							   *
*									   *
*  Compare functions for Qsorting the Point3 vector.			   *
*									   *
***************************************************************************/

int XComp(void *E1, void *E2)
{
  Point3 **e1=(Point3 **)E1,
	 **e2=(Point3 **)E2;

  if((*e1)->x > (*e2)->x)      return  1;
  else if((*e1)->x < (*e2)->x) return -1;
  else return 0;
}

int YComp(void *E1, void *E2)
{
  Point3 **e1=(Point3 **)E1,
	 **e2=(Point3 **)E2;

  if((*e1)->y > (*e2)->y)      return  1;
  else if((*e1)->y < (*e2)->y) return -1;
  else return 0;
}

int ZComp(void *E1, void *E2)
{
  Point3 **e1=(Point3 **)E1,
	 **e2=(Point3 **)E2;

  if((*e1)->z > (*e2)->z)      return  1;
  else if((*e1)->z < (*e2)->z) return -1;
  else return 0;
}


/***************************************************************************
*									   *
* DeWall								   *
*									   *
* Given a vector v of n Point3 this function returns the tetrahedra list   *
* of the Delaunay triangulation of points. This Functions uses the	   *
* MergeFirst Divide and Conquer algorithm DeWall [Cignoni 92].		   *
*									   *
* This algorithm make use of Speed up techniques suggested in the paper	   *
* yelding an average linear performance (against a teorethical cubic worst *
* case.									   *
*									   *
* [Cignoni 92]								   *
* P. Cignoni, C. Montani, R. Scopigno		  d			   *
*"A Merge-First Divide and Conquer Algorithm for E  Delaunay Triangulation"*
* CNUCE Internal Report C92/16 Oct 1992					   *
*									   *
***************************************************************************/

void DeWall(Point3 *v[], Point3 *BaseV, int *UsedPoint, int n, List Q, List T, enum Axis a)
{
 List Ln=NULL_LIST,
      La=NULL_LIST,
      Lp=NULL_LIST;

 Tetra *t;
 ShortTetra *st;
 Face  *f;
 int i,j;
 UG g;
 Plane alpha;

 alpha.N.x=0;
 alpha.N.y=0;
 alpha.N.z=0;
 alpha.off=0;

 if(n>20)
 if(UGScaleFlag) BuildUG(v,UsedPoint,n, (int)(n*UGScale),&g); 	/* Initialize Uniform Grid */
	   else BuildUG(v,UsedPoint,n,      n,&g);

 Ln=NewList(FIFO,sizeof(Face));			/* Initialize Active Face */
 ChangeEqualObjectList(EqualFace,Ln);		/* List Ln.		  */
 if(n>40) HashList(n/4,HashFace,Ln);

 La=NewList(FIFO,sizeof(Face));			/* Initialize Active Face */
 ChangeEqualObjectList(EqualFace,La);		/* List La.		  */
 if(n>40) HashList(n/4,HashFace,La);

 Lp=NewList(FIFO,sizeof(Face));			/* Initialize Active Face */
 ChangeEqualObjectList(EqualFace,Lp);		/* List Lp.		  */
 if(n>40) HashList(n/4,HashFace,Lp);


 switch(a)
  {
   case XAxis :	qsort((void *)v, (size_t)n, sizeof(Point3 *),
				 (int (*)(const void *,const void *))XComp);
		alpha.N.x = 1;
		alpha.off = (v[n/2-1]->x+v[n/2]->x)/2;
		break;

   case YAxis : qsort((void *)v, (size_t)n, sizeof(Point3 *), 
				 (int (*)(const void *,const void *))YComp);
		alpha.N.y = 1;
		alpha.off = (v[n/2-1]->y+v[n/2]->y)/2;
		break;

   case ZAxis : qsort((void *)v, (size_t)n, sizeof(Point3 *),
				 (int (*)(const void *,const void *))ZComp);
		alpha.N.z = 1;
		alpha.off = (v[n/2-1]->z+v[n/2]->z)/2;
		break;
  }

 if(CountList(Q)==0)
 {
  t=FirstTetra(v,n);

  for(i=0;i<4;i++)
    {
      switch (Intersect(t->f[i],&alpha))
	{
	case  0 :     InsertList(t->f[i], La);	 break;
	case  1 :     InsertList(t->f[i], Lp);	 break;
	case -1 :     InsertList(t->f[i], Ln);	 break;
	}
      for(j=0;j<3;j++)
	if(t->f[i]->v[j]->mark==-1)  t->f[i]->v[j]->mark=1;
	else t->f[i]->v[j]->mark++;
    }
  st=Tetra2ShortTetra(t,BaseV);
  free(t);
  InsertList(st,T);
  SI.Face+=4;
 }
 else
 {
  while(ExtractList(&f,Q))
    switch (Intersect(f,&alpha))
    {
      case  0 :     InsertList(f, La);	 break;
      case  1 :     InsertList(f, Lp);	 break;
      case -1 :     InsertList(f, Ln);	 break;
    }
 }

 while(ExtractList(&f,La))
 {
  if(n>20) t=FastMakeTetra(f,v,n,&g);
      else t=MakeTetra(f,v,n);
  if(t==NULL) SI.CHFace++;
  else
     {
      st=Tetra2ShortTetra(t,BaseV);

      if(SafeTetraFlag) if(MemberList(st, T))
			Error("Cyclic Tetrahedra Creation\n",EXIT);
      InsertList(st,T);

      SI.Face+=3;
      SI.Tetra++;

      if(UpdateFlag)
	 if(SI.Tetra%50 == 0) printf("Tetrahedra Built %i\r",SI.Tetra++);

      for(i=1;i<4;i++)
	 switch (Intersect(t->f[i],&alpha))
	  {
	   case	0 :	if(MemberList(t->f[i],La))
			  {
			   DeleteCurrList(La);
			   
			   SI.Face--;
			   for(j=0;j<3;j++)
			     t->f[i]->v[j]->mark--;
			   free(t->f[i]);
			  }
			else 
			  {
			    InsertList(t->f[i],La);
			    for(j=0;j<3;j++)
			      if(t->f[i]->v[j]->mark==-1)  t->f[i]->v[j]->mark=1;
			      else t->f[i]->v[j]->mark++;
			    
			  }
	     break;
	   case 1 :	if(MemberList(t->f[i],Lp))
			  {
			    DeleteCurrList(Lp);
			  
			    SI.Face--;
			    for(j=0;j<3;j++)
			      t->f[i]->v[j]->mark--;
			    free(t->f[i]);
			  }
	                else 
			  {
			    InsertList(t->f[i],Lp);
			    for(j=0;j<3;j++)
			      if(t->f[i]->v[j]->mark==-1)  t->f[i]->v[j]->mark=1;
			      else t->f[i]->v[j]->mark++;
			    
			  }
	     break;
	   case -1:	if(MemberList(t->f[i],Ln))
			  {
			    DeleteCurrList(Ln);
	
			    SI.Face--;
			    for(j=0;j<3;j++)
			      t->f[i]->v[j]->mark--;		
			    free(t->f[i]);

			  }
			else 
			  {
			    InsertList(t->f[i],Ln);
			    for(j=0;j<3;j++)
			      if(t->f[i]->v[j]->mark==-1)  t->f[i]->v[j]->mark=1;
			      else t->f[i]->v[j]->mark++;
			    
			  }
			break;
	  }

      free(t->f[0]);
      free(t);
     }
  free(f);
 }
 if(SI.WallSize==0) SI.WallSize=SI.Tetra;
 /* if(n>20) EraseUG(&g); */ 

 switch(a)
  {
   case XAxis : if(CountList(Ln)>0) DeWall(v,	     BaseV,UsedPoint,n/2,    Ln,T,YAxis);
		if(CountList(Lp)>0) DeWall(&(v[n/2]),BaseV,UsedPoint,n-(n/2),Lp,T,YAxis);
		break;
   case YAxis : if(CountList(Ln)>0) DeWall(v,	     BaseV,UsedPoint,n/2,    Ln,T,ZAxis);
		if(CountList(Lp)>0) DeWall(&(v[n/2]),BaseV,UsedPoint,n-(n/2),Lp,T,ZAxis);
		break;
   case ZAxis : if(CountList(Ln)>0) DeWall(v,	     BaseV,UsedPoint,n/2,    Ln,T,XAxis);
		if(CountList(Lp)>0) DeWall(&(v[n/2]),BaseV,UsedPoint,n-(n/2),Lp,T,XAxis);
		break;
  }

 EraseList(Ln);
 EraseList(La);
 EraseList(Lp);
}

/***************************************************************************
*									   *
* main									   *
*									   *
* Do the usual command line parsing, file I/O and timing matters.	   *
*									   *
***************************************************************************/

main(int argc, char *argv[])
{
 char buf[80];
 Point3 **v;
 Point3 *BaseV;
 int *usedpoint;
 List	T=NULL_LIST,
	Q=NULL_LIST;
 int n,i=1;
 FILE *fp=stdout;
 double sec;
 SetProgramName("DeWall");
 if((argc<2) ||
    (strcmp(argv[i],"/?")==0)||
    (strcmp(argv[i],"-?")==0)||
    (strcmp(argv[i],"/h")==0)||
    (strcmp(argv[i],"-h")==0)  ) Error(USAGE_MESSAGE, EXIT);

  while(*argv[i]=='-')
   {
    switch(argv[i][1])
      {
       case 'p' : UpdateFlag=ON;			break;
       case 'c' : CheckFlag=ON; 			break;
       case 't' : SafeTetraFlag=ON;			break;
       case 's' : StatFlag=ON;
		  if(argv[i][2]=='1') NumStatFlag=ON;
		  if(argv[i][2]=='2')
				{
				 NumStatTitleFlag=ON;
				 NumStatFlag=ON;
				}
		  break;

       case 'u' : UGScaleFlag=ON;
		  if(argv[i][2]==0 && isdigit(argv[i+1][0]))
			 UGScale=atof(argv[++i]);
		    else UGScale=atof(argv[i]+2);
	          break;

       default	: sprintf(buf,"Unknown options '%s'\n",argv[i]);
		  Error(buf,NO_EXIT);
      }
    i++;
    }

 BaseV=ReadPoints(argv[i++],&n);

 if(argc>i) fp=fopen(argv[i],"w");

 SI.Point=n;

 v=(Point3 **)malloc(n*sizeof(Point3 *));
 usedpoint=(int *)malloc(n*sizeof(int));

 if(!v || !usedpoint) Error("Unable to allocate memory for Points\n",EXIT);
 for(i=0;i<n;i++) v[i]=&(BaseV[i]);
 for(i=0;i<n;i++)  usedpoint[i] = -1;

 Q=NewList(FIFO,sizeof(Face));			/* Initialize First Face  */
 ChangeEqualObjectList(EqualFace,Q);		/* List Q.		  */
 if(n>40) HashList(n/4,HashFace,Q);

 T=NewList(LIFO,sizeof(ShortTetra));		/* Initialize Built Tetra-*/
 ChangeEqualObjectList(EqualTetra,T);		/* hedra List T.	  */
 if(SafeTetraFlag && n>40) HashList(n/4,HashTetra,T);

 ResetChronos(USER_CHRONOS);
 StartChronos(USER_CHRONOS);

 DeWall(v,BaseV,usedpoint,n,Q,T,XAxis);

 StopChronos(USER_CHRONOS);
 sec=ReadChronos(USER_CHRONOS);

 SI.Tetra=CountList(T);
 

 SI.Secs=sec;

 if(StatFlag && !NumStatFlag) PrintStat();
 if(StatFlag && NumStatFlag && NumStatTitleFlag) PrintNumStatTitle();
 if(StatFlag && NumStatFlag) PrintNumStat();
 if(!StatFlag)
     printf("Points:%7i Secs:%6.2f Tetras:%7i\n",SI.Point,SI.Secs,SI.Tetra);

 WriteTetraList(T,fp);

 return 0;
}
