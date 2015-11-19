/***************************************************************************
******************************* 09/Jan/93 **********************************
****************************   Version 0.9   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	list.c							   *
*									   *
* PURPOSE:	Implementing functions that manipulates lists globally.	   *
*									   *
*									   *
* EXPORTS:	NewList	      Creating a new instance of a list.	   *
*		NewHashList   Creating a new instance of a list, hashing it*
*									   *
*		ChangeEqualObjectList	Changing the EqualObject function. *
*		ChangeCompareObjectList	Changing the CompareObject function*
*									   *
*		EqualList     Checking if two lists are equal.		   *
*		CountList     Counting the number of elements in a list.   *
*		IsEmptyList   Checking if a list is empty.		   *
*									   *
*		EraseList     Erasing an entire List freeing all the memory*
*		CopyList      Creating a Copy of an entire List.	   *
*									   *
*		UnionList     Creating the list union of two lists.	   *
*		IntersectList Creating the list intersection of two lists. *
*		AppendList    Appending a list to another one.		   *
*		*DiffList						   *
*		*FusionList						   *
*									   *
* IMPORTS:								   *
*									   *
* GLOBALS:								   *
*									   *
*   NOTES:								   *
*									   *
*    TODO:	Controllare che ogni volta che si usa la tail, dove serve, *
*		si faccia un push-pop del CurrList.			   *
*									   *
****************************************************************************
***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <OList/general.h>
#include <OList/olist.h>
#include <OList/error.h>




/***************************************************************************
*									   *
* FUNCTION:	NewList							   *
*									   *
*  PURPOSE:	Create a new instance of a list.			   *
*									   *
*   PARAMS:	The type of list and the size of the object		   *
*		to put in the list.					   *
*									   *
*   RETURN:	The created List if succesful,				   *
*		NULL else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	Before using a list you must use this function!!	   *
*		The parameter size is actually only to test the equalness  *
*		of two objects. 					   *
*		The type can be:					   *
*		FIFO, LIFO,						   *
*		CIRCULAR,						   *
*		ORDERED							   *
*									   *
***************************************************************************/

List NewList(TypeList type, int size)
{
 List l;

 l=(List)malloc(sizeof(struct Listtag));
 if(!l) ErrorNULL("InsertList, unable to allocate ListElem\n");

 l->objectsize=size;
 l->nobject=0;
 l->H=NULL;
 l->T=NULL;
 l->C=NULL;
 l->EqualObj=NULL;
 l->CompareObj=NULL;
 l->type=type;
 l->hash=NULL;
 return l;
}

/***************************************************************************
*									   *
* FUNCTION:	NewHashList						   *
*									   *
*  PURPOSE:	Create a new instance of a list hashing it and setting the *
*		EqualObjectFunction.					   *
*									   *
*   PARAMS:	The type of list and the size of the object		   *
*		to put in the list, EqualObject function, hash size, and   *
*		the hash function.					   *
*									   *
*   RETURN:	The created List if successful,				   *
*		NULL if creation goes wrong.				   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	This function simply call the NewList, ChangeEqualObject   *
*		and HashList Function.					   *
*		Its purpose is shorting the code.			   *
*		If HashCreation Fails this function says nothing.	   *
*									   *
***************************************************************************/

List NewHashList(TypeList type, int size,
		int (* comp)(void *elem1, void *elem2),
		int hashsize, int (*HashKey)(void *)   )
{
 List T=NULL_LIST;

 T=NewList(type,size);
 if(!T) return NULL_LIST;

 ChangeEqualObjectList(comp,T);
 HashList(hashsize,HashKey,T);

 return T;
}


/***************************************************************************
*									   *
* FUNCTION:	ChangeEqualObjectList					   *
*									   *
*  PURPOSE:	To change the EqualObject function.			   *
*									   *
*   PARAMS:	A list and a pointer to a boolean function accepting	   *
*		two object pointers					   *
*									   *
*   RETURN:	TRUE if Success 					   *
*		FALSE else						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	The function passed must accept two object of the same	   *
*		type of list ones.					   *
*									   *
*									   *
***************************************************************************/

boolean ChangeEqualObjectList(boolean (* eq)(void *elem1,
					       void *elem2), List l)
{
 if(!l) ErrorFALSE("ChangeEqualObjectList, not initialized List.\n");
 l->EqualObj=eq;
 return TRUE;
}



