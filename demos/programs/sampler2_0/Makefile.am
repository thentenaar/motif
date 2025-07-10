MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/sampler2_0
datadir = ${prefix}/share/Xm/sampler2_0

EXTRA_DIST = bee.xpm bomb.xpm cherry.xpm da.xpm girlface.xpm k7.xpm \
             labir.xpm main.xpm pile.xpm prev.xpm spir.xpm stack.xpm \
             text.xpm README sampler2_0.man sampler2_0.help \
             XmdSampler2_0.ad
bin_PROGRAMS = sampler2_0
sampler2_0_SOURCES = sampler2_0.c
data_DATA = $(sampler2_0_SOURCES) $(EXTRA_DIST)
LDADD = ../../lib/Xmd/libXmd.la ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../lib -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
