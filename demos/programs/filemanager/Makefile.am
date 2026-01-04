MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = code.xpm dir.xpm exec.xpm file.xpm none.xpm     \
                       obj.xpm pix.xpm s_dir.xpm s_exec.xpm s_file.xpm \
					   s_none.xpm s_pix.xpm filemanager.help XmdFilemanager.ad

bindir  = ${prefix}/share/Xm/filemanager
datadir = ${prefix}/share/Xm/filemanager
mandir  = ${prefix}/share/Xm/filemanager

noinst_HEADERS      = filemanager.h
data_DATA           = $(filemanager_SOURCES) $(EXTRA_DIST)
man_MANS            = filemanager.man
bin_PROGRAMS        = filemanager
filemanager_SOURCES = filemanager.c actions.c readdir.c convert.c interface.c
AM_CPPFLAGS         = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES    = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                      -I$(top_builddir)/include -I$(top_builddir)/lib
filemanager_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                      $(top_builddir)/lib/Xm/libXm.la


