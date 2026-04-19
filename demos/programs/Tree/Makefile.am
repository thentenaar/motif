MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/Tree
datadir = ${prefix}/share/motif/demos/Tree

data_DATA        = $(tree_SOURCES)
bin_PROGRAMS     = tree
tree_SOURCES     = bxutil-c.c tree.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(srcdir)/.. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
tree_LDADD       = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

