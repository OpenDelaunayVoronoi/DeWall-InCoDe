/***************************************************************************
******************************* 10/Nov/93 **********************************
****************************   Version 0.9   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:      listobj.c                                                  *
*                                                                          *
* PURPOSE:      Implementing functions that manipulates the objects        *
*               contained in list.                                         *
*                                                                          *
*                                                                          *
* EXPORTS:	EqualObject  Test whether two list objects are equal.	   *
*		InsertList   Insert an element in a list.		   *
*		ExtractList  Extract an element in a list.		   *
*		MemberList   Check and find if an element belong to a list.*
*	     DeleteCurrList  Delete the current element from a list.	   *
*		DeleteList   Delete an element from a list.		   *
*		FindLessObj  Find the element in a list that is less	   *
*			     than passed Object.			   *
*                                                                          *
*                                                                          *
*                                                                          *
* IMPORTS:                                                                 *
*                                                                          *
* GLOBALS:								   *
*                                                                          *
*   NOTES:								   *
*									   *
****************************************************************************
***************************************************************************/

#include <stdlib.h>
#include <OList/general.h>
#include <OList/error.h>
#include <OList/olist.h>
#include <OList/listprot.h>


/***************************************************************************
*                                                                          *
* FUNCTION:     EqualObject                                                *
*                                                                          *
*  PURPOSE:     Test whether two objects are equal.                        *
*                                                                          *
*   PARAMS:     The pointers to objects and their size (all be equal).     *
*                                                                          *
*   RETURN:     TRUE if they are equal                                     *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     If the user has not supplied a EqualObject function we     *
*               test if two objects are equals we test them word by word.  *
*                                                                          *
***************************************************************************/
boolean EqualObject(pointer h1, pointer h2, List l)
{
 int i;
 char *v1, *v2;

 if(l->EqualObj) return l->EqualObj(h1,h2);

 v1=(char *)h1;
 v2=(char *)h2;

 for(i=0;i<l->objectsize;i++)
    if(v1[i]!=v2[i]) return FALSE;

 return TRUE;
}

/***************************************************************************
*                                                                          *
* FUNCTION:	Eq_Ref							   *
*                                                                          *
*  PURPOSE:	Test whether two objects are the same pointer.		   *
*                                                                          *
*   PARAMS:	The pointers to objects;				   *
*                                                                          *
*   RETURN:     TRUE if they are equal                                     *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:	This function is one of the two for object comparing.	   *
*                                                                          *
***************************************************************************/
boolean Eq_Ref(pointer h1, pointer h2)
{
 if(h1==h2) return TRUE;
      else return FALSE;
}

/***************************************************************************
*                                                                          *
* FUNCTION:     InsertList                                                 *
*                                                                          *
*  PURPOSE:     Insert an element in a list.                               *
*                                                                          *
*   PARAMS:     The list and the pointer of the object to insert           *
*                                                                          *
*   RETURN:     TRUE if Success                                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     As already stated List contains only pointers,             *
*               not real object.                                           *
*               The insert algorithm is the same for FIFO and LIFO         *
*               strategy, because in both cases we insert in Tail          *
*               In the ORDEDED case we must find where in the list         *
*               insert the new element.                                    *
*  10/Nov/92	In a CIRCULAR list we insert before l->C, that is between  *
*		l->C and l->C->prev and move l->C to the new element, so   *
*		inserting and extractig are LIFO like.			   *
*		In a CIRCULAR list Tail and Head point to two object	   *
*		such that Head->next == Tail and Tail->prev = Head.	   *
*		If we go from Tail to Head, next by next we scan the	   *
*		whole list						   *
*		If we go from Head to Tail, prev by prev we scan the	   *
*		whole list						   *
*		If nothing is deleted Head is the oldest element and Tail  *
*		the newest;						   *
*		In a CIRCULAR list l->C Must never be NULL, except when    *
*		List is empty.						   *
*                                                                          *
***************************************************************************/

