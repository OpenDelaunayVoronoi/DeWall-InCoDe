/***************************************************************************
******************************* 18/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	olist.h							   *
*									   *
* PURPOSE:	Define List type.					   *
*									   *
*   NOTES:	It can be harmlessy included more than one time.	   *
*		This is the file user must include to use LIST funtions.   *
*									   *
*   18/Jan/93	Changed from list.h to olist.h				   *
*									   *
****************************************************************************
***************************************************************************/

#ifndef OLIST_H 	/* If LIST_H is already defined all this file  */
			/* must be skipped.                            */
#define OLIST_H

/***************************************************************************
*   CONST:	NULL_LIST						   *
*									   *
* PURPOSE:	Defining a null value for lists.			   *
*									   *
*   NOTES:	This defined value is for first initializing lists at	   *
*		their declaration.					   *
*									   *
***************************************************************************/
#ifndef NULL_LIST
#define NULL_LIST NULL
#endif

/***************************************************************************
*									   *
*    TYPE:	TypeList						   *
*									   *
* PURPOSE:	Defining the strategy of insertion-extraction in a list    *
*									   *
*   NOTES:	This constants define the type of a list.		   *
*		FIFO and LIFO are the usual FirstInFirstOut and 	   *
*		LastInFirstOut strategy you know. ORDERED define a	   *
*		ordered list in which object you extract is the minimum    *
*		respect a user defined ordering. Obviously in an ordered   *
*		list you must supply the compareobject function.	   *
*									   *
*   10/Nov/92	Added CIRCULAR. 					   *
*		A CircularList is a List with joined Head and Tail.	   *
*		Insertion and extraction are with respect of CurrentList   *
*		pointer. We insert at CurrList, moving it next, and we	   *
*		extract it, moving the next to its place.		   *
*									   *
***************************************************************************/

enum TypeListTag
{
 LIFO,
 FIFO,
 CIRCULAR,
 ORDERED
};

typedef enum TypeListTag TypeList;

/***************************************************************************
*									   *
*    TYPE:	pointer							   *
*									   *
* PURPOSE:	Representing a generic pointer to an object.		   *
*									   *
*   NOTES:	An pointer is a pointer to a generic object!		   *
*									   *
***************************************************************************/
typedef void * pointer;



/***************************************************************************
*									   *
*    TYPE:	ListElem						   *
*									   *
* PURPOSE:	Representing a generic element of a double chained list.   *
*									   *
*   NOTES:	This element can contain any object. To implement a	   *
*		Object Oriented list the structure of elem must not be	   *
*		known by the user.					   *
*									   *
***************************************************************************/
typedef struct Elemtag
{
 struct Elemtag *next;
 struct Elemtag *prev;
 pointer obj;
} ListElem;


/***************************************************************************
*									   *
*    TYPE:	BuckElem						   *
*									   *
* PURPOSE:	Representing a generic element of a hash bucket.	   *
*									   *
*   NOTES:	The hash index contains referemces to ListElem, so each    *
*		BuckElem must contain a ListElem.			   *
*									   *
***************************************************************************/
typedef struct BuckElemtag
{
 struct BuckElemtag *next;
 struct BuckElemtag *prev;
 ListElem *le;
} BuckElem;



/***************************************************************************
*									   *
*    TYPE:	Hash							   *
*									   *
* PURPOSE:	Mantaining all needed information for hashing a list.	   *
*									   *
*   NOTES:	For assuring fast retrieving of an object in a list we use *
*		an hash index over the list. Such index contains references*
*		to ListElem of the lists. Such references are contained in *
*		Buckets arranged in a vector of BuckElem.		   *
*									   *
*		H is a vector of size BuckElem*; if H[i]==NULL such bucket *
*			is empty.					   *
*		HashKey is the user supplied coding function; it get an    *
*			object and returns an integer.			   *
*									   *
*									   *
***************************************************************************/
typedef struct Hashtag
{
 BuckElem **H;
 int size;
 int (*HashKey)(pointer);
} Hash;


/***************************************************************************
*									   *
*    TYPE:	List							   *
*									   *
* PURPOSE:	Representing a list.					   *
*									   *
*   NOTES:	This is a list of a generic objects. A user before using a *
*		List must invoke MakeNewList to create a new list.	   *
*		User must accede to List only with the functions in list.c.*
*		Note that List is a pointer! But user must not knows.	   *
*									   *
*	      - nobject is the number of object actually containd in List  *
*	      - objectsize is sizeof the object whose pointers are	   *
*			contained in the list (it is not the size of	   *
*			pointer!!)					   *
*	      -	H point to the first (Head) element of a list.		   *
*	      -	T point to the last  (Tail) element of a list.		   *
*	      -	C point to the current element of a list scan.		   *
*	      -	EqualObj is a function returning TRUE on equal	arguments  *
*	      -	CompareObj is a function returning:			   *
*		    < 0	   <elem1> less than <elem2>			   *
*		    = 0	   <elem1> equivalent to <elem2>		   *
*		    > 0    <elem1> greater than <elem2> 		   *
*	      - Hash is the pointer to Hash struct containing infroamation *
*			for hashing the list. It is not NULL only if list  *
*			is hashed					   *
*									   *
*									   *
***************************************************************************/

struct Listtag
{
 int objectsize;
 int nobject;
 ListElem *H;
 ListElem *T;
 ListElem *C;
 TypeList type;
 boolean (*EqualObj)(void *, void *);
 int (*CompareObj)(void *, void *);
 Hash *hash;
};


typedef struct Listtag *List;




#endif		/* this #endif is the brother of #ifndef OLIST_H.*/
		/* If OLIST_H was already defined all this file  */
		/* must be skipped.                              */


#ifndef LISTPROT_H	/* LISTPROT_H is defined only in listprot.h */
#include "listprot.h"	/* With this include an user can include in */
#endif			/* his files only olist.h		    */
