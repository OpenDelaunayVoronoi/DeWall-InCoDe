/***************************************************************************
******************************* 05/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:      unifgrid.c                                                 *
*                                                                          *
* PURPOSE:      Functions for Uniform Grid speed up technique.             *
*                                                                          *
* IMPORTS:      OList                                                      *
*               Statistic Variables                                        *
*                                                                          *
* EXPORTS:	BuildUG       Initialize the UG data structure		   *
*               FastMakeTetra   Build a new tetra using UG to speed up     *
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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"

#include <OList/general.h>
#include <OList/error.h>
#include <OList/olist.h>

#include "incode.h"


/* Global for Statistic Infomations */

extern StatInfo SI;

/* Global Program Flag */

extern boolean CheckFlag;
extern boolean StatFlag;

/***************************************************************************
*									   *
* AddPoint								   *
*									   *
* Each UG cell has a list of points it contains.			   *
* This function is used on UG setup when we fill each cell with its points *
*									   *
* This function add a points to list of points.				   *
*									   *
***************************************************************************/

Plist *AddPoint(Plist *P, int p)
{
 Plist *TempP;

 TempP=(Plist *)malloc(sizeof(Plist));
 if(!TempP)
   Error("Insufficient memory to add a point to point list of the UG cell!!\n", EXIT);

 TempP->p=p;
 TempP->next=P;

 return TempP;
}

/***************************************************************************
*									   *
* UGMark								   *
* UGIsMarked								   *
* UGResetMark								   *
*									   *
* These functions mark and test for marking cells of a Uniform Grid.	   *
* The function ResetMark unmark all the cells.				   *
*									   *
***************************************************************************/

void UGMark(UG *G, int Index)
{
 G->Marked[Index]=G->Mark;
 SI.TestedCell++;
}

void UGResetMark(UG *G)
{
 G->Mark++;
}

boolean UGIsMarked(UG *G, int Index)
{
 if(G->Marked[Index]==G->Mark) return TRUE;
			  else return FALSE;
}


/***************************************************************************
*									   *
* BuildUG								   *
*									   *
* Given a vector v of n points this functions setup a Uniform Grid G of m  *
* (or a greater number) cells.						   *
*									   *
* First find the bounding box (BB) of the point set (G->vn and G->pn are   *
* the min and max vertices of this box).				   *
* Calculate the volume of BB.						   *
* The volume of a cell is:  volumeBB/m					   *
*									   *
* Each cell is cubic an its edge is:					   *
*			    __________					   *
*			 3 /						   *
*		 edge =   / CellVolume					   *
*			\/						   *
*									   *
*									   *
* Then we count cell numer in each dimension of the BB (G->x, G->y...).    *
*									   *
* Because cell are cubic there is some waste of space because the UG	   *
* go beyond the BB							   *
*									   *
* Example: n=8, m=6							   *
*									   *
*      *		    Initial Dataset				   *
*	   *	 *							   *
*									   *
*     *									   *
*		    *							   *
*	   *	*							   *
*	      *								   *
*									   *
*									   *
*     +*------------|	   The Bounding Box has volume 16x7=112		   *
*     |	   *	 *  |					   _		   *
*     |		    |	   each cell has volume 112/6= 18.66		   *
*     *		    |							   *
*     |		    *	   each cell has edge squareroot(18.66)=4.32	   *
*     |	   *	*   |							   *
*     +-------*-----|							   *
*									   *
*     +*--+---+---+---+	   In each side of BB are			   *
*     |	  | * |	 *|   |							   *
*     |	  |   |	  |   |	   ceil(16/4.32)= 4 cells			   *
*     *---+---+---+---+	   ceil(7/4.32)= 2 cells			   *
*     |	  |   |	  | * |							   *
*     |	  |*  |	* |   |	   so the Ug is at last composed of 2*4 = 8 cells. *
*     +---+---*---+---+							   *
*									   *
*    +-*-+---+---+---+	   at last we move the vertices of UG to adapt	   *
*    |	 |  *|	 *   |	   it to the points.				   *
*    |	 |   |	 |   |							   *
*    +*--+---+---+---+							   *
*    |	 |   |	 |  *|							   *
*    |	 | * |	*|   |							   *
*    +---+---+*--+---+							   *
*									   *
*									   *
*									   *
***************************************************************************/

