MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libXmd.la

noinst_PROGRAMS = onHelp

libXmd_la_SOURCES = Help.c Menus.c Print.c AdjView.c

noinst_HEADERS = AdjView.h \
Help.h HelpP.h \
Menus.h \
Print.h PrintP.h

LDADD = libXmd.la ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(srcdir)/../../../lib ${X_CFLAGS}

EXTRA_DIST =

