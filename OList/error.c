/***************************************************************************
******************************* 18/Jan/93 **********************************
****************************   Version 1.0   *******************************
************************* Author: Paolo Cignoni ****************************
*									   *
*    FILE:	error.c							   *
*									   *
* PURPOSE:	Implementing functions for error handling		   *
*									   *
*									   *
* EXPORTS:	Error							   *
*		ErrorNULL						   *
*		ErrorFALSE						   *
*									   *
* IMPORTS:								   *
*									   *
* GLOBALS:	ProgramName						   *
*									   *
*   NOTES:	User Must declare an external variable			   *
*		ProgramName containing a string with the program name.	   *
*									   *
*    18/Jan/93	Changed	ProgramName to internal global variable and added  *
*		SetProgramName function					   *
*                                                                          *
*    18/Jan/94  Added Errorf function that print a formatted error message *
*                                                                          *
****************************************************************************
***************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>

#include <OList/general.h>
#include <OList/error.h>


char ProgramName[80]="Error";

/***************************************************************************
*									   *
* FUNCTION:	Error							   *
*									   *
*  PURPOSE:	Write an error message on stderr and exit -1.		   *
*									   *
*   PARAMS:	The string to write, and the exit condition.		   *
*									   *
*   RETURN:	Nothing.						   *
*									   *
*  IMPORTS:	The global string ProgramName, that is normally		   *
*		defined in file containing main().			   *
*									   *
*    NOTES:	If the param. Exit is true this function exit after	   *
*		writing the error message.				   *
*		The Error message format should be uniform:		   *
*									   *
*			Program: function, personal comment		   *
*									   *
*		Where Program is the program name, function is the	   *
*		function where the error happened and comment is some	   *
*		personal comment descriving the error and its cause.	   *
*		The user must supply only a message ('\n' terminated) with *
*		function name and comment.				   *
*									   *
***************************************************************************/


void Error(char *message, boolean ExitFlag)
{
 fputs(ProgramName,stderr);
 fputs(": ",stderr);
 fputs(message,stderr);
 if(ExitFlag) exit(-1);
}


/***************************************************************************
*									   *
* FUNCTION:     Errorf                                                     *
*									   *
*  PURPOSE:     Write a formatted error message on stderr and exit -1.     *
*									   *
*   PARAMS:	The string to write, and the exit condition.		   *
*									   *
*   RETURN:	Nothing.						   *
*									   *
*  IMPORTS:	The global string ProgramName, that is normally		   *
*		defined in file containing main().			   *
*									   *
*    NOTES:	If the param. Exit is true this function exit after	   *
*		writing the error message.				   *
*		The Error message format should be uniform:		   *
*									   *
*			Program: function, personal comment		   *
*									   *
*		Where Program is the program name, function is the	   *
*		function where the error happened and comment is some	   *
*		personal comment descriving the error and its cause.	   *
*		The user must supply only a message ('\n' terminated) with *
*		function name and comment.				   *
*									   *
***************************************************************************/


void Errorf(boolean ExitFlag, char *message, ...)
{
 va_list marker;

 fputs(ProgramName,stderr);
 fputs(": ",stderr);

 va_start( marker, message );
 vfprintf(stderr,message,marker);
 va_end( marker );

 if(ExitFlag) exit(-1);
}

/***************************************************************************
*									   *
* FUNCTION:	SetProgramName						   *
*									   *
*  PURPOSE:	Set the name of the program that is using Error.	   *
*									   *
*   PARAMS:	The char pointer to the program name.			   *
*									   *
*   RETURN:	Nothing.						   *
*									   *
*  IMPORTS:	The global string ProgramName;				   *
*									   *
*    NOTES:	this function set the global variable ProgramName imported *
*		by Error.						   *
*									   *
***************************************************************************/

void SetProgramName(char *s)
{
 strcpy(ProgramName, s);
}