boolean InsertList(pointer object, List l)
{
 ListElem *t;
 ListElem *oldT;

 t=(ListElem *)malloc(sizeof(ListElem));
 if(!t) {
	 Error("InsertList, unable to allocate ListElem\n", NO_EXIT);
	 return FALSE;
	}
 t->obj=object;
 if(l->hash) InsertHash(t, l->hash);
 switch(l->type)
  {
   case ORDERED:
		FindLessObj(object,l);
                t->next=l->C;

                if(l->C)                  /* insert before l->C */
                  {
                    t->prev=l->C->prev;

                    if(l->C->prev)  l->C->prev->next=t;  /* normal case */
                               else l->T=t;              /* l->C was Tail */
                    l->C->prev=t;
                  }
                else                      /* insert in Head */
                 {
                   t->prev=l->H;
                   if(l->H) l->H->next=t; /* list not empty */
                       else l->T=t;       /* empty list, modify Tail */
                   l->H=t;
                  }

                t->next=l->C;


		break;
   case CIRCULAR:
		if(l->C)		  /* insert before l->C */
		 {
		   t->next=l->C;	  /* normal case */

		   t->prev=l->C->prev;
		   l->C->prev->next=t;

		   l->C->prev=t;

		   l->T=l->H->next;	  /* Adjust Head and Tail.	 */

		   l->C=t;		  /* Move l->C to Last inserted  */
		 }
		else if(l->nobject==0)	  /* List was empty */
		 {
		   l->C=t;
		   t->next=t;
		   t->prev=t;
		   l->H=t;
		   l->T=t;
		 }
		else		  /* l->C==NULL and Not Empty List */
		ErrorFALSE("InsertList, NULL CurrList in a CIRCULAR list.\n")

		break;
   case FIFO:
   case LIFO:   oldT=l->T;
                l->T=t;
                l->T->next=oldT;
                l->T->prev=NULL;
                if(oldT==NULL) l->H=t; /* If empty list init head of list */
                          else oldT->prev=t;
  }


 l->nobject++;

 return TRUE;
}



/***************************************************************************
*                                                                          *
* FUNCTION:     ExtractList                                                *
*                                                                          *
*  PURPOSE:     Extract an element in a list.                              *
*                                                                          *
*   PARAMS:     The list and the pointer for the object to Extract         *
*                                                                          *
*   RETURN:     TRUE if Success                                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     As already stated List contains only pointers,             *
*               not real object, so we can free everything.                *
*               FIFO we extract from Head.                                 *
*               LIFO we extract from Tail.                                 *
*               ORDERED we extract the minimum, that is from Tail.         *
*               If the list contains one element after extracting          *
*               both the Head and Tail are NULL                            *                       *
*                                                                          *
*		Remember that for each extract you must update the HASH!!! *
*   10/Nov/92	Extracting from CIRCULAR list: we extract the L->C Element.*
*		l->C, l->H and l->T are NULL only if List is empty;	   *
*                                                                          *
***************************************************************************/

boolean ExtractList(pointer Object, List l)
{
pointer *object=(pointer *)Object;
 ListElem *elemtofree;

 if(l->nobject==0) return FALSE;	/* Empty list */


 switch(l->type)
  {
   case FIFO:   *object=l->H->obj;          /* object to give back */
                elemtofree=l->H;
                l->H = l->H->prev;
                if(l->H) l->H->next = NULL; /* Was not last obj */
                    else l->T=NULL;         /* Was the last obj */
		break;
   case CIRCULAR:
		if(l->C==NULL)		  /* l->C==NULL and Not Empty List */
		ErrorFALSE("ExtractList, NULL CurrList in a CIRCULAR list.\n")

		*object=l->C->obj;	    /* object to give back */
		elemtofree=l->C;
		if(l->nobject==1)
		 {
		  l->C=NULL;
		  l->T=NULL;
		  l->H=NULL;
		 }
		else
		 {
		  l->C->prev->next=l->C->next;
		  l->C->next->prev=l->C->prev;

		  if(l->C==l->H) l->H=l->H->prev;
		  if(l->C==l->T) l->T=l->T->next;

		  l->C=l->C->next;
		 }
		break;
   case ORDERED:

   case LIFO:
   default:     *object=l->T->obj;          /* object to give back */
                elemtofree=l->T;
                l->T = l->T->next;
                if(l->T) l->T->prev = NULL; /* Was not last obj */
                    else l->H = NULL;       /* Was the last obj */
  }

 l->nobject--;

 if(l->hash) if(!DeleteHash(*object, l))
	{
	 Error("ExtractList, extracted object was not in hash!\n",NO_EXIT);
	 return FALSE;
	}
 /* the following line was modified on 10 Nov 1993; previously I freed
    elemtofree before the DeleteHash; was wrong but worked on every machine /*
 free(elemtofree);	    /* free the memory of old list element */
 return TRUE;
}


