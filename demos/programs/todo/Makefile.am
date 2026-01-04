MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README XmdTodo.ad example.todo todo.help

bindir  = ${prefix}/share/Xm/todo
datadir = ${prefix}/share/Xm/todo
mandir  = ${prefix}/share/Xm/todo

noinst_HEADERS   = page.h
data_DATA        = $(todo_SOURCES) $(EXTRA_DIST)
man_MANS         = todo.man
bin_PROGRAMS     = todo
todo_SOURCES     = todo.c io.c actions.c
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
todo_LDADD       = $(top_builddir)/demos/lib/Exm/libExm.a \
                   $(top_builddir)/demos/lib/Xmd/libXmd.la \
                   $(top_builddir)/lib/Xm/libXm.la


