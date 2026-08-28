MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/font_dialog
datadir = ${prefix}/share/motif/demos/font_dialog

data_DATA         = $(font_dialog_SOURCES)
bin_PROGRAMS      = font_dialog
AM_CPPFLAGS       = $(X_CFLAGS)
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
font_dialog_LDADD = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

