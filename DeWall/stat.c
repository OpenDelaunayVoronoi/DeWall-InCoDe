/***************************************************************************
******************************* 11/Feb/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:	stat.c							   *
*                                                                          *
* PURPOSE:	Functions for Statistics evaluations.			   *
*                                                                          *
* IMPORTS:                                                                 *
*                                                                          *
* EXPORTS:								   *
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "dewall.h"

extern StatInfo SI;
extern NumStatFlag;

void InitStat()
{
 SI.Tetra=1;
 SI.MakeTetra=0;
 SI.EqualTest=0;
 SI.MaxPointPerCell=0;
 SI.EmptyBox=0;
 SI.SecondBox=0;
 SI.UsefulSecondBox=0;
 SI.TestedCell=0;
 SI.TestedPoint=0;
 SI.MinRadius=0;
 SI.MinRadiusNum=0;
 SI.Radius=0;
SI.WallSize=0;
}

void PrintStat()
{
 printf("+----- Statistical Informations ----------------------------------------+\n");

 printf("|Points     |Time (sec.)|Tetras     |Faces      |CH Faces   |TetraRadius|");
 printf("\n");
 printf("|%7i    |%7.3f    |%7i    |%7i    |%7i    |%7.3f    |",SI.Point,SI.Secs,SI.Tetra,SI.Face,SI.CHFace,SI.Radius/SI.Tetra);
 printf("\n");

 printf("|UGMakeTetra|Empty Box  | 2nd Box   |Useful 2nd |PntPerFace |CellPerFace|");
 printf("\n");
 printf("|%7i    |%7i    |%7i    |%7i    |%7.2f    |%7.2f    ",SI.MakeTetra,SI.EmptyBox,SI.SecondBox,SI.UsefulSecondBox,(double)SI.TestedPoint/SI.Face,(double)SI.TestedCell/SI.Face);
 printf("\n");
 printf("WallSize %7i, Cell Num %7i   Empty Cell %7i MaxPoint %7i   \n",
	SI.WallSize,SI.Cell, SI.EmptyCell, SI.MaxPointPerCell);
}

void PrintNumStat()
{
 printf("%7i   , %7.3f   , %7i   , %7i   , %7i   , %7.3f   , ",
	SI.Point,SI.Secs,SI.Tetra,SI.Face,SI.CHFace,SI.Radius/SI.Tetra);
 printf("%7i   , %7i   , %7i   , %7i   , %7.2f   , %7.2f   , ",
	SI.MakeTetra, SI.EmptyBox, SI.SecondBox, SI.UsefulSecondBox,
	(double)SI.TestedPoint/SI.Face,(double)SI.TestedCell/SI.Face);

}
void PrintUgStat()
{
  
 printf("Cell Num %7i   Empty Cell %7i MaxPoint %7i   \n",
	SI.Cell, SI.EmptyCell, SI.MaxPointPerCell);
 

}

void PrintNumStatTitle()
{
 printf("Points    , Time      , Tetras    , Faces     , CH Faces  , TetraRadius,");
 printf("UGMakTetra, Empty Box ,  2nd Box  , Useful 2nd, PntPerFace, CellPerFace\n");
}
