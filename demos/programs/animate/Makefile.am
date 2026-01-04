MAINTAINERCLEANFILES = Makefile.in
CLEANFILES           = dog.uid plane.uid superman.uid xmanimate.uid
EXTRA_DIST           = dog.uil plane.uil superman.uil xmanimate.uil \
                       superman1.xbm superman2.xbm superman3.xbm    \
                       superman4.xbm superman5.xbm superman6.xbm    \
                       XmdAnimate.ad README xmanimate.help

bindir  = ${prefix}/share/Xm/animate
datadir = ${prefix}/share/Xm/animate
mandir  = ${prefix}/share/Xm/animate

data_DATA              = $(xmanimate_SOURCES) $(EXTRA_DIST)
man_MANS               = xmanimate.man
bin_PROGRAMS           = xmanimate
xmanimate_DEPENDENCIES = dog.uid plane.uid superman.uid xmanimate.uid
AM_CPPFLAGS            = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES       = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                         -I$(top_builddir)/include -I$(top_builddir)/lib
xmanimate_LDADD        = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                         $(top_builddir)/lib/Mrm/libMrm.la \
                         $(top_builddir)/lib/Xm/libXm.la

.uil.uid:
	$(top_builddir)/clients/uil/uil -I$(top_srcdir)/clients/uil -I$(top_builddir)/clients/uil -o $@ $<

SUFFIXES = .uil .uid

