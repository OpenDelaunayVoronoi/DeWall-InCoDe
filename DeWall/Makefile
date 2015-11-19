############################################################################
############################### 17/Jul/93 ##################################
############################   Version 1.0   ###############################
######################### Author: Paolo Cignoni ############################
#									   #
#  FILE:	Makefile						   #
#									   #
#  PURPOSE:	Generating DeWall program				   #
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

# To change settings uncomment the MYFLAGS line relative to your machine. 
# (To uncomment a line remove the starting '#') 
# Only one of the following lines must be selected.

#### SunOs 4.1 ####
# MYFLAGS = -O -DSUN -I$(INCLUDEDIR)

#### Hp-UX 8.05 ####
# MYFLAGS = -O -DHP -Aa -D_INCLUDE_POSIX_SOURCE -I$(INCLUDEDIR)

#### Normal ansi C (imprecise timing functions) ####
# MYFLAGS = -O -DNOMACHINE -I$(INCLUDEDIR)

#### MsDos 3.30 or later. ####
# MYFLAGS = -O -DMSDOS -I$(INCLUDEDIR)

#### Silicon Graphics ####
MYFLAGS = -O2 -DSGI  

CFLAGS =  -I$(INCLUDEDIR)
OLISTDIR= ../OList

OLISTFILE=$(OLISTDIR)/list.c $(OLISTDIR)/listhash.c  $(OLISTDIR)/listobj.c \
          $(OLISTDIR)/listscan.c $(OLISTDIR)/chronos.c $(OLISTDIR)/error.c
 
OLISTOBJ= $(OLISTDIR)/list.o $(OLISTDIR)/listhash.o  $(OLISTDIR)/listobj.o \
          $(OLISTDIR)/listscan.o $(OLISTDIR)/chronos.o $(OLISTDIR)/error.o
 
OLISTINC= $(INCLUDEDIR)/OList/olist.h $(INCLUDEDIR)/OList/error.h \
          $(INCLUDEDIR)/OList/general.h $(INCLUDEDIR)/OList/chronos.h
 
#
# Dependencies
#

dewall:     file.o main.o unifgrid.o stat.o geometry.o ggveclib.o \
		$(OLISTDIR)/libolist.a 
		$(CC) $(CFLAGS) $(MYFLAGS) file.o main.o unifgrid.o stat.o geometry.o \
		ggveclib.o -o dewall -lm -L$(OLISTDIR) -lolist

main.o:     main.c graphics.h dewall.h $(OLISTINC) 
	    $(CC) $(CFLAGS) $(MYFLAGS) -c main.c -o main.o

file.o:     file.c graphics.h dewall.h $(OLISTINC)
	    $(CC) $(CFLAGS) $(MYFLAGS) -c file.c -o file.o

unifgrid.o: unifgrid.c graphics.h dewall.h $(OLISTINC)
	    $(CC) $(CFLAGS) $(MYFLAGS) -c unifgrid.c -o unifgrid.o

stat.o:	    stat.c graphics.h dewall.h $(OLISTINC)
	    $(CC) $(CFLAGS) $(MYFLAGS) -c stat.c -o stat.o

geometry.o: geometry.c graphics.h dewall.h $(OLISTINC)
	    $(CC) $(CFLAGS) $(MYFLAGS) -c geometry.c -o geometry.o

ggveclib.o: ggveclib.c graphics.h
	    $(CC) $(CFLAGS) $(MYFLAGS) -c ggveclib.c -o ggveclib.o

$(OLISTDIR)/libolist.a: $(OLISTFILE) $(OLISTINC)
	cd $(OLISTDIR); make MYFLAGS="$(MYFLAGS)" CC=$(CC)


clean: 
	- rm -f *.o 
	- rm -f ../OList/*.o 
	- rm -f ../OList/*.a
	- rm -f dewall 
	- rm -f nul 
	- rm -f core 
	- rm -f DeWallResult
test:
	sh test.script
