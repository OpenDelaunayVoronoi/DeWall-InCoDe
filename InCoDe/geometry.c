/***************************************************************************
******************************* 05/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:      geometry.c                                                 *
*                                                                          *
* PURPOSE:      Functions for geometric calculations.                      *
*                                                                          *
* IMPORTS:                                                                 *
*                                                                          *
* EXPORTS:      CalcPlane           The plane given 3 points               *
*               RightSide           A point is on RightSide respect a plane*
*               CalcPlaneInter      Intersection of 3 planes               *
*               CalcSphereCenter    Center of sphere for 4 points          *
*               NormalToFace                                               *
*               CalcMiddlePlane     Middle plane given two points          *
*               CalcLineofCenter    Given a face calc the line containing  *
*                                   centers of spheres touching the face   *
*                                   vertices.                              *
*               CalcLinePlaneInter  intersection of a line and a plane     *
*               PointBelongtoPlane                                         *
*               PointBelongtoLine                                          *
*                                                                          *
*		ReverseFace	    Given a face reverse it		   *
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <OList/general.h>
#include <OList/error.h>
#include <OList/olist.h>

#include "graphics.h"
#include "incode.h"

extern StatInfo SI;

/*
 * CalcPlane
 *
 * Dati tre punti restituisce un piano che passi per i tre punti, il piano e'
 * orientato in maniera tale da in piedi sopra il piano (orientati come la
 * normale) si vedano i tre punti in senso antiorario. Se i tre punti erano
 * quelli di una faccia questo corrisponde a definire come esterno di una
 * faccia quello da cui si vedono i punti in senso antiorario.
 *
 */

Plane *CalcPlane(Vector3 *p0, Vector3 *p1, Vector3 *p2, Plane *p )
{
 Vector3 v1,v2;
 Vector3 *N;
 N=&(p->N);


 V3Sub(p1,p0,&v1);
 V3Sub(p2,p0,&v2);
 V3Cross(&v1,&v2,N);
 if(N->x==0.0 && N->y==0.0 && N->z==0.0) return NULL;

 V3Normalize(N);

 p->off=V3Dot(N,p0);

 return p;
}

/*
 * RightSide
 *
 * Dato un piano ed un punto restituisce vero se il punto si trova nel
 * semispazio indicato dalla normale.
 *
 */

boolean RightSide(Plane *p, Point3 *v)
{
 if(V3Dot(v,&(p->N)) > p->off + EPSILON) return TRUE;
				    else return FALSE;
}


/*
 * CalcPlaneInter
 *
 * Dati tre piani calcola la loro intersezione risolvendo un sistema 3x3.
 * L'inversa della matrice e' calcolata come esposto qui sotto:
 *

	    + a11 a22 - a12 a21  a02 a21 - a01 a22    a01 a12 - a02 a11 +
   -1  1    |                                                           |
  A = ---   | a12 a20 - a10 a22  a00 a22 - a02 a20    a02 a10 - a00 a12 |
      det   |                                                           |
	    + a10 a21 - a11 a20  a01 a20 - a00 a21    a00 a11 - a01 a10 +
 *
 * Per l'espansione dell'inversa si ringrazia DERIVE.
 *
 */

Point3 *CalcPlaneInter(Plane *p0, Plane *p1,Plane *p2,Point3 *c)
{
 double det;
 double  a[3][3],inv[3][3];
 Vector3 V;


 V.x=p0->off;
 V.y=p1->off;
 V.z=p2->off;

 a[0][0]=p0->N.x;
 a[0][1]=p0->N.y;
 a[0][2]=p0->N.z;

 a[1][0]=p1->N.x;
 a[1][1]=p1->N.y;
 a[1][2]=p1->N.z;

 a[2][0]=p2->N.x;
 a[2][1]=p2->N.y;
 a[2][2]=p2->N.z;

 det=a[0][0]*(a[1][1]*a[2][2]-a[1][2]*a[2][1])+
     a[0][1]*(a[1][2]*a[2][0]-a[1][0]*a[2][2])+
     a[0][2]*(a[1][0]*a[2][1]-a[1][1]*a[2][0]);

 if(det == 0) return NULL;


 inv[0][0]= a[1][1]*a[2][2]-a[1][2]*a[2][1];
 inv[0][1]= a[0][2]*a[2][1]-a[0][1]*a[2][2];
 inv[0][2]= a[0][1]*a[1][2]-a[0][2]*a[1][1];

 inv[1][0]= a[1][2]*a[2][0]-a[1][0]*a[2][2];
 inv[1][1]= a[0][0]*a[2][2]-a[0][2]*a[2][0];
 inv[1][2]= a[0][2]*a[1][0]-a[0][0]*a[1][2];

 inv[2][0]= a[1][0]*a[2][1]-a[1][1]*a[2][0];
 inv[2][1]= a[0][1]*a[2][0]-a[0][0]*a[2][1];
 inv[2][2]= a[0][0]*a[1][1]-a[0][1]*a[1][0];


 c->x=(inv[0][0]*V.x +inv[0][1]*V.y +inv[0][2]*V.z)/det;
 c->y=(inv[1][0]*V.x +inv[1][1]*V.y +inv[1][2]*V.z)/det;
 c->z=(inv[2][0]*V.x +inv[2][1]*V.y +inv[2][2]*V.z)/det;

 return c;
}

