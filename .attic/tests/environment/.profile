#
#  Set up environment for QATS automated testing
#
#
export PATH=.:/X11/bin:/usr/ccs/bin:/usr/sbin:/usr/bin:/usr/bin/X11:/bin:/usr/atria/bin:/usr/ucb
#
#  Usable tty settings
#
stty erase ^h kill ^u intr ^c
#
#  Temporary work-around until the tests get built with the proper
#	dependencies built in
#
LINK_PATH=/proj/motif/exports/lib:/proj/motif/imports/x11/lib
if [ `uname` = "SunOS" ]; then
	export LD_LIBRARY_PATH=$LINK_PATH

elif [ `uname` = "HP-UX" ]; then
	export SHLIB_PATH=$LINK_PATH
fi
#
umask 0
