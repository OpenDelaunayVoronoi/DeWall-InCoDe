/***************************************************************************
******************************* 16/Dic/92 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	listprot.h						   *
*									   *
* PURPOSE:	Prototypes of the function in list.c and listobj.c	   *
*									   *
*   NOTES:	It is automatically included after olist.h.		   *
*		User must include only olist.h to use list functions.	   *
*		Including this file is harmless.			   *
*									   *
****************************************************************************
***************************************************************************/

#ifndef OLIST_H		/* OLIST_H is defined only inside olist.h		*/
#include "olist.h"
#endif

#ifndef LISTPROT_H	/* If LISTPROT_H is already defined all this file */
			/* must be skipped.				  */

#define LISTPROT_H	/* LISTPROT_H is defined only inside listprot.h */

/***************************************************************************
*	Functions in list.c						   *
***************************************************************************/

List NewList(TypeList type, int size);
List NewHashList(TypeList type, int size,
		int (* comp)(void *elem1, void *elem2),
		int hashsize, int (*HashKey)(void *)   );
int CountList(List l);
boolean ChangeCompareObjectList(int (* comp)(void *e1, void *e2), List l);
boolean ChangeEqualObjectList(boolean (* eq)(void *e1, void *e2), List l);
boolean EraseList(List l);
boolean EqualList(List l0, List l1);
List CopyList(List l);
List UnionList(List L0, List L1);
List IntersectList(List L0, List L1);
List AppendList(List L1, List L2);


/***************************************************************************
*	Functions in listobj.c						   *
***************************************************************************/

boolean InsertList(pointer object, List l);

/*  Temporanea modifica per vedere se si puo' evitare il cast a pointer
boolean ExtractList(pointer *object, List l);
*/
boolean ExtractList(pointer object, List l);

boolean MemberList(pointer object, List l);
boolean DeleteList(pointer object, List l);
boolean DeleteCurrList(List l);

boolean FindLessObj(pointer object, List l);
boolean EqualObject(pointer h1, pointer h2, List l);
boolean Eq_Ref(pointer h1, pointer h2);
#define Eq_Obj NULL	/* Olist compare entire Obj if has no user supplied*/
			/* function for comparing.			   */


/***************************************************************************
*	Functions in listscan.c						   *
***************************************************************************/

boolean TailList(List l);
boolean HeadList(List l);


/*  Temporanea modifica per vedere se si puo' evitare il cast a pointer
boolean CurrList(pointer *object, List l);

boolean NextList(pointer *object, List l) ;
boolean PrevList(pointer *object, List l);

*/
boolean CurrList(pointer object, List l);

boolean NextList(pointer object, List l) ;
boolean PrevList(pointer object, List l);

boolean PushCurrList(List l);
boolean PopCurrList(List l);


/***************************************************************************
*	Functions in listhash.c						   *
***************************************************************************/

boolean HashList(int n, int (*HashKey)(void *), List l);
boolean InsertHash(ListElem *le, Hash *h);
boolean MemberHash(pointer obj, List l);
boolean DeleteHash(pointer obj, List l);
boolean EraseHashList(List l);
boolean ReHashList(int newsize, List l);



#endif		/* this #endif is the brother of #ifndef LISTPROT_H.*/
		/* If LISTPROT_H was already defined all this file  */
		/* must be skipped.				    */
