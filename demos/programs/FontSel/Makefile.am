MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/fontsel
datadir = ${prefix}/share/Xm/fontsel

bin_PROGRAMS = fontsel
fontsel_SOURCES = callbacks-c.c creation-c.c fontsel.c fontsel.h
EXTRA_DIST =
data_DATA = $(EXTRA_DIST) $(fontsel_SOURCES)
LDADD = $(TOPDIR)/lib/Xm/libXm.la
TOPDIR = ../../..

AM_CPPFLAGS = -I$(TOPDIR)/lib -I$(srcdir)/$(TOPDIR)/lib ${X_CFLAGS}
