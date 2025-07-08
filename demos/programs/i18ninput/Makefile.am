MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/i18ninput
datadir = ${prefix}/share/Xm/i18ninput

bin_PROGRAMS = i18ninput
i18ninput_SOURCES = i18ninput.c input.c 
EXTRA_DIST = XmdI18nInput.ad README i18ninput.man i18ninput.help
data_DATA = $(EXTRA_DIST) $(i18ninput_SOURCES)
LDADD = ../../lib/Xmd/libXmd.la ../../../lib/Xm/libXm.la

INCLUDES = -I$(srcdir)/../../lib -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS} 
