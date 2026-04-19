MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/dainput
datadir = ${prefix}/share/motif/demos/dainput

data_DATA        = $(dainput_SOURCES)
bin_PROGRAMS     = dainput
dainput_SOURCES  = dainput.c dainput_dlg.c dainput_ui.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
dainput_LDADD    = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