/*
 * CalcSphereCenter
 *
 * Dati quattro punti calcola il centro della sfera ad essi circoscritta.
 * Tale centro e' l'intersezione di tre piani di mezzo tra tre coppie distinte
 * di punti; si ricordi che il piano di mezzo tra due punti e' il luogo
 * geometrico dei punti equidistanti ai due punti dati, e che il centro della
 * sfera e' equidistante dai quattro punti.
 *
 */

Point3 *CalcSphereCenter(Point3 *v0, Point3 *v1, Point3 *v2, Point3 *v3,Point3 *c)
{
 Vector3 d1, d2, d3;

 Plane p1,p2,p3;

 V3Add(v0, v1, &d1);
 V3Add(v0, v2, &d2);
 V3Add(v0, v3, &d3);

 d1.x/=2; d1.y/=2; d1.z/=2;
 d2.x/=2; d2.y/=2; d2.z/=2;
 d3.x/=2; d3.y/=2; d3.z/=2;

 V3Sub(v0, v1, &(p1.N));
 V3Sub(v0, v2, &(p2.N));
 V3Sub(v0, v3, &(p3.N));

 V3Normalize(&(p1.N));
 V3Normalize(&(p2.N));
 V3Normalize(&(p3.N));

 p1.off = V3Dot(&(p1.N), &d1);
 p2.off = V3Dot(&(p2.N), &d2);
 p3.off = V3Dot(&(p3.N), &d3);


 if(CalcPlaneInter(&p1, &p2, &p3, c)) return c;
				else  return NULL;

}

/*
 * NormalToFace
 *
 * Data una faccia restituisce la normale alla faccia.
 * Utilizzata per il calcolo dell'ombreggiatura.
 *
 */

Point3 *NormalToFace(Point3 *v, Face *f, Point3 *n)
{
 Point3 t1,t2;

 V3Sub(&(v[f->v[1]]),&(v[f->v[0]]),&t1);
 V3Sub(&(v[f->v[2]]),&(v[f->v[0]]),&t2);
 V3Cross(&t1,&t2,n);

 return n;
}



/*
 * CalcMiddlePlane
 *
 * Dati due punti calcola il piano di mezzo, luogo geometrico dei punti
 * equidistanti dai due punti dati.
 *
 */

Plane *CalcMiddlePlane(Point3 *p1, Point3 *p2, Plane *p)
{Point3 m0;

 V3Add(p1,p2,&m0);
 m0.x/=2;m0.y/=2;m0.z/=2;
 V3Sub(p1,p2,&(p->N));
 V3Normalize(&(p->N));
 p->off = V3Dot(&m0,&(p->N));

 return p;
}


/*
 * CalcLineofCenter
 *
 * Dati i tre vertici di una faccia calcola la linea che contiene i centri di
 * tutte le sfere che passano per la faccia. Tale linea e' l'intersezione tra
 * due piani di mezzo tra due coppie distinte di punti della faccia.
 * Qui viene calcolato piu' rudimentalmente: si calcola dove passa la linea
 * nel piano che contiene la faccia e si usa la normale come direzione.
 *
 */

Line *CalcLineofCenter(Point3 *p1, Point3 *p2, Point3 *p3, Line *l)
{
 Plane pl1,pl2,pl3;

  CalcMiddlePlane(p1,p2,&pl1);
  CalcMiddlePlane(p1,p3,&pl2);
  CalcPlane(p1,p2,p3,&pl3);

  CalcPlaneInter(&pl1,&pl2,&pl3,&(l->Lu));

  l->Lv=pl3.N;

  return l;
}


/*
 * CalcLinePlaneInter
 *
 * Dati una linea ed un piano ne calcola l'intersezione.
 *
 */

Point3 *CalcLinePlaneInter(Line *l, Plane *p, Point3 *c)
{
 double t,t1;

 t  =  - p->off + V3Dot(&(l->Lu),&(p->N));
 t1 = V3Dot(&(l->Lv),&(p->N));
 if(t1==0) return NULL;
 t=-(t/t1);
 c->x=l->Lv.x * t;
 c->y=l->Lv.y * t;
 c->z=l->Lv.z * t;

 V3Add(c,&(l->Lu),c);

 SI.TestedPoint++;

 return c;
}


/*
 * PointBelongtoLine
 *
 * Restituisce TRUE se il punto appartiene alla linea.
 *
 */

boolean PointBelongtoLine(Point3 *p, Line *l)
{
 Point3 temp;
 double t;

 V3Sub(p,&(l->Lu),&temp);

 t=temp.x/l->Lv.x;

 if(fabs(l->Lv.y*t-temp.y) <= EPSILON) return 1;

 return 0;
}


/*
 * PointBelongtoPlane
 *
 * Restituisce TRUE se il punto appartiene al Piano.
 *
 */

boolean PointBelongtoPlane(Point3 *p, Plane *pl)
{ double v;
 v=  -pl->off+V3Dot(p,&(pl->N));
 if(fabs(v)<=0.001) return 1;

 return 0;
}

/*
 * ReverseFace
 *
 * Reverse a Face exchanging the order of first two vertices.
 *
 */

boolean ReverseFace(Face *f)
{
 int index;

  index  = f->v[0];
 f->v[0] = f->v[1];
 f->v[1] =  index;

 return TRUE;
}
