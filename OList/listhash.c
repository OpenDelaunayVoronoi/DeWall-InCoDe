/******************************* 29/Oct/92 **********************************
****************************   Version 0.5   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	listhash.c						   *
*                                                                          *
* PURPOSE:	Implementing functions for hashing object in lists.	   *
*                                                                          *
*                                                                          *
* EXPORTS:	HashList	Create a new instance of a hash index over *
*				an existing list.			   *
*		InsertHash	Add a ListElem referring in the hash.	   *
*		MemberHash	Retrieve a ListElem referring in a	   *
*				hashed list.				   *
*		DeleteHash	Delete a ListElem referring in a	   *
*				hashed list.				   *
*		EraseHashList	Destroy an hash index from a list.	   *
*		ReHashList	Rehash a list changing the hash size.	   *
*                                                                          *
* IMPORTS:								   *
*                                                                          *
* GLOBALS:								   *
*                                                                          *
*   NOTES:	These file Functions are for fast hash indexing elements   *
*		of a list; The hash strategy is a static one. The user must*
*		pass the function to hash and the size of the hash index.  *
*		If an user wants to hash a list L must simply call:	   *
*									   *
*			NewHash(size, hashfunc, L);			   *
*									   *
*		And since then all the accesses to list will be 	   *
*		trasparentely speeded up by an hash index. Simmetrically   *
*		to remove an hash index on a list call: 		   *
*									   *
*			EraseHashList(L);					   *
*									   *
*		If an user want to change the hash size must invoke the    *
*		Rehash function.					   *
*									   *
*									   *
****************************************************************************
***************************************************************************/

#include <stdlib.h>
#include <OList/general.h>
#include <OList/olist.h>
#include <OList/error.h>




/***************************************************************************
*									   *
* FUNCTION:	HashList						   *
*									   *
*  PURPOSE:	Create a new instance of a hash index over a list.	   *
*									   *
*   PARAMS:	the size of the hash vector and the function HashKey.	   *
*									   *
*   RETURN:	TRUE if succesful,					   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	This function is used to build the hash index over a given *
*		list; once initialized the hash struct with an index	   *
*		composed by n BuckElem we scan all the list and insert its *
*		elements in the hash index.				   *
*									   *
*									   *
***************************************************************************/

boolean HashList(int size, int (*HashKey)(void *), List l)
{
 Hash *H;
 pointer nocarepointer;

 if(l->hash) {
	      Error("HashList, Hash index already existing.\n",NO_EXIT);
	      EraseHashList(l);
	     }
 if(size<5) ErrorFALSE("HashList, Hash size is too little to be useful.\n");
 H=(Hash *)malloc(sizeof(Hash));

 if(!H) ErrorFALSE("NewHash, unable to allocate Hash struct.\n");

 H->size=size;
 H->HashKey=HashKey;
 H->H=(BuckElem **)calloc(size,sizeof(BuckElem *));
 if(!H->H) ErrorFALSE("NewHash, unable to allocate Hash vector.\n");
 l->hash=H;

 PushCurrList(l);
 for(TailList(l); l->C; NextList(&nocarepointer, l))
				InsertHash(l->C, l->hash);
 PopCurrList(l);

 return TRUE;
}

/***************************************************************************
*									   *
* FUNCTION:	InsertHash						   *
*									   *
*  PURPOSE:	Add a ListElem referring in the hash.			   *
*									   *
*   PARAMS:	The listElem to add and the hash.			   *
*									   *
*   RETURN:	TRUE if succesful,					   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	The Hash index is structured as a vector of BuckElem	   *
*		pointers. The le field of each of them points to the	   *
*		ListElem of List to index. For each element to add we	   *
*		allocate a BuckElem.					   *
*									   *
***************************************************************************/

boolean InsertHash(ListElem *le, Hash *h)
{
 BuckElem *bucket;

 long ind=abs(h->HashKey(le->obj));

 ind=ind%(h->size);

 bucket=(BuckElem *)malloc(sizeof(BuckElem));
 if(!bucket) {
	 Error("InsertHash, unable to allocate bucket.\n",NO_EXIT);
	 return FALSE;
	}

 bucket->le=le;
 bucket->next=h->H[ind];
 bucket->prev=NULL;
 if(h->H[ind]) h->H[ind]->prev=bucket;
 h->H[ind]=bucket;

 return TRUE;
}