/***************************************************************************
*                                                                          *
* FUNCTION:     MemberList                                                 *
*                                                                          *
*  PURPOSE:     Check and find if an element belong to a list.             *
*                                                                          *
*   PARAMS:     The list and the pointer of the object to find;            *
*                                                                          *
*   RETURN:     TRUE if object belongs to list;                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     This function neither alloc nor free anything, only check  *
*               for the presence of the object in a list.                  *
*               If the object is found the l->C points to the element      *
*		found, else it is not changed.				   *
*               Current implementation scan the whole list.                *
*                                                                          *
*		The newer version use scanlist functions. Note that	   *
*		NextList put in l->C the ListElem next to the one	   *
*		containing the object returned, so you must take a step	   *
*		back and if you reach end get the tail.			   *
*                                                                          *
*   28/Oct/92	Note that version before 28 oct used to modify CurrList    *
*		in any case. Last version modify it only if it find it.    *
*                                                                          *
*   10/Nov/92	Modified for tracting CIRCULAR lists.			   *
*                                                                          *
*                                                                          *
***************************************************************************/

boolean oldoldMemberList(pointer object, List l)
{
 ListElem *curr;

 if(!(l->T))
  {
   l->C=NULL;
   return FALSE;        /* Empty list */
  }
 curr=l->T;

 while(curr)
  {
   if(EqualObject(object, curr->obj, l))
    {
     l->C=curr;
     return TRUE;
    }
   curr=curr->next;
  }

 l->C=NULL;
 return FALSE;
}

boolean oldMemberList(pointer object, List l)
{
 pointer scanobj;
 ListElem *TempCurrList=l->C;

 if(l->hash) return MemberHash(object, l);

 TailList(l);

 while(NextList(&scanobj, l))
   if(EqualObject(object, scanobj, l))
   {
    if(!PrevList(&scanobj, l)) HeadList(l);
    return TRUE;
   }

 l->C=TempCurrList;
 return FALSE;
}

boolean MemberList(pointer object, List l)     /* 10/Nov/92 */
{
 pointer scanobj;
 ListElem *TempCurrList=l->C;
 int i,n;

 if(l->hash) return MemberHash(object, l);

 TailList(l);
 n=CountList(l);
 for(i=0;i<n;i++)
  {
   NextList(&scanobj, l);
   if(EqualObject(object, scanobj, l))
   {
    if(!PrevList(&scanobj, l)) HeadList(l);    /* Take a step back for */
    return TRUE;			       /* right CurrList       */
   }
  }
 l->C=TempCurrList;
 return FALSE;
}

/***************************************************************************
*                                                                          *
* FUNCTION:	DeleteCurrList						   *
*                                                                          *
*  PURPOSE:     Delete the current element from a list.                    *
*                                                                          *
*   PARAMS:     The list                                                   *
*                                                                          *
*   RETURN:     TRUE if Success                                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     This function is not different for LIFO and FIFO.          *
*               As already stated List contains only pointers,             *
*               not real object, so we free only the memory of the list    *
*               element, the object that it contained remains in memory    *
*               and its pointer is still valid.                            *
*               Special cases:                                             *
*                       Empty list,                                        *
*                       object to is the only one                          *
*                                                                          *
*  10/Nov/92	Changed to manage CIRCULAR lists.			   *
*		In a CIRCULAR List CurrList after deleting point to next   *
*		element.						   *
*                                                                          *
***************************************************************************/

