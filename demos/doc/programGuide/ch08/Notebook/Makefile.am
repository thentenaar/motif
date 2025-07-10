MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README notebook.man

noinst_PROGRAMS = notebook

notebook_SOURCES = interface.c  notebook.c

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
