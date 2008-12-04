# $Id$
############################################################################
#
# File:    Makefile
#
# Author:  Damien Stuart
# 
# Purpose: Makefile for fwknop-c implementation
#
# Copyright (C) 2008 Damien Stuart (dstuart@dstuart.org)
#
#  License (GNU Public License):
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
#     USA
#
############################################################################
#
CC      = gcc
AR      = ar

# Base CFLAGS
# For Full debugging (for extreme verbose output at runtime), add
# "-DDEBUG to the BASE_CFLAGS arg.  This should not be used on a
# production build.
#
BASE_CFLAGS = -Wall -fno-strict-aliasing 

# Uncomment one of these CFLAGS based on your needs
#
## Prod Build
#CFLAGS  = -O2 $(BASE_CFLAGS)
#
## For debugging symbols if you plan to use a debugger
#
CFLAGS  = -g -O0 $(BASE_CFLAGS)

LDFLAGS =

LIBS    = #$(PCAP_LIB) -lm -lz

PROG    = fwknop
SRC     = fwknop.c

OBJ     = $(SRC:.c=.o)

TPROG   = fko_test
TSRC    = fko_test.c

TOBJ    = $(TSRC:.c=.o)

LIBFWK  = libfwknop.a
LIBSRCS = spa_random_number.c \
          spa_user.c \
          spa_timestamp.c \
          spa_version.c \
          spa_message_type.c \
          md5.c \
          sha1.c \
          sha256.c \
          digest.c \
          base64.c \
          strlcat.c \
          strlcpy.c

LIBOBJS = $(LIBSRCS:.c=.o)

# Group all the source files for make depend.
#
ALLSRCS = $(LIBSRCS) $(SRC) $(TSRC)

###########################################################################
# Targets
#
all: $(PROG) $(TPROG)

$(PROG): $(OBJ) $(LIBFWK)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBFWK) $(LIBS)

$(TPROG): $(TOBJ) $(LIBFWK)
	$(CC) $(LDFLAGS) -o $@ $(TOBJ) $(LIBFWK) $(LIBS)

lib: $(LIBFWK)

$(LIBFWK): $(LIBOBJS)
	$(AR) rcs $@ $(LIBOBJS)

# Force a normal rebuild.
#
rebuild: clean $(PROG)

strip: $(PROG)
	strip $(PROG)

clean:
	rm -f $(PROG) $(TPROG) *.o *.a *.so

realclean: clean
	rm -f core *.bak *.tmp *[-~]

# Generate the dependencies for the sources in this current directory
# while ignoring warnings. Note: If you don't have makedepend in your PATH,
# you will simple get a warning and noting will happen.
#
depend:
	@`which makedepend 2>/dev/null` -Y -- $(CFLAGS) -- $(ALLSRCS) 2> /dev/null \
		&& echo "makedepend -Y -- $(CFLAGS) -- $(ALLSRCS) 2> /dev/null" \
		|| echo " - makedepend not found.  Aborting..."


###########################################################################
# Dependencies - (These are automatically generate with "make depend")
#
# DO NOT DELETE

spa_random_number.o: fwknop.h types.h digest.h md5.h sha.h base64.h
spa_user.o: fwknop.h types.h digest.h md5.h sha.h base64.h
spa_timestamp.o: fwknop.h types.h digest.h md5.h sha.h base64.h
spa_version.o: fwknop.h types.h digest.h md5.h sha.h base64.h
spa_message_type.o: fwknop.h types.h digest.h md5.h sha.h base64.h
md5.o: md5.h types.h
sha1.o: sha.h types.h
sha256.o: sha.h types.h
digest.o: digest.h types.h md5.h sha.h
base64.o: base64.h
fwknop.o: fwknop.h types.h digest.h md5.h sha.h base64.h
fko_test.o: fwknop.h types.h digest.h md5.h sha.h base64.h
