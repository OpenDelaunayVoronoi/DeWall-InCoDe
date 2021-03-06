/* 
 * 2d and 3d Vector C Library
 * by Andrew Glassner
 * from "Graphics Gems", Academic Press, 1990
 */

#include <math.h>
#include <stdlib.h>
#include "graphics.h"

/******************/
/*   2d Library   */
/******************/

/* returns squared length of input vector */	
double V2SquaredLength(Vector2 *a)
{	return((a->x * a->x)+(a->y * a->y));
	}
	
/* returns length of input vector */
double V2Length(Vector2 *a)
{
	return(sqrt(V2SquaredLength(a)));
	}
	
/* negates the input vector and returns it */
Vector2 *V2Negate(Vector2 *v)
{
	v->x = -v->x;  v->y = -v->y;
	return(v);
	}

/* normalizes the input vector and returns it */
Vector2 *V2Normalize(Vector2 *v)
{
double len = V2Length(v);
	if (len != 0.0) { v->x /= len;	v->y /= len; }
	return(v);
	}


/* scales the input vector to the new length and returns it */
Vector2 *V2Scale(Vector2 *v, double newlen)
{
double len = V2Length(v);
	if (len != 0.0) { v->x *= newlen/len;	v->y *= newlen/len; }
	return(v);
	}

/* return vector sum c = a+b */
Vector2 *V2Add(Vector2 *a, Vector2 *b, Vector2 *c)
{
	c->x = a->x+b->x;  c->y = a->y+b->y;
	return(c);
	}
	
/* return vector difference c = a-b */
Vector2 *V2Sub(Vector2 *a, Vector2 *b, Vector2 *c)
{
	c->x = a->x-b->x;  c->y = a->y-b->y;
	return(c);
	}

/* return the dot product of vectors a and b */
double V2Dot(Vector2 *a, Vector2 *b)
{
	return((a->x*b->x)+(a->y*b->y));
	}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector2 *V2Lerp(Vector2 *lo, Vector2 *hi, double alpha, Vector2 *result)
{
	result->x = LERP(alpha, lo->x, hi->x);
	result->y = LERP(alpha, lo->y, hi->y);
	return(result);
	}


/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector2 *V2Combine (Vector2 *a, Vector2 *b, Vector2 *result, double ascl, double bscl)
{
	result->x = (ascl * a->x) + (bscl * b->x);
	result->y = (ascl * a->y) + (bscl * b->y);
	return(result);
	}

/* multiply two vectors together component-wise */
Vector2 *V2Mul (Vector2 *a, Vector2 *b, Vector2 *result)
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
	return(result);
	}

/* return the distance between two points */
double V2DistanceBetween2Points(Point2 *a, Point2 *b)
{
double dx = a->x - b->x;
double dy = a->y - b->y;
	return(sqrt((dx*dx)+(dy*dy)));
	}

/* return the vector perpendicular to the input vector a */
Vector2 *V2MakePerpendicular(Vector2 *a, Vector2 *ap)
{
	ap->x = -a->y;
	ap->y = a->x;
	return(ap);
	}

/* create, initialize, and return a new vector */
Vector2 *V2New(double x, double y)
{
Vector2 *v = NEWTYPE(Vector2);
	v->x = x;  v->y = y; 
	return(v);
	}
	

/* create, initialize, and return a duplicate vector */
Vector2 *V2Duplicate(Vector2 *a)
{
Vector2 *v = NEWTYPE(Vector2);
	v->x = a->x;  v->y = a->y; 
	return(v);
	}
	
/* multiply a point by a matrix and return the transformed point */
Point2 *V2MulPointByMatrix(Point2 *p, Matrix3 *m)
{
double w;
Point2 ptmp;
	ptmp.x = (p->x * m->element[0][0]) + 
             (p->y * m->element[1][0]) + m->element[2][0];
	ptmp.y = (p->x * m->element[0][1]) + 
             (p->y * m->element[1][1]) + m->element[2][1];
	w    = (p->x * m->element[0][2]) + 
             (p->y * m->element[1][2]) + m->element[2][2];
	if (w != 0.0) { ptmp.x /= w;  ptmp.y /= w; }
	*p = ptmp;
	return(p);
	}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix3 *V2MatMul(Matrix3 *a, Matrix3 *b, Matrix3 *c)
{
int i, j, k;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			c->element[i][j] = 0;
		for (k=0; k<3; k++) c->element[i][j] += 
				a->element[i][k] * b->element[k][j];
			}
		}
	return(c);
	}




/******************/
/*   3d Library   */
/******************/
	
