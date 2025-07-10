MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = dogs.uil Dog.uil Square.uil bark.bm  down.bm  up.bm

noinst_HEADERS = Dog.h \
DogP.h \
Square.h \
SquareP.h

CLEANFILES = dogs.uid

noinst_PROGRAMS = dogs

dogs_SOURCES = dogs.c Dog.c Square.c

dogs_DEPENDENCIES = dogs.uid

LDADD = $(top_builddir)/lib/Mrm/libMrm.la $(top_builddir)/lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}

SUFFIXES = .uil .uid

.uil.uid:
	../../../clients/uil/uil $< -I$(srcdir) -o $@


