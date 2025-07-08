MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libXmd.la

noinst_PROGRAMS = onHelp

libXmd_la_SOURCES = Help.c Menus.c Print.c RegEdit.c AdjView.c

noinst_HEADERS = AdjView.h \
Help.h HelpP.h \
Menus.h \
Print.h PrintP.h \
RegEdit.h RegEditI.h

LDADD = libXmd.la ../../../lib/Xm/libXm.la

INCLUDES = -I$(top_builddir)/lib -I$(srcdir)/../../../lib ${X_CFLAGS} 

EXTRA_DIST =