UG *BuildUG(Point3 *v, int n, int m, UG *G)
{
 int i;
 int indx, indy, indz, index;
 int CellNumber;
 double volume;
 double xoffset,yoffset,zoffset;

 G->vn.x=v[0].x;
 G->vn.y=v[0].y;
 G->vn.z=v[0].z;

 G->vp.x=v[0].x;
 G->vp.y=v[0].y;
 G->vp.z=v[0].z;

 for(i=0;i<n;i++)     /* Find the Bounding Box */
 {
  if(v[i].x < G->vn.x) G->vn.x=v[i].x;
  if(v[i].y < G->vn.y) G->vn.y=v[i].y;
  if(v[i].z < G->vn.z) G->vn.z=v[i].z;

  if(v[i].x > G->vp.x) G->vp.x=v[i].x;
  if(v[i].y > G->vp.y) G->vp.y=v[i].y;
  if(v[i].z > G->vp.z) G->vp.z=v[i].z;
 }

 volume=(G->vp.x - G->vn.x)*(G->vp.y - G->vn.y)*(G->vp.z - G->vn.z);
 G->side = pow(volume/(double)m,0.3333333);
 SI.CellEdge=G->side;

 G->x=(int)ceil((G->vp.x - G->vn.x)/G->side);  /* How many cell per side */
 G->y=(int)ceil((G->vp.y - G->vn.y)/G->side);
 G->z=(int)ceil((G->vp.z - G->vn.z)/G->side);

 xoffset = G->x*G->side - (G->vp.x-G->vn.x);
 yoffset = G->y*G->side - (G->vp.y-G->vn.y);
 zoffset = G->z*G->side - (G->vp.z-G->vn.z);


 G->vn.x -= xoffset/2;
 G->vn.y -= yoffset/2;
 G->vn.z -= zoffset/2;

 G->vp.x += xoffset/2;
 G->vp.y += yoffset/2;
 G->vp.z += zoffset/2;


 CellNumber=G->x*G->y*G->z;
 G->n=CellNumber;

 SI.Cell=CellNumber;

 G->C  =  (Plist **)calloc((size_t)CellNumber, sizeof(Plist *));
 G->Marked = (int *)calloc((size_t)CellNumber, sizeof(int));
 if(!G->C || !G->Marked) Error("BuildUG, Not enough memory to build UG!!\n",EXIT);

 for(i=0;i<n;i++)
 {
  indx=(int)((v[i].x - G->vn.x)/G->side);
  indy=(int)((v[i].y - G->vn.y)/G->side);
  indz=(int)((v[i].z - G->vn.z)/G->side);
  index=indx + indy*G->x + indz*G->y*G->x;
  G->C[index] = AddPoint((G->C)[index], i);
 }

 G->Mark=0;

 G->UsedPoint = (int *)calloc((size_t)n, sizeof(int));
 if(!G->UsedPoint) Error("BuildUG, Not enough memory to build UG!!\n",EXIT);
 for(i=0;i<n;i++) G->UsedPoint[i]=-1;

 if(StatFlag)	  /* Calculate Statistical Information only if needed */
 {
  int c;
  SI.EmptyCell=0;

  for(i=0;i<CellNumber;i++)
   {
    c=0;
    if(!G->C[i]) SI.EmptyCell++;	/* How many cell are empty?  */
    else
     {
      Plist *P=G->C[i];
      while(P)
       {
	c++;
	P=P->next;
       }
      if(c>SI.MaxPointPerCell)		/* What is the max number of */
		SI.MaxPointPerCell=c;	/* points per cell?	     */
     }
   }
 }
 return G;
}



