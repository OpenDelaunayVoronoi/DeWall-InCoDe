/***************************************************************************
******************************* 05/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:      main.c                                                     *
*                                                                          *
* PURPOSE:      An incremental costruction Delaunay 3d triangulator.       *
*                                                                          *
* IMPORTS:      OList                                                      *
*                                                                          *
* EXPORTS:      ProgramName to error.c                                     *
*               Statistic variables to unifgrid.c                          *
*                                                                          *
*   NOTES:      This is the optimized version of the InCoDe algorithm.     *
*               It use hashing and Uniform Grid techniques to speed up     *
*               list management and tetrahedra construction.               *
*                                                                          *
*               This program uses the 0.9 release of OList for list        *
*               management.                                                *
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

#include "graphics.h"
#include "incode.h"

#define USAGE_MESSAGE "\nUsage: incode [-s[1|2]] [-u nnn] [-p] [-c] [-f|-t] Filein [Fileout]\n\t\
 -s\tTurn on statistic informations \n\t\
 -s1\tTurn on only numerical statistical informations \n\t\
 -s2\tAdd a description line to numerical statistical informations\n\t\
 -u nnn\t Set Uniform Grid size = nnn cell\n\t\
 -p print the number of built tetrahedra while processing\n\t\
 -c\tCheck every tetrahedron is a Delaunay one\n\t\
 -f\tCheck for double creating Face (caused by numerical errors) \n\t\
 -t\tCheck for double creating Tetrahedra (caused by num. errors) \n\
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

boolean UGSizeFlag	= OFF;	/* Whether UG size is user defined	   */
int	UGSize		= 1;	/* The UG size user proposed		   */

boolean UpdateFlag	= OFF;	/* Whether printing the increasing number  */
				/* of builded tetrahedra while processing. */

boolean SafeFaceFlag	= OFF;	/* Whether Testing AFL inserted Face to not*/
				/* twice inserted. This situation happens  */
				/* for numerical errors and cause loop the */
				/* whole algorithm.			   */

boolean SafeTetraFlag	= OFF;	/* Analogous to SafeFaceFlag but check the */
				/* tetrahedra instead of faces (Faster!)   */

/***************************************************************************
*									   *
* CheckTetra								   *
*									   *
* Test that all the points are out of the sphere circumscribed to	   *
* tetrahedron t. This test need a O(n) time and is made only on explicit   *
* user request (-c parameter).						   *
*									   *
***************************************************************************/

