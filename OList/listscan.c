/***************************************************************************
******************************* 07/Sep/92 **********************************
****************************   Version 0.5   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	listscan.c						   *
*                                                                          *
* PURPOSE:	Implementing functions for scanning object in lists.	   *
*                                                                          *
*                                                                          *
* EXPORTS:	HeadList,	Reset the Current Indicator object to the  *
*		TailList	Head (Tail) of the list.		   *
*                                                                          *
*		CurrList,	Get the object in the Current Indicator    *
*				of list.				   *
*                                                                          *
*		NextList,	Scan all the elements of a list without    *
*		PrevList	destroying or modifying them.		   *
*                                                                          *
*		PushCurrList,	Store and retrieve in a stack the Current  *
*		PopCurrList	Indicator of a list			   *
*                                                                          *
*                                                                          *
* IMPORTS:	NewList, InsertList, ExtractList			   *
*                                                                          *
* GLOBALS:	CurrListStack						   *
*                                                                          *
*   NOTES:	All functions in this files make a heavy use of the	   *
*		Current Indicator of a list. The Current Indicator is a    *
*		ListElem pointer used to hold a particularly interesting   *
*		object.							   *
*									   *
****************************************************************************
***************************************************************************/

#include <stdlib.h>
#include <OList/general.h>
#include <OList/error.h>
#include <OList/olist.h>


/***************************************************************************
*  GLOBAL:	CurrListStack						   *
*									   *
* PURPOSE:	This global variable is used to mantain a stack of current *
*		of list to permit scanning of list without modifying the   *
*		current of a list.					   *
*									   *
***************************************************************************/
List CurrListStack=NULL;



/***************************************************************************
*									   *
* FUNCTION:	HeadList, TailList					   *
*									   *
*  PURPOSE:	Reset the current object to the Head (Tail) of the list.   *
*									   *
*   PARAMS:	The list;						   *
*									   *
*   RETURN:	TRUE if successful.					   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	It set the Current ListElem  pointer to the value of	   *
*		l->H (T).						   *
*		This function must be used before scanning a list with the *
*		NextElemList PrevElemList function.			   *
*									   *
***************************************************************************/

boolean HeadList(List l)
{
 l->C=l->H;

 return TRUE;
}

boolean TailList(List l)
{
 l->C=l->T;

 return TRUE;
}


/***************************************************************************
*									   *
* FUNCTION:	CurrList						   *
*									   *
*  PURPOSE:	Get the object in the current indicator of list.	   *
*									   *
*   PARAMS:	The list and a pointer where put the pointer find;	   *
*									   *
*   RETURN:	TRUE if current indicator of list is not NULL.		   *
*		FALSE else						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	Note that:						   *
*									   *
*		CurrList(&obj0, l);					   *
*		NextList(&obj1, l);					   *
*									   *
*		then obj0 is equal to obj1, but:			   *
*									   *
*		NextList(&obj0, l);					   *
*		CurrList(&obj1, l);					   *
*									   *
*		then obj0 is not equal to obj1. 			   *
*		Here are two examples of using CurrList for List scanning: *
*									   *
*		TailList(l)						   *
*		while(CurrList(&obj,l))					   *
*		   {							   *
*		    MakeSomethingListElem(obj);				   *
*		    NextList(&nocare, l);				   *
*		   }							   *
*									   *
*		for(TailList(l); CurrList(&obj,l); NextList(&nocare, l))   *
*		    MakeSomethingListElem(obj);				   *
*									   *
*		The user is suggested to use only NextList for scanning.   *
*									   *
***************************************************************************/
boolean CurrList(pointer Object, List l)
/* boolean CurrList(pointer *object, List l) Modified to avoid a warning */
{
 pointer *object=(pointer *)Object;
 if(l->C)
     {
      *object = l->C->obj;
      return TRUE;
     }
 else return FALSE;
}


