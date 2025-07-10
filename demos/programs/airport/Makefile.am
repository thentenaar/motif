MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/airport
datadir = ${prefix}/share/Xm/airport

bin_PROGRAMS = airport
airport_SOURCES = main.c dragsource.c dropsite.c airport.h \
                  dragsource.h dropsite.h main.h
EXTRA_DIST = XmdAirport.ad README
data_DATA = $(airport_SOURCES) $(EXTRA_DIST)
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