void CheckTetra(Tetra *t, Point3 v[], int n)
{
 int i;
 Point3 Center;
 double Radius;
 double d;

 if(!CalcSphereCenter(&(v[t->f[0]->v[0]]),
		      &(v[t->f[0]->v[1]]),
		      &(v[t->f[0]->v[2]]),
		      &(v[t->f[1]->v[1]]), &Center ))
			  Error("CheckTetra, Bad Tetrahedron, CalcSphereCenter Failed!\n",EXIT);

 Radius = V3DistanceBetween2Points(&Center,&(v[t->f[0]->v[0]]));
 for(i=0;i<n;i++)
 {
   d=V3DistanceBetween2Points(&Center,&(v[i]));
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

Tetra *BuildTetra(Face *f, int p)
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

Tetra *MakeTetra(Face *f,Point3 *v,int n)
{
 Plane p,Mp;
 boolean found=FALSE;
 double Radius=BIGNUMBER,  rad;
 Line Lc;
 int pind=0;
 Tetra *t;
 Point3 Center, c;
 int i;

 if(!CalcPlane(&(v[f->v[0]]),&(v[f->v[1]]),&(v[f->v[2]]),&p))
		Error("MakeTetra, Face with collinar vertices!\n",EXIT);

 CalcLineofCenter(&(v[f->v[0]]),&(v[f->v[1]]),&(v[f->v[2]]),&Lc);

 for(i=0;i<n;i++)
 {
  if((i!=f->v[0]) &&
     (i!=f->v[1]) &&
     (i!=f->v[2]) &&
     RightSide(&p,&(v[i])) )
		{
		 CalcMiddlePlane(&(v[i]),&(v[f->v[0]]),&Mp);
		 if(CalcLinePlaneInter(&Lc,&Mp,&c))
		      {
			rad=V3SquaredDistanceBetween2Points(&c, &(v[i]));

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

 t=BuildTetra(f,pind);

 if(CheckFlag)  CheckTetra(t,v,n);

 return t;
}

/***************************************************************************
*									   *
* FirstTetra								   *
*									   *
* Build the first Delaunay tetrahedron of the triangulation.		   *
*									   *
***************************************************************************/

Tetra *FirstTetra(Point3 *v, int n)
{
 int i, MinIndex=0;
 double Radius, MinRadius=BIGNUMBER;

 Tetra *t;
 Plane p[3], Mp;
 Line Lc;
 Face f;
 Point3 c;

 boolean found=FALSE;

 f.v[0]=0;		/* The first point of the face is the 0 */
			/* (any point is equally good).		*/

 for(i=0;i<n;i++)	/* The 2nd point of the face is the	*/
  if(i!=f.v[0]) 	/* euclidean nearest to first point	*/
    {
     Radius=V3SquaredDistanceBetween2Points(&(v[0]), &(v[i]));
     if(Radius<MinRadius)
	{
	 MinRadius=Radius;
	 MinIndex=i;
	}
    }

 f.v[1]=MinIndex;
			/* The 3rd point is that with previous	*/
			/* ones builds the smallest circle.	*/

 CalcMiddlePlane(&(v[f.v[0]]),&(v[f.v[1]]), &(p[0]));

 MinRadius=BIGNUMBER;

 for(i=0;i<n;i++)
  if(i!=f.v[0] && i!=f.v[1])
   {
    CalcMiddlePlane(&(v[f.v[0]]), &(v[i]),&(p[1]));
    if(CalcPlane(&(v[f.v[0]]),&(v[f.v[1]]),&(v[i]),&(p[2])))
      if(CalcPlaneInter(&(p[0]), &(p[1]), &(p[2]), &c))
	{
	 Radius=V3DistanceBetween2Points(&c, &(v[0]));
	  if(Radius<MinRadius)
		{
		 MinRadius=Radius;
		 MinIndex=i;
		}
	}
   }

 f.v[2]=MinIndex;


 /* The first tetrahedron construction is analogous to normal */
 /* MakeTetra, only we dont limit search to an halfspace.     */

 MinRadius=BIGNUMBER;

 CalcPlane(&(v[f.v[0]]),&(v[f.v[1]]),&(v[f.v[2]]),&p[0]);
 CalcLineofCenter(&(v[f.v[0]]),&(v[f.v[1]]),&(v[f.v[2]]),&Lc);

 for(i=0;i<n;i++)
  if(i!=f.v[0] && i!=f.v[1] && i!=f.v[2] )
   {
    CalcMiddlePlane(&(v[i]),&(v[f.v[0]]),&Mp);
    if(CalcLinePlaneInter(&Lc,&Mp,&c))
	 {
	   Radius=V3SquaredDistanceBetween2Points(&c, &(v[i]));

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

 if(!RightSide(&p[0],&(v[MinIndex])))	ReverseFace(&f);

 t=BuildTetra(&f, MinIndex);

 CheckTetra(t,v,n);

 ReverseFace(t->f[0]); /* First Face in first Tetra   */
		       /* must be outward oriented    */
 return t;

}

/***************************************************************************
*									   *
* InCoDe								   *
*									   *
* Given a vector v of n Point3 this function returns the tetrahedra list   *
* of the Delaunay triangulation of points. This Functions uses the incre-  *
* mental construction algoritm InCoDe [Cignoni 92].			   *
*									   *
* We build the triangulation adding to an initial Delaunay tetrahedron new *
* Delaunay tetrahedra built from its faces (using MakeTetra function).	   *
*									   *
* This algorithm make use of Speed up techniques suggeste in the paper	   *
* yelding an average linear performance (against a teorethical cubic worst *
* case.									   *
*									   *
* [Cignoni 92]								   *
* P. Cignoni, C. Montani, R. Scopigno		  d			   *
*"A Merge-First Divide and Conquer Algorithm for E  Delaunay Triangulation"*
* CNUCE Internal Report C92/16 Oct 1992					   *
*									   *
***************************************************************************/

List InCoDe(Point3 *v, int n)
{
 List Q=NULL_LIST;
 List T=NULL_LIST;
 List OldFace=NULL_LIST;

 Tetra *t;
 ShortTetra *st;
 Face  *f;
 int i,j;
 UG g;
 
 Q=NewList(FIFO,sizeof(Face));			/* Initialize Active Face */
 ChangeEqualObjectList(EqualFace,Q);		/* List Q.		  */
 HashList(n/4,HashFace,Q);

 if(SafeFaceFlag)
   {						/* Initialize list for	  */
    OldFace=NewList(FIFO,sizeof(Face)); 	/* preventing double face */
    ChangeEqualObjectList(EqualFace,OldFace);	/* looping on numerical   */
    HashList(n/4,HashFace,OldFace);		/* errors		  */
   }

 T=NewList(FIFO,sizeof(ShortTetra));		/* Initialize Built Tetra-*/
 ChangeEqualObjectList(EqualTetra,T);		/* hedra List T.	  */
 if(SafeTetraFlag) HashList(n/4,HashTetra,T);


 if(UGSizeFlag) BuildUG(v,n,UGSize,&g); 	/* Initialize Uniform Grid */
	   else BuildUG(v,n,n	  ,&g);

 t=FirstTetra(v,n);

 for(i=0;i<4;i++)
  {
   InsertList(t->f[i],Q);
   if(SafeFaceFlag)  InsertList(t->f[i],OldFace);
   for(j=0;j<3;j++)
     if(g.UsedPoint[t->f[i]->v[j]]==-1) 
       g.UsedPoint[t->f[i]->v[j]]=1;
     else 
       g.UsedPoint[t->f[i]->v[j]]++; 
 }

 SI.Face=4;

 st=Tetra2ShortTetra(t);

 free(t);

 InsertList(st,T);

 while(ExtractList(&f,Q))
   {
     t=FastMakeTetra(f,v,n,&g);
     
     if(t==NULL) SI.CHFace++;
     else
       {
	 st=Tetra2ShortTetra(t);
	 
	 if(SafeTetraFlag) if(MemberList(st, T))
	   Error("Cyclic Tetrahedra Creation\n",EXIT);
	 InsertList(st,T);
	 
	 if(UpdateFlag)
	   if(++SI.Tetra%50 == 0) printf("Tetrahedra Built %i\r",SI.Tetra++);
	 
	 for(i=1;i<4;i++)
	   if(MemberList(t->f[i],Q))
	     {
	       DeleteCurrList(Q);
	       free(t->f[i]);
	       
	       for(j=0;j<3;j++)
		 g.UsedPoint[t->f[i]->v[j]]--;
	     }
	   else
	     {
	       InsertList(t->f[i],Q);
	       SI.Face++;
	       
	       for(j=0;j<3;j++)
		 if(g.UsedPoint[t->f[i]->v[j]]==-1) 
		   g.UsedPoint[t->f[i]->v[j]]=1;
		 else g.UsedPoint[t->f[i]->v[j]]++;
		   
	       if(SafeFaceFlag)
		 {
		   if(MemberList(t->f[i], OldFace))
		     Error("Cyclic insertion in Active Face List\n",EXIT);
		   InsertList(t->f[i],OldFace);
		 }
	     }
	 free(t->f[0]);
	 free(t);
       }
     for(i=0;i<3;i++)                   
       {
	 g.UsedPoint[f->v[i]]--;
	/*  if(g.UsedPoint[f->v[i]]==0) printf("Point %i completed\n",f->v[i]);*/
       }
     if(!SafeFaceFlag) free(f);
   }
 

 return T;
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
 Point3 *v;
 List   T=NULL_LIST;
 int n,i=1;
 FILE *fp=stdout;
 double sec;

 SetProgramName("InCoDe");

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
       case 'f' : SafeFaceFlag=ON;			break;
       case 't' : SafeTetraFlag=ON;			break;
       case 's' : StatFlag=ON;
		  if(argv[i][2]=='1') NumStatFlag=ON;
		  if(argv[i][2]=='2')
				{
				 NumStatTitleFlag=ON;
				 NumStatFlag=ON;
				}
		  break;

       case 'u' : UGSizeFlag=ON;
		  if(argv[i][2]==0 && isdigit(argv[i+1][0]))
			 UGSize=atoi(argv[++i]);
		    else UGSize=atoi(argv[i]+2);
		  break;

       default	: sprintf(buf,"Unknown options '%s'\n",argv[i]);
		  Error(buf,NO_EXIT);
      }
    i++;
    }

 v=ReadPoints(argv[i++],&n);

 if(argc>i) fp=fopen(argv[i],"w");

 SI.Point=n;

 ResetChronos(USER_CHRONOS);
 StartChronos(USER_CHRONOS);
 T=InCoDe(v,n);
 StopChronos(USER_CHRONOS);

 SI.Tetra=CountList(T);
 
 sec=ReadChronos(USER_CHRONOS);

 SI.Secs=sec;

 if(StatFlag && !NumStatFlag) PrintStat();
 if(StatFlag && NumStatFlag && NumStatTitleFlag) PrintNumStatTitle();
 if(StatFlag && NumStatFlag) PrintNumStat();
 if(!StatFlag)
     printf("Points:%7i Secs:%6.2f Tetras:%7i\n",SI.Point,SI.Secs,SI.Tetra);

 WriteTetraList(T,fp);

 return 0;
}