/***************************************************************************
*                                                                          *
* FUNCTION:	NextList, PrevList					   *
*                                                                          *
*  PURPOSE:     Scan all the elements of a list without destroying them.   *
*                                                                          *
*   PARAMS:     The list, and a pointer where put the pointer find;        *
*                                                                          *
*   RETURN:     TRUE if successful.                                        *
*               FALSE if list ends.                                        *
*                                                                          *
*  IMPORTS:     None                                                       *
*                                                                          *
*    NOTES:     It use the ListElem pointer C to mantain the position      *
*               inside a list.                                             *
*               Its name is a little uncorrect because it returns the      *
*               current element instead of the next (prev) element, but    *
*               also increment that pointer, from that the name.           *
*               To remember                                                *
*               Next move from tail to head                                *
*               Prev move from head to tail                                *
*                                                                          *
*               Examples:                                                  *
*                                                                          *
*               TailList(l);                                               *
*		while(NextList(&obj,l)) MakeSomething(obj);		   *
*                                                                          *
*		for(TailList(l);NextList(&obj,l);) MakeSomething(obj);	   *
*                                                                          *
*               HeadList(l);                                               *
*		while(PrevList(&obj,l)) MakeSomething(obj);		   *
*                                                                          *
*		for(HeadList(l);PrevList(&obj,l);) MakeSomething(obj);	   *
*                                                                          *
*		If you want to use instead of an object the current	   *
*		indicator l->C of the list (as we do in hash functions)    *
*		you must remember that after NextList(&obj,l) l->C points  *
*		to the ListElem after the one containing obj so you must   *
*		do NextList as last thing.				   *
*                                                                          *
*		TailList(l)						   *
*		while(CurrList(&obj,l))					   *
*		   {							   *
*		    MakeSomethingListElem(obj);				   *
*		    NextList(nocare, l);				   *
*		   }							   *
*									   *
*		for(TailList(l); CurrList(&obj,l); NextList(nocare, l))	   *
*		    MakeSomethingListElem(obj);				   *
*									   *
***************************************************************************/

boolean NextList(pointer Object, List l)
{
 pointer *object=(pointer *)Object;
 if(l->C==NULL) return FALSE;

 *object=l->C->obj;
 l->C=l->C->next;

 return TRUE;
}

boolean PrevList(pointer Object, List l)
{
 pointer *object=(pointer *)Object;
 if(l->C==NULL) return FALSE;

 *object=l->C->obj;
 l->C=l->C->prev;

 return TRUE;
}


/***************************************************************************
*                                                                          *
* FUNCTION:	PushCurrList, PopCurrList				   *
*                                                                          *
*  PURPOSE:	Store and retrieve in a stack the Current of a list	   *
*                                                                          *
*   PARAMS:	The list						   *
*                                                                          *
*   RETURN:     TRUE if Success                                            *
*               FALSE else                                                 *
*                                                                          *
*  IMPORTS:	The global CurrListStack				   *
*                                                                          *
*    NOTES:	This function is used to leave unchanged the current	   *
*		indicator of a list. Infact many functions explicitely	   *
*		modify that indicator. This function provide a tool for    *
*		overiding this problem. 				   *
*		For a safe use any function before exiting must pop all it *
*		pushed!!						   *
*		E.g. for analyzing a list without changing the current of  *
*		the list:						   *
*									   *
*		PushCurrList(l);					   *
*		TailList(l);						   *
*		while(NextList(l,&obj)) MakeSomething(obj);		   *
*		PopCurrList(l);						   *
*									   *
***************************************************************************/
boolean PushCurrList(List l)
{
 if(!CurrListStack) CurrListStack=NewList(LIFO,sizeof(ListElem *));

 return InsertList(l->C, CurrListStack);
}

boolean PopCurrList(List l)
{
 if(!CurrListStack) Error("PopCurrList, pop without push!\n", NO_EXIT);

 ExtractList((pointer)&(l->C), CurrListStack);
 return TRUE;
}