/***************************************************************************
*									   *
* FUNCTION:	ChangeCompareObjectList					   *
*									   *
*  PURPOSE:	To change the CompareObject function.			   *
*									   *
*   PARAMS:	A list and a pointer to a int function accepting	   *
*		two object pointers					   *
*									   *
*   RETURN:	TRUE if Success 					   *
*		FALSE else						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	The function passed must accept two object of the same	   *
*		type of list ones.					   *
*		It must return: 					   *
*		< 0	   <elem1> less than <elem2>			   *
*		= 0	  <elem1> equivalent to <elem2> 		   *
*		> 0	  <elem1> greater than <elem2>			   *
*									   *
*									   *
***************************************************************************/

boolean ChangeCompareObjectList(int (* comp)(void *elem1,
					       void *elem2), List l)
{
 if(!l) ErrorFALSE("ChangeCompareObjectList, not initialized List.\n");

 l->CompareObj=comp;
 return TRUE;
}



/***************************************************************************
*									   *
* FUNCTION:	EqualList						   *
*									   *
*  PURPOSE:	Checking if two list are equals.			   *
*									   *
*   PARAMS:	The two lists;						   *
*									   *
*   RETURN:	TRUE if the two lists are equal;			   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	EqualObject						   *
*									   *
*    NOTES:	Two lists are equal if they contain same objects in the    *
*		same order.It is supposed that the two lists contain	   *
*		object of same type and the compare function is the same.  *
*		This function doesnt modify current element indicator	   *
*		in the lists.						   *
*  10/11/92	Changed while loop to for to manage Circular Lists.	   *
*									   *
***************************************************************************/

boolean EqualList(List l0, List l1)
{
 ListElem *e0,*e1;
 int i,n;
 if(!l0 || !l1) ErrorFALSE("EqualList, not initialized List.\n");

 if(l0->nobject!=l1->nobject) return FALSE;
 if(l0->objectsize!=l1->objectsize) return FALSE;

 e0=l0->T;
 e1=l1->T;
 n=l0->nobject;
 for(i=0;i<n;i++)
  {
   if(!EqualObject(e0->obj, e1->obj, l0)) return FALSE;
   e0=e0->next;
   e1=e1->next;
  }
 return TRUE;
 }


/***************************************************************************
*									   *
* FUNCTION:	CountList						   *
*									   *
*  PURPOSE:	Counting the number of elements of a list.		   *
*									   *
*   PARAMS:	The list;						   *
*									   *
*   RETURN:	The nubmer of elements in a list.			   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	Use the nobject field in List structure.		   *
*									   *
***************************************************************************/

int CountList(List l)
{
 if(!l) ErrorZERO("CountList, not initialized List.\n");
 return l->nobject;
}


/***************************************************************************
*									   *
* FUNCTION:	IsEmptyList						   *
*									   *
*  PURPOSE:	Checking if a list is empty.				   *
*									   *
*   PARAMS:	The list;						   *
*									   *
*   RETURN:	TRUE if the list is empty.				   *
*		FALSE else						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	Use the CountList function.				   *
*									   *
***************************************************************************/

int IsEmptyList(List l)
{
 if(!l) ErrorTRUE("IsEmptyList, not initialized List.\n");
 if (CountList(l)==0) return TRUE;
    else return FALSE;
}


/***************************************************************************
*									   *
* FUNCTION:	EraseList						   *
*									   *
*  PURPOSE:	Erase an entire List freeing all the memory.		   *
*									   *
*   PARAMS:	The list;						   *
*									   *
*   RETURN:	TRUE if successful.					   *
*									   *
*  IMPORTS:	ExtractList, EraseHash					   *
*									   *
*    NOTES:	All the elements of the list and the List structure are    *
*		freed. Because a List contains only pointers the objects   *
*		it contained retain in memory.				   *
*									   *
***************************************************************************/
boolean EraseList(List l)
{
 pointer nocare;

 if(!l) ErrorTRUE("EraseList, not initialized List.\n");

 while(ExtractList(&nocare,l));

 if(l->hash) EraseHashList(l);

 free(l);

 return TRUE;
}