/***************************************************************************
*									   *
* FUNCTION:	MemberHash						   *
*									   *
*  PURPOSE:	Retrieve a ListElem referring in a hashed list.		   *
*									   *
*   PARAMS:	The ListElem to find and the hash.			   *
*									   *
*   RETURN:	TRUE if succesful,					   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	This function use hash index for checking the presence of  *
*		an element in a list. Using the HashKey function we find   *
*		the right bucket, then we scan all its ListElem. Remember  *
*		that a ListElem of a bucket has a ListElem as object (in a *
*		bucket we contains refers to ListElems) this explain that  *
*		we check:						   *
*		EqualObject(obj, ler->obj, l)				   *
*		where ler is the ListElement Referred in bucket.	   *
*									   *
***************************************************************************/

boolean MemberHash(pointer obj, List l)
{
 ListElem *le;
 BuckElem *b;
 long ind;
 ListElem *TempCurrList=l->C;

 if(!l->hash)
    {
     Error("MemberHash, List not hashed!\n",NO_EXIT);
     return FALSE;
    }

 ind=abs(l->hash->HashKey(obj));
 ind=ind%(l->hash->size);

 b=l->hash->H[ind];
 while(b)
  {
   le=b->le;
   if(EqualObject(obj, le->obj, l))
       {
	l->C=le;
	return TRUE;
       }
   b=b->next;
  }
 l->C=TempCurrList;
 return FALSE;
}


/***************************************************************************
*									   *
* FUNCTION:	DeleteHash						   *
*									   *
*  PURPOSE:	Delete a ListElem referring in a hashed list.		   *
*									   *
*   PARAMS:	The object in the ListElem to find and the List.	   *
*									   *
*   RETURN:	TRUE if succesful,					   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:								   *
*									   *
***************************************************************************/

boolean DeleteHash(pointer obj, List l)
{
 ListElem *le;
 BuckElem *b;
 long ind;

 if(!l->hash)
    {
     Error("DeleteHash, List not hashed!\n",NO_EXIT);
     return FALSE;
    }
 ind=abs(l->hash->HashKey(obj));
 ind=ind%(l->hash->size);

 b=l->hash->H[ind];
 while(b)
  {
   le=b->le;
   if(EqualObject(obj, le->obj, l))
       {
	if(b->prev) (b->prev)->next=b->next;
	       else l->hash->H[ind]=b->next;
	if(b->next) (b->next)->prev=b->prev;
	free(b);
	return TRUE;
       }
   b=b->next;
  }

 return FALSE;
}


/***************************************************************************
*									   *
* FUNCTION:	EraseHashList						   *
*									   *
*  PURPOSE:	Destroy an hash index from a list.			   *
*									   *
*   PARAMS:	The hash to destroy.					   *
*									   *
*   RETURN:	TRUE if succesful,					   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	This function frees all the memory allocated for the hash  *
*		index; for each bucket free all the BuckElem allocated in	*
*		it. Then free the whole vector of ListElem pointers.	   *
*									   *
***************************************************************************/
boolean EraseHashList(List l)
{
 int i;
 Hash *h;
 BuckElem *next, *it;
 if(!l->hash)
   {
    Error("EraseHash, trying to erase an unexistent hash.\n",NO_EXIT);
    return FALSE;
   }
 h=l->hash;
 for(i=0;i<h->size;i++)	    /* For each bucket	*/
   if(h->H[i])		    /* not empty	*/
     {
      it=h->H[i];	    /* it is the first ListElem */

      while(it)		    /* For each ListElem	*/
	{
	 next=it->next;     /* next is the next   :)	*/
	 free(it);
	 it=next;	    /* the next it is it! :)	*/
	}
     }
 free(h->H);
 free(h);
 l->hash=NULL;

 return TRUE;
}

/***************************************************************************
*									   *
* FUNCTION:	ReHashList						   *
*									   *
*  PURPOSE:	Rehash a list changing the hash size.			   *
*									   *
*   PARAMS:	The hash index size and the list to rehash.		   *
*									   *
*   RETURN:	TRUE if succesful,					   *
*		FALSE else.						   *
*									   *
*  IMPORTS:	None							   *
*									   *
*    NOTES:	This function erase the old hash and create a new one.	   *
*		So all the list elements must inserted in it again; this   *
*		function is time consuming. If the new hashing fails the   *
*		old one is preserved.					   *
*									   *
***************************************************************************/
boolean ReHashList(int newsize, List l)
{
 Hash *hold, *hnew;

 hold=l->hash;
 l->hash=NULL;
 if(HashList(newsize, hold->HashKey, l))
     {
      hnew=l->hash;
      l->hash=hold;
      EraseHashList(l);
      l->hash=hnew;
      return TRUE;
     }
 else
     {
      l->hash=hold;
      return TRUE;
     }
}