boolean DeleteCurrList(List l)
{
 ListElem *ElemToFree=l->C;

 if(l->nobject==0)
   {
    Error("DeleteCurrList, Trying to delete in a empty list\n",NO_EXIT);
    return FALSE;	      /* Empty list */
   }
 if(l->C==NULL)
   {
    Error("DeleteCurrList, Trying to delete a NULL current of List\n",NO_EXIT);
    return FALSE;		 /* Unsetted Current element l->C   */
   }
 if(l->nobject==1)                      /* One element List, the list beco-*/
  {                                     /* me empty and all pointers NULL  */
    l->T=NULL;
    l->H=NULL;
  }

 if(l->hash) if(!DeleteHash(l->C->obj, l))
	{
	 Error("DeleteCurrList, L->C was not in hash\n",NO_EXIT);
	 return FALSE;
	}

 if(l->H==l->C) l->H=l->C->prev;
 if(l->T==l->C) l->T=l->C->next;

 if(l->C->next) l->C->next->prev=l->C->prev;
 if(l->C->prev) l->C->prev->next=l->C->next;


 if(l->type == CIRCULAR && (l->nobject > 1)) l->C=l->C->next;
					else l->C=NULL;


 free(ElemToFree);
 l->nobject--;
 return TRUE;
}


/***************************************************************************
*                                                                          *
* FUNCTION:     DeleteList                                                 *
*                                                                          *
*  PURPOSE:     Delete an element from a list.                             *
*                                                                          *
*   PARAMS:     The list and the pointer of the object to delete           *
*                                                                          *
*   RETURN:     TRUE if Success                                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     This function simply test for the presence in the list of  *
*               the element and delete it.                                 *
*                                                                          *
*   10/nov/92	Nasty Bug:						   *
*		If before delete CurrList points to deleting element we    *
*		mantain in l->C freed memory pointer.			   *
*		MemberList put in l->C the found element;		   *
*		DeleteCurrList set l->C=NULL or l->C=l->C->next for	   *
*		CIRCULAR lists						   *
*		So the only dangerous case is when DeleteCurrList set	   *
*		l->C=l->C->next.					   *
*                                                                          *
***************************************************************************/

boolean DeleteList(pointer object, List l)
{
 boolean result=FALSE;
 ListElem *OldCurr=l->C;
 ListElem *OldCurrNext=NULL;

 if(l->C) OldCurrNext = l->C->next;

 if(MemberList(object,l)) result=DeleteCurrList(l);
 else result=FALSE;

 if(OldCurrNext!=l->C) l->C=OldCurr;

 return result;
}



/***************************************************************************
*                                                                          *
* FUNCTION:     FindLessObj                                                *
*                                                                          *
*  PURPOSE:     Find the element in a list that is less than Object        *
*                                                                          *
*   PARAMS:     The list and the pointer of the object to compare          *
*                                                                          *
*   RETURN:     TRUE if Success                                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     This function is used to build a list than is increasingly *
*               ordered from tail to head (tail <= head).                  *
*               The object found pointer is put in l->C;                   *
*               l->C contains an object not less than obj, l->C->prev is   *
*               less or equal than object and l->C->next, if list is       *
*               ordered, is supposed to be not less than obj.              *
*               In this version we must scan the whole list.               *
*               To mantain list ordered you must insert object before l->C.*
*               If l->C==NULL you must insert object at head.              *
*                                                                          *
***************************************************************************/

boolean FindLessObj(pointer object, List l)
{
 if(l->type!=ORDERED)
    ErrorFALSE("FindLessObj used in a not ORDERED list.\n");
 if(l->CompareObj==NULL)
    ErrorFALSE("FindLessObj, uninitialized CompareObj function.");

 l->C=l->T;
 while(l->C)
 {
  if(l->CompareObj(object,l->C->obj) <= 0) break;
  l->C=l->C->next;
 }
 return TRUE;
}
