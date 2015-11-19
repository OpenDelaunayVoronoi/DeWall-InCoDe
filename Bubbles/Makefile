############################################################################
############################### 17/Jul/93 ##################################
############################   Version 1.0   ###############################
######################### Author: Paolo Cignoni ############################
#									   #
#  FILE:	Makefile						   #
#									   #
#  PURPOSE:	Generating bubbles program				   #
#									   #
#    NOTES:	bubbles: a program for generating random clustered         #
#		2d/3d point datasets. 					   #
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
MYFLAGS = -O -I../include -DSGI

OLISTOBJ= error.o 

OLISTINC= error.h 

#
# Dependencies
#

bubbles:     bubbles.o $(OLISTOBJ)
		$(CC) $(MYFLAGS) bubbles.o $(OLISTOBJ) -o bubbles -lm
		

bubbles.o:     bubbles.c $(OLISTINC) 
	    $(CC) $(MYFLAGS) -c bubbles.c -o bubbles.o


error.o:	error.c $(OLISTINC)
		$(CC) $(MYFLAGS) -c error.c -o error.o


test:
		sh maketest.script

clean: 
	- rm -f *.o 
	- rm -f ../OList/*.o 
	- rm -f bubbles 
	- rm -f core 
