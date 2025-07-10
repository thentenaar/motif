MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/drag_and_drop
datadir = ${prefix}/share/Xm/drag_and_drop

bin_PROGRAMS = DNDDemo simpledrop
EXTRA_DIST = simpledrop.man DNDDemo.man README

DNDDemo_SOURCES = DNDDemo.c DNDDraw.c
simpledrop_SOURCES = simpledrop.c
data_DATA = $(EXTRA_DIST) $(DNDDemo_SOURCES) $(simpledrop_SOURCES)
noinst_HEADERS = DNDDemo.h
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
