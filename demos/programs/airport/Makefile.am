MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = XmdAirport.ad README

bindir  = ${prefix}/share/Xm/airport
datadir = ${prefix}/share/Xm/airport

data_DATA        = $(airport_SOURCES) $(EXTRA_DIST)
bin_PROGRAMS     = airport
noinst_HEADERS   = airport.h dragsource.h dropsite.h main.h
airport_SOURCES  = main.c dragsource.c dropsite.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
airport_LDADD    = $(top_builddir)/lib/Xm/libXm.la

