MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README sampler2_0.help XmdSampler2_0.ad         \
                       bee.xpm bomb.xpm cherry.xpm da.xpm girlface.xpm \
					   k7.xpm labir.xpm main.xpm pile.xpm prev.xpm     \
					   spir.xpm stack.xpm text.xpm

bindir  = ${prefix}/share/Xm/sampler2_0
datadir = ${prefix}/share/Xm/sampler2_0
mandir  = ${prefix}/share/Xm/sampler2_0

data_DATA          = $(sampler2_0_SOURCES) $(EXTRA_DIST)
man_MANS           = sampler2_0.man
bin_PROGRAMS       = sampler2_0
AM_CPPFLAGS        = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES   = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                     -I$(top_builddir)/include -I$(top_builddir)/lib
sampler2_0_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                     $(top_builddir)/lib/Xm/libXm.la

