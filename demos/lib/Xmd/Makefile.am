MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = helpOn.help

bindir  = ${prefix}/share/Xm/Xmd
datadir = ${prefix}/share/Xm/Xmd

noinst_HEADERS     = AdjView.h Help.h HelpP.h Menus.h Print.h PrintP.h
data_DATA          = $(EXTRA_DIST)
bin_PROGRAMS       = onHelp
noinst_LTLIBRARIES = libXmd.la
libXmd_la_SOURCES  = Help.c Menus.c Print.c AdjView.c
AM_CPPFLAGS        = $(X_CFLAGS)
DEFAULT_INCLUDES   = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                     -I$(top_builddir)/include -I$(top_builddir)/lib
onHelp_LDADD       = libXmd.la $(top_builddir)/lib/Xm/libXm.la

