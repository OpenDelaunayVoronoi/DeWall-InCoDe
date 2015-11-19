/***************************************************************************
******************************* 05/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:      file.c                                                     *
*                                                                          *
* PURPOSE:      Functions for I/O and support to list management.          *
*                                                                          *
* IMPORTS:      OList                                                      *
*                                                                          *
* EXPORTS:      ReadPoints          Read a Point file                      *
*               Tetra2ShortTetra    Trasform a Tetra in a ShortTetra       *
*               WriteTetraList      Write a tetra list on a file           *
*               HashFace            Hash key function for Faces            *
*               EqualFace           Testing equalness of Faces             *
*               HashTetra           Hash key function for Tetras           *
*               EqualTetra          Testing equalness of Tetras            *
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
#include "dewall.h"

/* Global for Statistic Infomations */

extern StatInfo SI;


/*
 * ReadPoints
 *
 * Legge un file di punti e restituisce un vettore di punti e il numero n di punti letti.
 * Formato del File:
 *      N
 *      X Y Z
 *      X Y Z
 *      ...
 *
 */

Point3 *ReadPoints(char *filename, int *n)
{
 FILE *fp;
 int i;
 Point3 *vec;

 fp=fopen(filename,"r");
 if(!fp) Error("ReadPoints, Unable to open input file.\n",EXIT);

 fscanf(fp,"%d",n);
 vec=(Point3 *)calloc((size_t)*n,sizeof(Point3));
 if(!vec) Error("ReadPoints, Not enough memory to load point dataset.\n",EXIT);

 for(i=0;i<*n;i++)
   {
     fscanf(fp,"%lf %lf %lf ",&(vec[i].x),&(vec[i].y),&(vec[i].z));
     vec[i].mark=-1;
   }
 return vec;
}



/*
 * Tetra2ShortTetra
 *
 * Dato un tetra restituisce un puntatore ad un ShortTetra
 * vettore di int con i quattro vertici
 *
 */

ShortTetra *Tetra2ShortTetra(Tetra *t, Point3 *BaseV)
{
 int i;
 ShortTetra *st;

 st=(ShortTetra *)malloc(sizeof(ShortTetra));
 if(!st)
   Error("Tetra2ShortTetra, Unable to allocate memory for ShortTetra\n",EXIT);


 st->v[0]=(int)(t->f[0]->v[0]-BaseV);
 st->v[1]=(int)(t->f[0]->v[1]-BaseV);
 st->v[2]=(int)(t->f[0]->v[2]-BaseV);

 for(i=0;i<3;i++)
 {
  if(t->f[1]->v[i] != BaseV+st->v[0] &&
     t->f[1]->v[i] != BaseV+st->v[1] &&
     t->f[1]->v[i] != BaseV+st->v[2] ) st->v[3] = (int)(t->f[1]->v[i]-BaseV);
 }

 return st;
}

/*
 * WriteTetraList
 *
 * Scrive nel file fp una lista di tetraedri,
 * ogni tetraedro e' scritto come 4 vertici.
 * All'inizio del file viene scritto il numero di tetraedri, ricavato dalla
 * variabile globale Tetra.
 */

void WriteTetraList(List TList, FILE *fp)
{
 ShortTetra *t;

 fprintf(fp,"%d\n",CountList(TList));
 while(ExtractList(&t,TList))
   fprintf(fp,"%6i %6i %6i %6i\n",t->v[0],t->v[1],t->v[2],t->v[3]);
}




/*
 * HashFace
 *
 * Data una faccia ed un intero indicante la lunghezza del vettore Hash
 * restituisce la posizione nel vettore.
 *
 * tecnica usata: XOR dei tre vertici
 *
 */


int HashFace(void *F)
{
 Face *f=(Face *)F;
 return (int)((long)f->v[0]) ^ ((long)f->v[1]) ^ ((long)f->v[2]) ;
}



/*
 * EqualFace
 *
 * Equal testing function for faces list operations
 * two faces are equals if they have the same vertices
 * (even in different order).
 *
 */

boolean EqualFace(void *F1, void *F2)
{
 Face *f1=(Face *)F1;
 Face *f2=(Face *)F2;

 SI.EqualTest++;
 if( f1->v[0] != f2->v[0] &&
     f1->v[0] != f2->v[1] &&
     f1->v[0] != f2->v[2] ) return FALSE;

 if( f1->v[1] != f2->v[0] &&
     f1->v[1] != f2->v[1] &&
     f1->v[1] != f2->v[2] ) return FALSE;

 if( f1->v[2] != f2->v[0] &&
     f1->v[2] != f2->v[1] &&
     f1->v[2] != f2->v[2] ) return FALSE;

 return TRUE;
}

/* EqualTetra
 *
 * Equal testing function for tetrahedra list operations
 * two tetrahedra are equals if they have the same vertices
 * (even in different order).
 */

boolean EqualTetra(void *T0, void *T1)
{
 ShortTetra *t0=(ShortTetra *)T0;
 ShortTetra *t1=(ShortTetra *)T1;

 if(t0->v[0]!=t1->v[0] &&
    t0->v[0]!=t1->v[1] &&
    t0->v[0]!=t1->v[2] &&
    t0->v[0]!=t1->v[3] ) return FALSE;

 if(t0->v[1]!=t1->v[0] &&
    t0->v[1]!=t1->v[1] &&
    t0->v[1]!=t1->v[2] &&
    t0->v[1]!=t1->v[3] ) return FALSE;

 if(t0->v[2]!=t1->v[0] &&
    t0->v[2]!=t1->v[1] &&
    t0->v[2]!=t1->v[2] &&
    t0->v[2]!=t1->v[3] ) return FALSE;

 if(t0->v[3]!=t1->v[0] &&
    t0->v[3]!=t1->v[1] &&
    t0->v[3]!=t1->v[2] &&
    t0->v[3]!=t1->v[3] ) return FALSE;

 return TRUE;
}

/*
 * Hash Function for fast list operation.
 * It must return the same key for equal tetrahedra (even if vertices are
 * differentely ordered).
 */

int HashTetra(void *T)
{
 ShortTetra *t=(ShortTetra *)T;

 return (t->v[0]*73)^(t->v[1]*73)^(t->v[2]*73)^(t->v[3]*73);
}