/***************************************************************************
*									   *
* FUNCTION:	CopyList						   *
*									   *
*  PURPOSE:	Create a Copy of an entire List.			   *
*									   *
*   PARAMS:	The list to copy;					   *
*									   *
*   RETURN:	A new list containing the copy if successful.		   *
*		NULL else.						   *
*									   *
*  IMPORTS:	TailList, NextList, InsertList				   *
*									   *
*    NOTES:	Because Lists contains only pointers only them are	   *
*		duplicated! The objects they refers are not copied.	   *
*									   *
*		In LIFO and FIFO lists in head are the older elements, so  *
*		if we insert in NewL from head to tail the object order    *
*		is preserved.						   *
*									   *
*   10/11/92	Added  for(i=0;... to manage circular Lists		   *
*									   *
***************************************************************************/
List CopyList(List l)
{
 List NewL;
 pointer object;

 if(!l) ErrorNULL("CopyList, not initialized List.\n");

 NewL=(List)malloc(sizeof(struct Listtag));
 NewL->objectsize=l->objectsize;
 NewL->nobject=0;
 NewL->H=NULL;
 NewL->T=NULL;
 NewL->C=NULL;
 NewL->EqualObj=l->EqualObj;
 NewL->CompareObj=l->CompareObj;
 NewL->type=l->type;
 NewL->hash=NULL;

 if(l->type!=CIRCULAR)
  {
   PushCurrList(l);

   for(HeadList(l);PrevList(&object,l);)
      InsertList(object, NewL);

   PopCurrList(l);
  }
 else
  {
   int i,n;
   n=l->nobject;
   PushCurrList(l);
   HeadList(l);
   for(i=0;i<n;i++)
    {
     PrevList(&object,l);
     InsertList(object, NewL);
    }

   PopCurrList(l);
  }
 return NewL;
}


/***************************************************************************
*									   *
* FUNCTION:	UnionList						   *
*									   *
*  PURPOSE:	Create the list union of two lists.			   *
*									   *
*   PARAMS:	The two lists to unite;					   *
*									   *
*   RETURN:	A new list containing the union if successful.		   *
*		NULL else.						   *
*									   *
*  IMPORTS:	CopyList, TailList, NextList, MemberList, InsertList	   *
*									   *
*    NOTES:	This function does not modify the two lists passed,	   *
*		it create a new list. It is assumed that the two lists	   *
*		contains object of the same type.			   *
*									   *
***************************************************************************/

List UnionList(List L0, List L1)
{
 List UList;
 pointer object;

 if(!L0 || !L1) ErrorNULL("UnionList, not initialized List.\n");

 UList=CopyList(L0);

 TailList(L1);
 while(NextList(&object, L1))
	if(!MemberList(object, UList)) InsertList(object, UList);

 return UList;
}


/***************************************************************************
*									   *
* FUNCTION:	IntersectList						   *
*									   *
*  PURPOSE:	Create the list intersection of two lists.		   *
*									   *
*   PARAMS:	The two lists to intersecate;				   *
*									   *
*   RETURN:	A new list containing the intersection if successful.	   *
*		NULL else.						   *
*									   *
*  IMPORTS:	CopyList, TailList, NextList, MemberList, DeleteList	   *
*									   *
*    NOTES:	This function does not modify the two lists passed,	   *
*		it create a new list. It is assumed that the two lists	   *
*		contains object of the same type.			   *
*		To build intersection we make a copy of L0 and we delete   *
*		from it all the object that does not belongs to L1.	   *
*									   *
***************************************************************************/

List IntersectList(List L0, List L1)
{
 List InterList;
 pointer object;

 if(!L0 || !L1) ErrorNULL("IntersectList, not initialized List.\n");

 InterList=CopyList(L0);

 TailList(InterList);
 while(NextList(&object, InterList))
	if(!MemberList(object, L1)) DeleteList(object, InterList);

 return InterList;
}


/***************************************************************************
*									   *
* FUNCTION:	AppendList						   *
*									   *
*  PURPOSE:	Append a list to another one.				   *
*									   *
*   PARAMS:	The two lists to append.				   *
*									   *
*   RETURN:	The first list modified if successful.			   *
*		NULL else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	This function modify the first of lists passed,	appending  *
*		all the object of second list. It is assumed that the two  *
*		lists contain object of the same type.			   *
*		All the objects in L2 are inserted (if not presents) in L1.*
*									   *
***************************************************************************/
List AppendList(List L1, List L2)
{
 pointer object;

 if(!L1 || !L2) ErrorNULL("AppendList, not initialized List.\n");

 PushCurrList(L2);
 TailList(L2);
 while(NextList(&object, L2))
	if(!MemberList(object, L1)) InsertList(object, L2);
 PopCurrList(L2);
 return L1;
}