/***************************************************************************
*									   *
* CalcBox								   *
*									   *
* Given a Face and the line containing centers of sphere circumscribing	   *
* the face this function calculates the cell Box in the UG that contain    *
* a sphere of given radius and passing for face vertices.		   *
*									   *
* The IntPoint vn and vp describe the box calculated. This function returns*
* the (squared) maximum dd distance for 'fair' point.			   *
*		      .....						   *
*		  ....	   ....						   *
*		..	       ..					   *
*	       .		 .					   *
*	      .		|	  .					   *
*	     .		|	   .					   *
*	     .		|	   .					   *
*	    .	      \	|	    .					   *
*	    .	     /	o  -	    .					   *
*	    .Radius /  /|  |	    .					   *
*	    .	   /  / |  |	    .					   *
*	     .	  /  /	|  | offset.					   *
*	     .	 /  /	|  |	   .					   *
*	      .	/  /	|  |	  .					   *
*	      .\  /	|  |	 .					   *
*	       . /	|  -	.					   *
*		----------------					   *
*		    ....|....						   *
*		|-------|						   *
*		   Face	|						   *
*		  Radius|		   2	    2		 2	   *
*			|	     offset = Radius - FaceRadius	   *
*		Line of>|						   *
*		Centers	|						   *
*									   *
***************************************************************************/

double CalcBox(Face *f, Point3 *v, Line *Lc, UG *G,
				IntPoint3 *vn, IntPoint3 *vp, double Radius)
{
 Point3 BoxCenter;
 double FaceRadius;
 double offset;

 FaceRadius=V3DistanceBetween2Points(&(Lc->Lu), &(v[f->v[0]]));

 offset=sqrt(Radius*Radius - FaceRadius*FaceRadius);

 BoxCenter.x=Lc->Lu.x+Lc->Lv.x*offset;
 BoxCenter.y=Lc->Lu.y+Lc->Lv.y*offset;
 BoxCenter.z=Lc->Lu.z+Lc->Lv.z*offset;

 vn->x = (int)((BoxCenter.x - G->vn.x - Radius)/G->side);
 vn->y = (int)((BoxCenter.y - G->vn.y - Radius)/G->side);
 vn->z = (int)((BoxCenter.z - G->vn.z - Radius)/G->side);

 vp->x = (int)((BoxCenter.x - G->vn.x + Radius)/G->side);
 vp->y = (int)((BoxCenter.y - G->vn.y + Radius)/G->side);
 vp->z = (int)((BoxCenter.z - G->vn.z + Radius)/G->side);

 vn->x=max(0,vn->x);
 vn->y=max(0,vn->y);
 vn->z=max(0,vn->z);
 vp->x=min(G->x-1,vp->x);
 vp->y=min(G->y-1,vp->y);
 vp->z=min(G->z-1,vp->z);

 return(Radius*Radius);
}


/***************************************************************************
*									   *
* CalcBestBox								   *
*									   *
* Calc the max sphere that is inside a box.				   *
*									   *
* The overhead of a more precise calculation does not improve the	   *
* performance of the algorithm, so this function is actually unused.	   *
*									   *
***************************************************************************/
double CalcBestBox(Face *f, Point3 *v, Line *Lc, UG *G,
				IntPoint3 *vn, IntPoint3 *vp, double Radius)
{
 IntPoint3 tvn,tvp;
 int MaxSide;
 int i;
 double MaxRadius,TestRadius,MinRadius,CalcRadius,NewRadius;

 CalcRadius=CalcBox(f,v,Lc,G,vn,vp,Radius);

 MaxSide=	      vp->x - vn->x + 1;
 MaxSide=min(MaxSide,(vp->y - vn->y + 1));
 MaxSide=min(MaxSide,(vp->z - vn->z + 1));

 MaxRadius = (MaxSide * G->side)/2.0;

 if(MaxRadius<=Radius) return CalcRadius;

 MinRadius = Radius;

 for(i=0;i<4;i++)
 {
  TestRadius=(MinRadius+MaxRadius)/2;

  NewRadius=CalcBox(f,v,Lc,G,&tvn,&tvp,TestRadius);
  if((tvn.x==vn->x)&&(tvn.y==vn->y)&&(tvn.z==vn->z)&&
     (tvp.x==vp->x)&&(tvp.y==vp->y)&&(tvp.z==vp->z))
		       {
			MinRadius=TestRadius;
			CalcRadius=NewRadius;
		       }
		  else	MaxRadius=TestRadius;
 }

 /* return CalcBox(f,v,Lc,G,vn,vp,MinRadius); */

 return CalcRadius;
}

