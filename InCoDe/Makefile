############################################################################
############################### 17/Jul/93 ##################################
############################   Version 1.0   ###############################
######################### Author: Paolo Cignoni ############################
#									   #
#  FILE:	Makefile						   #
#									   #
#  PURPOSE:	Generating InCoDe program				   #
#									   #
#    NOTES:	The two triangulators share a part of code. Such part of   #
#		code, contained in directory OList, concerns management	   #
#		of list, error message functions and machine indipendent   #
#		timing functions.					   #
#	        Note that this code require an ANSI C compiler.		   #
#									   #
############################################################################
############################################################################

#
# MACROS
#

# Change this line to use another C compiler (gcc is the GNU C compiler)  
# CC=cc


# To change settings uncomment the MYFLAGS line relative to your machine.
# (To uncomment a line remove the starting '#')
# Only one of the following lines must be selected.

#### SunOs 4.1 ####
# MYFLAGS = -O -I$(PWD)/../include -DSUN
 
#### Hp-UX 8.05 ####
# MYFLAGS = -O -I../include -DHP -Aa -D_INCLUDE_POSIX_SOURCE

#### Normal ansi C (imprecise timing functions) ####
# MYFLAGS = -O -I../include -DNOMACHINE

#### MsDos 3.30 or later. ####
# MYFLAGS = -O -I../include -DMSDOS

### Silicon Graphics ####
MYFLAGS = -O2 -non_shared -mips2 -sopt -I../include -DSGI

OLISTOBJ= ../OList/list.o ../OList/listhash.o  ../OList/listobj.o \
	  ../OList/listscan.o ../OList/chronos.o ../OList/error.o

OLISTINC= ../include/OList/olist.h ../include/OList/error.h \
	  ../include/OList/general.h

#
# Dependencies
#

incode:     file.o main.o unifgrid.o stat.o geometry.o ggveclib.o $(OLISTOBJ)
		$(CC) $(MYFLAGS) file.o main.o unifgrid.o stat.o geometry.o \
		ggveclib.o $(OLISTOBJ) -o incode -lm

main.o:     main.c graphics.h incode.h $(OLISTINC) ../include/OList/chronos.h
	    $(CC) $(MYFLAGS) -c main.c -o main.o

file.o:     file.c graphics.h incode.h $(OLISTINC)
	    $(CC) $(MYFLAGS) -c file.c -o file.o

unifgrid.o: unifgrid.c graphics.h incode.h $(OLISTINC)
	    $(CC) $(MYFLAGS) -c unifgrid.c -o unifgrid.o

stat.o:	    stat.c graphics.h incode.h $(OLISTINC)
	    $(CC) $(MYFLAGS) -c stat.c -o stat.o

geometry.o: geometry.c graphics.h incode.h $(OLISTINC)
	    $(CC) $(MYFLAGS) -c geometry.c -o geometry.o

ggveclib.o: ggveclib.c graphics.h
	    $(CC) $(MYFLAGS) -c ggveclib.c -o ggveclib.o



../OList/list.o:	../OList/list.c $(OLISTINC)
			$(CC) $(MYFLAGS) -c ../OList/list.c -o ../OList/list.o

../OList/listhash.o:	../OList/listhash.c $(OLISTINC)
			$(CC) $(MYFLAGS) -c ../OList/listhash.c -o ../OList/listhash.o

../OList/listobj.o:	../OList/listobj.c $(OLISTINC)
			$(CC) $(MYFLAGS) -c ../OList/listobj.c -o ../OList/listobj.o

../OList/listscan.o:	../OList/listscan.c $(OLISTINC)
			$(CC) $(MYFLAGS) -c ../OList/listscan.c -o ../OList/listscan.o

../OList/chronos.o:	../OList/chronos.c $(OLISTINC)
			$(CC) $(MYFLAGS) -c ../OList/chronos.c -o ../OList/chronos.o

../OList/error.o:	../OList/error.c $(OLISTINC)
			$(CC) $(MYFLAGS) -c ../OList/error.c -o ../OList/error.o


clean: 
	- rm -f *.o 
	- rm -f ../OList/*.o 
	- rm -f incode
	- rm -f nul
	- rm -f core 
	- rm -f InCoDeResult 

test:
	sh test.script
