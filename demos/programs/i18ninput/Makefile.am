MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README i18ninput.help XmdI18nInput.ad

bindir  = ${prefix}/share/Xm/i18ninput
datadir = ${prefix}/share/Xm/i18ninput
mandir  = ${prefix}/share/Xm/i18ninput

data_DATA         = $(i18ninput_SOURCES) $(EXTRA_DIST)
man_MANS          = i18ninput.man
bin_PROGRAMS      = i18ninput
i18ninput_SOURCES = i18ninput.c input.c
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
i18ninput_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