/***************************************************************************
*									   *
* CalcLastScan								   *
*									   *
* If the first box we scanned was empty we scan that part of UG that is    *
* contained in the halfspace H affine to the face f.			   *
*									   *
* The scan must start from a Vertex of the UG an scan only that part of    *
* UG that is in H.							   *
*									   *
*									   *
*		 \							   *
*	    ......\..........<-Starting Vertex 'start'			   *
*	    .	.3 \.2	.1  .						   *
*	    .	.   \	.   .						   *
*  ^	    .........\.......						   *
* Y|	    .	.   .5\ .4  .						   *
*  |	    .	.   .  \.   .						   *
*  |	    ............\....						   *
*  |	    .	.   .	.\ 6.						   *
*--o---->   .	.   .	. \ .  <--		     inc.x = -1		   *
*  |   X    ...............\.	  HalfSpace H	     inc.y =- 1		   *
*  |	    .	.   .	.7  \ 						   *
*	    .	.   .	.   .\  					   *
*	    ................. \ 					   *
*	    ^		  \Plane p					   *
*	    |								   *
*	 Ending Vertex 'end'						   *
*									   *
*									   *
* This function also determines the scanning directions storing them in    *
* the IntPoint3 *inc. In the illustred case the direction are both	   *
* negative because we scan the dataset so that when we meet a cell having  *
* null intersection with H we stop scanning in that direction.		   *
*									   *
***************************************************************************/

void CalcLastScan(UG *C, IntPoint3 *start, IntPoint3 *end, IntPoint3 *inc, Plane *p)
{
 if(p->N.x>0) inc->x= -1;
	 else inc->x=  1;
 if(p->N.y>0) inc->y= -1;
	 else inc->y=  1;
 if(p->N.z>0) inc->z= -1;
	 else inc->z=  1;

 if(p->N.x>0) start->x= C->x-1;
	 else start->x= 0;
 if(p->N.y>0) start->y= C->y-1;
	 else start->y= 0;
 if(p->N.z>0) start->z= C->z-1;
	 else start->z= 0;

 if(p->N.x>0) end->x= 0;
	 else end->x= C->x-1;
 if(p->N.y>0) end->y= 0;
	 else end->y= C->y-1;
 if(p->N.z>0) end->z= 0;
	 else end->z= C->z-1;

}


/***************************************************************************
*									   *
* ExaminableCell							   *
*									   *
* Given an UG and a cell with x,y,z coord this function tell if it contains*
* points in the HalfSpace delimited by p.				   *
*									   *
* To check this we check only one vertex of the cell (the one nearest to   *
* the plane normal).							   *
*									   *
***************************************************************************/

boolean ExaminableCell(int x, int y, int z, Plane *p, UG *c)
{
 Point3 UGVertex;

 if(p->N.x>0) x++;
 if(p->N.y>0) y++;
 if(p->N.z>0) z++;

 UGVertex.x=x*c->side+c->vn.x;
 UGVertex.y=y*c->side+c->vn.y;
 UGVertex.z=z*c->side+c->vn.z;

 return RightSide(p, &UGVertex);

}

