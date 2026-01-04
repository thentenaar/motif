MAINTAINERCLEANFILES = Makefile.in
CLEANFILES           = dogs.uid
EXTRA_DIST           = README dogs.uil Dog.uil Square.uil bark.bm down.bm up.bm

bindir  = ${prefix}/share/Xm/dogs
datadir = ${prefix}/share/Xm/dogs

data_DATA         = Dog.h DogP.h Square.h SquareP.h $(dogs_SOURCES) \
                    $(dogs_DEPENDENCIES) $(EXTRA_DIST)
bin_PROGRAMS      = dogs
dogs_SOURCES      = dogs.c Dog.c Square.c
dogs_DEPENDENCIES = dogs.uid
AM_CPPFLAGS       = $(X_CFLAGS)
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
dogs_LDADD        = $(top_builddir)/lib/Mrm/libMrm.la \
                    $(top_builddir)/lib/Xm/libXm.la

.uil.uid:
	$(top_builddir)/clients/uil/uil -I$(srcdir) -I$(top_srcdir)/clients/uil -I$(top_builddir)/clients/uil -o $@ $<

SUFFIXES = .uil .uid