/* returns squared length of input vector */	
double V3SquaredLength(Vector3 *a)
{
	return((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
	}

/* returns length of input vector */
double V3Length(Vector3 *a)
{
	return(sqrt(V3SquaredLength(a)));
	}

/* negates the input vector and returns it */
Vector3 *V3Negate(Vector3 *v)
{
	v->x = -v->x;  v->y = -v->y;  v->z = -v->z;
	return(v);
}

/* normalizes the input vector and returns it */
Vector3 *V3Normalize(Vector3 *v)
{
double len = V3Length(v);
	if (len != 0.0) { v->x /= len;	v->y /= len; v->z /= len; }
	return(v);
	}

/* scales the input vector to the new length and returns it */
Vector3 *V3Scale(Vector3 *v, double newlen)
{
double len = V3Length(v);
	if (len != 0.0) {
	v->x *= newlen/len;   v->y *= newlen/len;  v->z *= newlen/len;
	}
	return(v);
	}


/* return vector sum c = a+b */
Vector3 *V3Add(Vector3 *a, Vector3 *b, Vector3 *c)
{
	c->x = a->x+b->x;  c->y = a->y+b->y;  c->z = a->z+b->z;
	return(c);
	}
	
/* return vector difference c = a-b */
Vector3 *V3Sub(Vector3 *a, Vector3 *b, Vector3 *c)
{
	c->x = a->x-b->x;  c->y = a->y-b->y;  c->z = a->z-b->z;
	return(c);
	}

/* return the dot product of vectors a and b */
double V3Dot(Vector3 *a, Vector3 *b)
{
	return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
	}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector3 *V3Lerp(Vector3 *lo, Vector3 *hi,double  alpha, Vector3 *result)
{
	result->x = LERP(alpha, lo->x, hi->x);
	result->y = LERP(alpha, lo->y, hi->y);
	result->z = LERP(alpha, lo->z, hi->z);
	return(result);
	}

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector3 *V3Combine (Vector3 *a, Vector3 *b, Vector3 *result, double ascl, double bscl)
{
	result->x = (ascl * a->x) + (bscl * b->x);
	result->y = (ascl * a->y) + (bscl * b->y);
	result->y = (ascl * a->z) + (bscl * b->z);
	return(result);
	}


/* multiply two vectors together component-wise and return the result */
Vector3 *V3Mul (Vector3 *a, Vector3 *b, Vector3 *result)
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
	result->z = a->z * b->z;
	return(result);
	}

/* return the distance between two points */
double V3DistanceBetween2Points(Point3 *a, Point3 *b)
{
double dx = a->x - b->x;
double dy = a->y - b->y;
double dz = a->z - b->z;
	return(sqrt((dx*dx)+(dy*dy)+(dz*dz)));
	}

/* return the distance between two points */
double V3SquaredDistanceBetween2Points(Point3 *a, Point3 *b)
{
double dx = a->x - b->x;
double dy = a->y - b->y;
double dz = a->z - b->z;
	return((dx*dx)+(dy*dy)+(dz*dz));
	}

/* return the cross product c = a cross b */
Vector3 *V3Cross(Vector3 *a, Vector3 *b, Vector3 *c)
{
	c->x = (a->y*b->z) - (a->z*b->y);
	c->y = (a->z*b->x) - (a->x*b->z);
	c->z = (a->x*b->y) - (a->y*b->x);
	return(c);
	}

/* create, initialize, and return a new vector */
Vector3 *V3New(double x, double y, double z)
{
Vector3 *v = NEWTYPE(Vector3);
	v->x = x;  v->y = y;  v->z = z;
	return(v);
	}

/* create, initialize, and return a duplicate vector */
Vector3 *V3Duplicate(Vector3 *a)
{
Vector3 *v = NEWTYPE(Vector3);
	v->x = a->x;  v->y = a->y;  v->z = a->z;
	return(v);
	}

	
/* multiply a point by a matrix and return the transformed point */
Point3 *V3MulPointByMatrix(Point3 *p, Matrix4 *m)
{
double w;
Point3 ptmp;
	ptmp.x = (p->x * m->element[0][0]) + (p->y * m->element[1][0]) + 
		 (p->z * m->element[2][0]) + m->element[3][0];
	ptmp.y = (p->x * m->element[0][1]) + (p->y * m->element[1][1]) + 
		 (p->z * m->element[2][1]) + m->element[3][1];
	ptmp.z = (p->x * m->element[0][2]) + (p->y * m->element[1][2]) + 
		 (p->z * m->element[2][2]) + m->element[3][2];
	w =    (p->x * m->element[0][3]) + (p->y * m->element[1][3]) + 
		 (p->z * m->element[2][3]) + m->element[3][3];
	if (w != 0.0) { ptmp.x /= w;  ptmp.y /= w;  ptmp.z /= w; }
	*p = ptmp;
	return(p);
	}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix4 *V3MatMul(Matrix4 *a, Matrix4 *b, Matrix4 *c)
{
int i, j, k;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			c->element[i][j] = 0;
			for (k=0; k<4; k++) c->element[i][j] += 
				a->element[i][k] * b->element[k][j];
			}
		}
	return(c);
	}

/* binary greatest common divisor by Silver and Terzian.  See Knuth */
/* both inputs must be >= 0 */
gcd(int u, int v)
{
int k, t, f;
	if ((u<0) || (v<0)) return(1); /* error if u<0 or v<0 */
	k = 0;  f = 1;
	while ((0 == (u%2)) && (0 == (v%2))) {
		k++;  u>>=1;  v>>=1,  f*=2;
		}
	if (u&01) { t = -v;  goto B4; } else { t = u; }
	B3: if (t > 0) { t >>= 1; } else { t = -((-t) >> 1); }
	B4: if (0 == (t%2)) goto B3;

	if (t > 0) u = t; else v = -t;
	if (0 != (t = u - v)) goto B3;
	return(u*f);
	}
