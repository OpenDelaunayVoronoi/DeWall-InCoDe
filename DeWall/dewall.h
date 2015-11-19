/***************************************************************************
******************************* 11/Feb/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*                                                                          *
*    FILE:	dewall.h						   *
*                                                                          *
* PURPOSE:      Type definitions and prototyping.                          *
*                                                                          *
* IMPORTS:      OList definitions                                          *
*                                                                          *
* EXPORTS:      Face            Definition                                 *
*               Tetra           Definition                                 *
*               ShortTetra      Definition                                 *
*               Plane           Definition                                 *
*               Line            Definition                                 *
*               Plist           Definition                                 *
*		UG		Definition				   *
*		StatInfo	Definition				   *
*                                                                          *
*   NOTES:	This is the optimized version of the DeWall algorithm.	   *
*               It use hashing and Uniform Grid techniques to speed up     *
*               list management and tetrahedra construction.               *
*                                                                          *
*               This program uses the 0.9 release of OList for list        *
*               management.                                                *
*                                                                          *
****************************************************************************
***************************************************************************/

#define BIGNUMBER 1000000000.0
#define EPSILON 0.0000001



typedef struct Facestruct	/* A Face is an array of 3 Point3 pointers*/
{				/* Faces are oriented, top follows right  */
 Point3 *v[3];			/* hand rules for vertex ordering	  */
} Face;


typedef struct Tetrastruct	/* A Tetra is an array of 4 Face pointers */
{
 Face *f[4];
} Tetra;

typedef struct Tetra2struct
{				/* A ShortTetra is an abbreviated type for*/
 int v[4];			/* representing Tetrahedra. It contains   */
} ShortTetra;			/* only the four index of its vertices	  */


typedef struct Planestruct	/* A Plane is a normal N and its offset   */
{				/* from origin. The normal is mantained   */
 Vector3 N;			/* normalized.				  */
 double off;
} Plane;

typedef struct Linestruct {	/* A Line in the space is the direction   */
	Vector3 Lu;		/* and a point where it pass.		  */
	Vector3 Lv;
	} Line;


struct plist {			/* This is a List of Point. I dont used   */
	Point3 *p;		/* Olist for this structure because it is */
	struct plist *next;	/* really very simple, used many times and*/
	};			/* I write this code before Olist :-)	  */
typedef struct plist Plist;

enum Axis
{XAxis, YAxis, ZAxis};		/* An Axis, used in recursive calls of	  */
				/* Dewall to indicate how to build next   */
				/* wall 				  */


/****************************************************************************
*									    *
* UG (Uniform Grid)							    *
*									    *
* A Uniform Grid is a regular not hierarchycal space partition into cubic   *
* cells. A list of points contained in each cell is mantained attached to   *
* each cell.								    *
* The set of cell is mantained as a vector of Plist (List of points).	    *
* The cell of (X,Y,Z) coords has the position: Z*UG.x*UG.y + Y*UG.x + X.    *
*									    *
****************************************************************************/

typedef struct UGstruct {
	int x;		/* Cell number for each axis */
	int y;
	int z;

	int n;		/* Total cell number */

	Point3 vp;	/* Maximum vertex of UG */
	Point3 vn;	/* Minimum vertex of UG */

	double side;	/* Cell Edge */

	Plist **C;	/* Vector of Plist pointers, each Plist is */
			/* associated to a cell and contains the   */
			/* Points in the cell.			   */
	int *UsedPoint;
	int *Marked;
	int Mark;
	} UG;



typedef struct StatInfostruct
{
			/* General Stats	*/
  int	 Point;
  double Secs;
  int	 Face;
  int	 CHFace;
  int	 Tetra;
			/* UG Stats		*/
  int	 Cell;
  int	 EmptyCell;
  int	 MaxPointPerCell;
  double CellEdge;
			/* List Stats		*/
  int	 QSize;
  int	 EqualTest;
			/* Incode Stats 	*/
  long	 TestedPoint;
  int	 MakeTetra;
  double MinRadius;
  double Radius;
  int	 MinRadiusNum;
			/* Incode + UG Stats	*/
  int	 EmptyBox;
  int	 SecondBox;
  int	 UsefulSecondBox;
  long	 TestedCell;

  int WallSize;
} StatInfo;

/**************************************************************************
*									  *
*				PROTOTYPES				  *
*									  *
**************************************************************************/

/**************************************************************************
*   file.c                                                                *
**************************************************************************/

Point3 *ReadPoints(char *filename, int *n);
void WriteTetraList(List T, FILE *fp);
ShortTetra *Tetra2ShortTetra(Tetra *t,Point3 *BaseV);

int HashFace(void *F);
int HashTetra(void *T);
boolean EqualFace(void *F1, void *F2);
boolean EqualTetra(void *T0,void *T1);


/**************************************************************************
*   geometry.c                                                            *
**************************************************************************/

Plane *CalcPlane(Vector3 *p0, Vector3 *p1, Vector3 *p2, Plane *p );
boolean RightSide(Plane *p, Point3 *v);
Point3 *CalcPlaneInter(Plane *p0, Plane *p1,Plane *p2,Point3 *c);
Point3 *CalcSphereCenter(Point3 *v0, Point3 *v1, Point3 *v2, Point3 *v3,Point3 *c);
Point3 *NormalToFace(Face *f, Point3 *n);
Point3 *CalcLinePlaneInter(Line *l, Plane *p, Point3 *c);
Plane *CalcMiddlePlane(Point3 *p1, Point3 *p2, Plane *p);
Line *CalcLineofCenter(Point3 *p1, Point3 *p2, Point3 *p3, Line *l);
boolean PointBelongtoLine(Point3 *p, Line *l);
boolean PointBelongtoPlane(Point3 *p, Plane *pl);
boolean ReverseFace(Face *f);
int Intersect(Face *f, Plane *p);

/**************************************************************************
*   unifgrid.c                                                            *
**************************************************************************/

UG *BuildUG(Point3 *v[], int *UsedPoint, int n, int m, UG *C);
Tetra *FastMakeTetra(Face *f,Point3 *v[], int n, UG *C);
void EraseUG(UG *G);


/**************************************************************************
*   main.c                                                                *
**************************************************************************/

void CheckTetra(Tetra *t, Point3 *v[], int n);
Tetra *BuildTetra(Face *f, Point3 *p);


/**************************************************************************
*   stat.c								  *
**************************************************************************/

void InitStat();
void PrintStat();
void PrintNumStat();
void PrintNumStatTitle();