/***************************************************************************
*									   *
* ScanCellBox								   *
*									   *
* Given an UG scan the cell from vn to vp, returning TRUE if the point	   *
* found is at a 'fair' distance (i.e. we need no more scan), it returns the*
* dd-nearest and its distance.						   *
*									   *
* It return FALSE if the box is empty or we find a point, but we are not   *
* sure it is the dd-nearest.						   *
*									   *
***************************************************************************/

boolean ScanCellBox(IntPoint3 *vn, IntPoint3 *vp, Point3 *v,
	Face *f, Line *Lc, Plane *p, UG *G, int *index,double *MinRadius)
{
 int i,j,k;
 int CellIndex;
 Plist *P;
 Plane Mp;
 Point3 c;
 double Radius;
 int indpnt=-1;
 boolean Found=FALSE;

 for(i=vn->x; i<=vp->x; i++)
   for(j=vn->y; j<=vp->y; j++)
     for(k=vn->z; k<=vp->z; k++)
	{
	 CellIndex = i + j*G->x + k*G->y*G->x;
	 if(!UGIsMarked(G,CellIndex))
	   {
	    UGMark(G,CellIndex);
	    P=G->C[CellIndex];
	    while(P)
	      {
	       indpnt=P->p;
	       if(G->UsedPoint[indpnt]!=0)
	       if((indpnt!=f->v[0]) &&
		  (indpnt!=f->v[1]) &&
		  (indpnt!=f->v[2]) &&
		   RightSide(p,&(v[indpnt])) )
		   {
		    CalcMiddlePlane(&(v[indpnt]),&(v[f->v[0]]),&Mp);
		    if(CalcLinePlaneInter(Lc,&Mp,&c))
		      {
			Radius=V3SquaredDistanceBetween2Points(&c, &(v[indpnt]));
			if(!RightSide(p,&c)) Radius=-Radius;

			if(Radius==*MinRadius) Error("Cinque punti cocircolari!!\n",EXIT);
			if(Radius<*MinRadius)
				{
				 *MinRadius=Radius;
				 *index=indpnt;
				 Found=TRUE;
				}

		      }
		   }
	       P=P->next;
	      }  /* end while	     */
	   }	 /* end if !IsMarked */
	}	 /* end for	     */


 return Found;
}

/***************************************************************************
*									   *
* MakeLastScan								   *
*									   *
* It do the last scan of the dataset. See CalcLastScan notes		   *
*									   *
***************************************************************************/

boolean MakeLastScan(IntPoint3 *Start, IntPoint3 *End, IntPoint3 *Inc, Point3 *v,
	 Face *f, Line *Lc, Plane *p, UG *G, int *index, double *MinRadius)
{
 int i,j,k;
 int CellIndex;
 Plist *P;
 Plane Mp;
 Point3 c;
 double Radius;
 int indpnt=-1;

 for(i=Start->x; Inc->x*i <= Inc->x*End->x; i+=Inc->x)
   {
    if(!ExaminableCell(i,Start->y,Start->z,p,G)) i=End->x;
    else for(j=Start->y; Inc->y*j<=Inc->y*End->y; j+=Inc->y)
	  {
	   if(!ExaminableCell(i,j,Start->z,p,G)) j=End->y;
	   else for(k=Start->z; Inc->z*k<=Inc->z*End->z; k+=Inc->z)
		 {
		  if(!ExaminableCell(i,j,k,p,G)) k=End->z;
		  else
		   {
		    CellIndex = i + j*G->x + k*G->y*G->x;
		    if(!UGIsMarked(G, CellIndex))
			{
			P=G->C[CellIndex];
			while(P)
			 {
			  indpnt=P->p;
			  if((indpnt!=f->v[0]) &&
			     (indpnt!=f->v[1]) &&
			     (indpnt!=f->v[2]) &&
			     RightSide(p,&(v[indpnt])) )
			   {
			    CalcMiddlePlane(&(v[indpnt]),&(v[f->v[0]]),&Mp);
			    if(CalcLinePlaneInter(Lc,&Mp,&c))
			     {
			      Radius=V3SquaredDistanceBetween2Points(&c, &(v[indpnt]));

			      if(!RightSide(p,&c)) Radius=-Radius;
			      if(Radius==*MinRadius) Error("Cinque punti cocircolari!!\n",EXIT);
			      if(Radius<*MinRadius)
				{
				 *MinRadius=Radius;
				 *index=indpnt;
				}
			     }
			   }
			  P=P->next;
			 }   /* end while		  */
			}    /* end if IsMarked		  */
		   }	     /* end if examinable	  */
		 }  /* end for k      */
	  }	    /* end for j      */
   }		    /* end for i      */

 if(*index==-1) return FALSE;

 return TRUE;
}

