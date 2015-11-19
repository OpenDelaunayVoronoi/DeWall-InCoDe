/***************************************************************************
******************************* 18/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	error.h							   *
*									   *
* PURPOSE:	protos, defines and macros for error.c.			   *
*									   *
*   NOTES:	Pay attention using macros!!				   *
*									   *
*    03/Nov/92	Added ErrorTRUE e ErrorZero				   *
*									   *
*    18/Jan/93	Changed ErrorFALSE(string,NO_EXIT) in ErrorFALSE(string)   *
*		because NO_EXIT was redundant.				   *
*									   *
****************************************************************************
***************************************************************************/

#ifndef GENERAL_H
#include "general.h"
#endif

#ifndef EXIT
#define EXIT TRUE
#endif

#ifndef NOT_EXIT
#define NOT_EXIT FALSE
#endif

#ifndef NO_EXIT
#define NO_EXIT FALSE
#endif

#ifndef CONTINUE
#define CONTINUE FALSE
#endif


/***************************************************************************
*  functions in error.c							   *
***************************************************************************/

void Error(char *message, int ExitFlag);
void Errorf(boolean ExitFlag, char *message, ...);
void SetProgramName(char *s);


/***************************************************************************
*									   *
*    MACRO:	ErrorNULL, ErrorFALSE;					   *
*									   *
*  PURPOSE:	Write an error message on stderr and returns from calling  *
*		function.						   *
*									   *
*   PARAMS:	The string to write, and the exit condition.		   *
*									   *
*   RETURN:	Nothing.						   *
*									   *
*  IMPORTS:	The global extern string ProgramName, that is normally	   *
*		defined in file containing main().			   *
*									   *
*    NOTES:	This macro came from wish of sinthesys to avoid:	   *
*									   *
*		    if(SomeThingWrong)					   *
*		       {						   *
*			Error("Function, Error Message\n",EXITFLAG);	   *
*			return NULL;					   *
*		       }						   *
*									   *
*		with the macro we have more compact code:		   *
*									   *
*		    if(SomeThingWrong)					   *
*			ErrorNULL("Function, Error Message\n",EXITFLAG);   *
*									   *
*		Note that this macro cause a return in calling function,   *
*		so dont use it in void func or strange situations.	   *
*									   *
*     BUGS:	Because it's a macro it can generate strange errors.	   *
*		The following example is wrong				   *
*									   *
*		if(SomeThingWrong)					   *
*			ErrorNULL("Function, Error Message\n",EXITFLAG);   *
*		else DoSomethingElse;					   *
*									   *
*		because once expanded it become:			   *
*									   *
*		if(SomeThingWrong)					   *
*			{						   *
*			 Error((message),(exitflag));			   *
*			 return NULL;					   *
*			} ;						   *
*		else DoSomethingElse;					   *
*									   *
*		and the ";" after "}" before the "else" generate an error. *
*		So the correct code is just without the ";":		   *
*									   *
*		if(SomeThingWrong)					   *
*			ErrorNULL("Function, Error Message\n",EXITFLAG)    *
*		else DoSomethingElse;					   *
*									   *
*									   *
***************************************************************************/

#define ErrorNULL(message) {Error((message),NOT_EXIT);\
					 return NULL;}

#define ErrorFALSE(message) {Error((message),NOT_EXIT);\
					 return FALSE;}

#define ErrorTRUE(message) {Error((message),NOT_EXIT);\
					 return TRUE;}

#define ErrorZERO(message) {Error((message),NOT_EXIT);\
					 return 0;}
