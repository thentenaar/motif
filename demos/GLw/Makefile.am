MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/paperplane
datadir = ${prefix}/share/Xm/paperplane

data_DATA         = paperplane.c
bin_PROGRAMS      = paperplane
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = $(GL_CFLAGS) $(XFT_CFLAGS) $(X_CFLAGS)
paperplane_LDADD  = $(top_builddir)/lib/GLw/libGLw.la \
                    $(top_builddir)/lib/Xm/libXm.la   \
                    $(GL_LIBS) $(XFT_LIBS) $(X_LIBS)

