############################################################################
############################### 19/Jul/93 ##################################
############################   Version 1.0   ###############################
######################### Author: Paolo Cignoni ############################
#                                                                          #
#  FILE:        Makefile                                                   #
#                                                                          #
#  PURPOSE:     Generating InCoDe, DeWall, Bubbles and IsoSurf program     #
#                                                                          #
#    NOTES:     The two triangulators share a part of code. Such part of   #
#               code, contained in directory OList, concerns management    #
#               of list, error message functions and machine indipendent   #
#               timing functions.                                          #
#               Note that this code require an ANSI C compiler.            #
#                                                                          #
############################################################################
############################################################################

#
# MACROS
#

# Change this line to use another C compiler (gcc is the GNU C compiler)  

CC=cc


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

### Silicon Graphics ####
 MYFLAGS = -O -I../include -DSGI
# 
#
# Dependencies
#

all:            incode dewall bubbles

incode:     
	cd InCoDe; make MYFLAGS="$(MYFLAGS)" CC=$(CC)

dewall:  
	cd DeWall; make MYFLAGS="$(MYFLAGS)" CC=$(CC) 

bubbles:  
	cd Bubbles; make MYFLAGS="$(MYFLAGS)" CC=$(CC)

clean: 
	cd InCoDe; make -i clean
	cd DeWall; make -i clean
	cd Bubbles; make -i clean

lines:
	wc InCoDe/*.c InCoDe/*.h InCoDe/M* \
	DeWall/*.c DeWall/*.h DeWall/M*\
	Bubbles/*.c Bubbles/M*\
	include/OList/*.h OList/*.c

text:
	wc InCoDe/*.txt DeWall/*.txt IsoSurf/*.txt Bubbles/*.txt

test:
	more Results.txt
	echo Testing Incode...
	cd InCoDe; make -i test
	echo Testing DeWall...
	cd DeWall; make -i test

