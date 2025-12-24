
MAINTAINERCLEANFILES = Makefile.in
AM_CPPFLAGS          = -I../../lib $(GL_CFLAGS) $(XFT_CFLAGS) $(X_CFLAGS)
bindir               = ${prefix}/share/Xm/GLw
bin_PROGRAMS         = paperplane
paperplane_LDADD     = ../../lib/GLw/libGLw.la ../../lib/Xm/libXm.la $(GL_LIBS) $(XFT_LIBS) $(X_LIBS) -lm
paperplane_SOURCES   = paperplane.c

