MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS     = xmbind
xmbind_LDADD     = ../../lib/Xm/libXm.la $(X_LIBS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS      = $(X_CFLAGS) $(XFT_CFLAGS)

