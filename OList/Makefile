############################################################################
############################### 17/Jul/93 ##################################
############################   Version 1.0   ###############################
######################### Author: Paolo Cignoni ############################
#									   #
#  FILE:	Makefile						   #
#									   #
#  PURPOSE:	Generating OList library 				   #
#									   #
#    NOTES:	The two triangulators share a part of code. Such part of   #
#		code, contained in directory OList, concerns management	   #
#		of list, error message functions and machine indipendent   #
#		timing functions.					   #
#               Note that this code require an ANSI C compiler.            #
#									   #
############################################################################
############################################################################

#
# MACROS
#

# Change this line to use another C compiler (gcc is the GNU C compiler) 
# CC=cc

INCLUDEDIR = ../include
CFLAGS=-I$(INCLUDEDIR)

# To change settings uncomment the MYFLAGS line relative to your machine. 
# (To uncomment a line remove the starting '#') 
# Only one of the following lines must be selected.

#### SunOs 4.1 ####
# MYFLAGS = -O -I../include -DSUN

#### Hp-UX 8.05 ####
# MYFLAGS = -O -I../include -DHP -Aa -D_INCLUDE_POSIX_SOURCE

#### Normal ansi C (imprecise timing functions) ####
# MYFLAGS = -O -I../include -DNOMACHINE

#### MsDos 3.30 or later. ####
# MYFLAGS = -O -I../include -DMSDOS

#### Silicon Graphics ####
# MYFLAGS = -O -I../include -DSGI 


OLISTOBJ= list.o listhash.o  listobj.o \
	  listscan.o chronos.o error.o

OLISTINC= ../include/OList/olist.h ../include/OList/error.h \
          ../include/OList/general.h

#
# Dependencies
#

libolist.a: 	$(OLISTOBJ) $(OLISTINC)
		ar qsv libolist.a $(OLISTOBJ)

list.o:		list.c $(OLISTINC)
			$(CC) $(MYFLAGS) $(CFLAGS) -c list.c -o list.o

listhash.o:	listhash.c $(OLISTINC)
			$(CC) $(MYFLAGS) $(CFLAGS) -c listhash.c -o listhash.o

listobj.o:	listobj.c $(OLISTINC)
			$(CC) $(MYFLAGS) $(CFLAGS) -c listobj.c -o listobj.o

listscan.o:	listscan.c $(OLISTINC)
			$(CC) $(MYFLAGS) $(CFLAGS) -c listscan.c -o listscan.o

chronos.o:	chronos.c $(OLISTINC)
			$(CC) $(MYFLAGS) $(CFLAGS) -c chronos.c -o chronos.o

error.o:	error.c $(OLISTINC)
			$(CC) $(MYFLAGS) $(CFLAGS) -c error.c -o error.o


clean: 
	- rm -f *.o
	- rm libolist.a 
	- rm -f core 