/***************************************************************************
*									   *
* FastMakeTetra								   *
*									   *
* Given a face and a UG, finds the point in v that is dd-nearest to face f.*
*									   *
* The algorithm for searching of the dd-nearest point the following:	   *
*									   *
*									   *
*    choose a box radius						   *
*									   *
*    while found nothing and Box radius is not too large		   *
*	scan this box							   *
*	increase boxradius						   *
*									   *
*    if we found a point whose dd distance > boxradius			   *
*	make a security scan of a box with radius = point dd distance	   *
*									   *
*    if we find nothing scan the whole dataset				   *
*									   *
*									   *
***************************************************************************/

Tetra *FastMakeTetra(Face *f, Point3 *v, int n, UG *G)
{
 Plane p;
 Tetra *t;
 double CellBoxRadius= 0.0,	/* Radius of Cell Box to scan for dd-nearest */
	BoxRadius,
	FaceRadius,
	MinRadius=BIGNUMBER;

 Line Lc;
 IntPoint3 vn,vp, start, end, inc;
 int Index=-1;
 boolean Found=FALSE;

 SI.MakeTetra++;

 UGResetMark(G);

 if(!CalcPlane(&(v[f->v[0]]),&(v[f->v[1]]),&(v[f->v[2]]),&p))
		Error("Faccia composta da tre punti allineati!!\n",EXIT);

 CalcLineofCenter(&(v[f->v[0]]),&(v[f->v[1]]),&(v[f->v[2]]),&Lc);

 FaceRadius=V3DistanceBetween2Points(&(Lc.Lu), &(v[f->v[0]]));

 do
 {
  CellBoxRadius++;
  BoxRadius=CalcBox(f,v,&Lc,G,&vn,&vp, CellBoxRadius*FaceRadius);
  Found=ScanCellBox(&vn, &vp, v, f, &Lc, &p, G, &Index, &MinRadius);
 }
 while(!Found && CellBoxRadius <= 1);

 if(Found && MinRadius>BoxRadius)
 {double oldMinRadius=MinRadius;

  SI.SecondBox++;
  BoxRadius=CalcBox(f,v,&Lc,G,&vn,&vp, sqrt(MinRadius));
  ScanCellBox(&vn, &vp, v, f, &Lc, &p, G, &Index, &MinRadius);
  if(oldMinRadius>MinRadius) SI.UsefulSecondBox++;
 }

 if(!Found)
 {
  SI.EmptyBox++;
  CalcLastScan(G, &start, &end, &inc, &p);
  Found=MakeLastScan(&start, &end, &inc, v, f, &Lc, &p, G, &Index,&MinRadius);
 }

 if(!Found) return NULL;
 if(MinRadius>0)
 {
  SI.MinRadius+=sqrt(MinRadius);
  SI.MinRadiusNum++;
 }


 t=BuildTetra(f,Index);
 if(StatFlag)
 {Point3 C;
  CalcSphereCenter(&(v[f->v[0]]),&(v[f->v[1]]),&(v[f->v[2]]),&(v[Index]),&C);
  SI.Radius+=V3DistanceBetween2Points(&C, &(v[Index]));
 }
 if(CheckFlag)  CheckTetra(t,v,n);

 return t;
}
