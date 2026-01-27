Motif
=====

![Build Status](https://github.com/thentenaar/motif/actions/workflows/build.yml/badge.svg?branch=master)
![IRC Channel](https://img.shields.io/badge/irc.oftc.net-%23motif-blue)

## Action Shots

![Xcursor in action](https://github.com/thentenaar/motif/blob/pages/screenshots/periodic-xcursor.png?raw=true)
![OpenBSD 7.8 + CDE](https://github.com/thentenaar/motif/blob/pages/screenshots/openbsd-7.8-cde.png?raw=true)

## Background

This fork of Motif was born of a desire to keep Motif (and other X11
technologies) alive and well. The original upstream Sourceforge project
hasn't had any activity in over two years, none of the project admins
have been active for at least that amount of time, the official bug
tracker has disappeared into the void; and the user forum was closed
way back in 2017. Sadly, it appears that the original upstream has
abandoned the project.

I've incorporated some fixes from upstream that have laid dormant
for years, a few others from Gentoo, and made a few improvements of my
own. I intend to maintain this fork, and in doing so advocate for the
continued use of the user interface toolkit that defined an era, and
influenced many of the user interfaces that came after it.

## Building from Source

You can build from source as follows (assuming that you have
autoconf, automake, etc. installed):
```
% ./autogen.sh && make
```

## Building the Demos

The demos can be built by running:
```
% make demos
```

Note that not all of the demos are in working order, and there are a
few that depend on running under mwm.

## Running the Tests

The tests require [check](https://github.com/libcheck/check) to be
installed, and can be run by:
```
% ./configure --enable-tests && make check
```

## Notable Changes

- Support for Xcursor and loading cursors from SVG/PNG files
- Initial support for Xrandr / Xinerama (Xrandr preferred)
- With ``XPRINT`` being long dead, and no-one using it, the PrintShell has been removed
- Transparent [Xdnd](https://www.freedesktop.org/wiki/Specifications/XDND) protocol support
- Mesa's [GL drawingarea](https://gitlab.freedesktop.org/mesa/glw) widget has been included,
  and may be built by specifying ``--enable-glw``.
- The autotools build system has been updated
- The following macros that were previously exposed in ``Xm.h`` have been
renamed, and will evaluate to 1 if enabled, 0 if disabled:
```
    - JPEG_SUPPORTED     -> XM_WITH_JPEG
    - PNG_SUPPORTED      -> XM_WITH_PNG
    - UTF8_SUPPORTED     -> XM_UTF8
```

- The ``XM_MSGCAT`` macro was added, to signify that Motif was built with
support for X/Open message catalogs.
- JPEG, PNG, Xft, and Xrandr support is enabled by default if present at build-time.
  Specify ``--without-jpeg``, etc. to disable them.
- Motif's editres implementation has been removed, so that the updated version
  in libXmu >= 1.3.0 can be used instead. Editres itself will require [this](https://gitlab.freedesktop.org/xorg/app/editres/-/merge_requests/7.diff)
  patch.
- ``XmCommandGetChild()`` is deprecated in favor of ``XtNameToWidget`` like the other ``*GetChild()`` functions
- The demos have been split out from the ``all`` make target
- Various bugs have been fixed
- Tons (probably >= 10k lines) of dead code pruned

## TODO

- Continued fixes and improvements

## Packaging Status

[![Packaging status](https://repology.org/badge/vertical-allrepos/motif.svg)](https://repology.org/project/motif/versions)

