#!/bin/csh
#
#	Environment variables
#
setenv HOST `hostname`
setenv DISPLAY	${HOST}:0.0
setenv MWMSHELL /bin/sh

#shouldn't this already be set by .login?
setenv MACHINE `uname`
if ($status) then
	setenv MACHINE `machine`
endif
if ($status) then
	unsetenv MACHINE
	echo "Warning: cannot set machine type"
endif

if (${MACHINE} == "SunOS") then
    setenv PURIFYHOME /usr/local/apps/purify
endif

unset autologout

set history	= 100				# length of history list
set pager	= 'more'			# Set mail pager
set noclobber					# Be careful on overwritting
set ignoreeof					# Disallow ^D to logout
set filec					# ESC key = filename completion
set fignore	= (.o a.out ~ .BAK \~ core)	# list of files/ext to ignore
set prompt	= "qauser@${HOST} \! % "	# PROPER PROMPT
set notify

umask 022					# creat files   rwxr-xr-x

#
#	Setup the PATH variable
#
set path = (. ~/. /usr/ode/bin_sc /usr/ucb /bin /usr/bin /usr/sbin /usr/local/apps/CenterLine/bin /usr/local/apps/purify /usr/local/bin /etc /usr/etc /usr/local/pub /usr/local/pub/DEC /usr/sccs /usr/new /usr/new/mh /usr/server1/usr/bin /usr/ode/bin_sc /usr/local/CenterLine/bin /project/motif/qa/qauser/qatools/auto_rep /project/motif/build/tools/src/bin/share /project/motif/qa/qauser/qatools/regression)

switch ($MACHINE)
   case OSF1*:
   case osf1*:
	if (`uname -m` == "alpha") then
	    set path=($path ~/bin/OSF1.alpha ~/bin)
	else if (${HOST} == "pmin11") then
#	    setenv PATH  $PATH\:/users/qauser/bin/OSF1.mips/.\:/users/qauser/bin\:/users/qauser/qatools/vista\:/project/tools/bin/mips.OSF1\:/usr/bin/vista
	    setenv PATH  /project/ot/newbin/mips.OSF1\:$PATH\:/users/qauser/bin/OSF1.mips/.\:/users/qauser/bin\:/users/qauser/qatools/vista\:/project/tools/bin/mips.OSF1\:/usr/bin/vista\:/project/motif/qa/qauser/qatools/regression
            setenv MANPATH /usr/man:/project/tools/man:/usr/ode/man:/usr/bin/vista/vista/doc/man
        else
#	    setenv PATH  $PATH\:/project/motif/qa/qauser/bin/OSF1/.\:/users/qauser/bin\:/users/qauser/qatools/vista\:/project/tools/bin/i386.OSF1\:/usr/vista/bin
	    setenv PATH  /project/ot/newbin/i386.OSF1\:$PATH\:/project/motif/qa/qauser/bin/OSF1/.\:/users/qauser/bin\:/users/qauser/qatools/vista\:/project/tools/bin/i386.OSF1\:/usr/vista/bin\:/project/motif/qa/qauser/qatools/regression
            setenv MANPATH /usr/man:/project/tools/man:/usr/ode/man:/usr/vista/man
        endif
	if (${HOST} != "chapman") then
	    setenv VLM_HOST pomerol
	endif
	breaksw
   case mips*:
   case ULTRIX*:
	setenv PATH $PATH\:/users/qauser/bin/Ultrix\:/usr/local/pub/DEC\:/users/qauser\:/project/tools/bin/mips.ULTRIX
	breaksw
   case sparc*:
   case SunOS*:
# This is temporary; to be removed as soon as Purify is installed on
# QA machines
        if (${HOST} == "moonbeam") then
#		setenv PATH  $PATH\:$HOME/bin/Sparc\:$HOME/bin\:/usr/sccs\:/project/tools/bin/sparc.SunOS\:$HOME/qatools/memory
		setenv PATH  /project/ot/newbin/sparc.SunOS\:$PATH\:$HOME/bin/Sparc\:$HOME/bin\:/usr/sccs\:/project/tools/bin/sparc.SunOS\:$HOME/qatools/memory
	else
#		setenv PATH  $PATH\:/users/qauser/bin/Sparc\:/users/qauser/bin\:/usr/sccs\:/project/tools/bin/sparc.SunOS\:/users/qauser/qatools/memory
		setenv PATH  /project/ot/newbin/sparc.SunOS\:$PATH\:/users/qauser/bin/Sparc\:/users/qauser/bin\:/usr/sccs\:/project/tools/bin/sparc.SunOS\:/users/qauser/qatools/memory\:/project/motif/qa/qauser/qatools/regression
	endif
	setenv PATH $PATH\:$PURIFYHOME
	endif
	setenv context sparc_sunos
        breaksw
   case sgi*:
   case IRIX*:
	setenv PATH  $PATH\:/users/qauser/bin/IRIX\:/users/qauser/bin
	breaksw
#hp700 now
   case HP-UX*:
   case hp700*:
#	setenv PATH  $PATH\:/users/qauser/bin/HPUX/.\:/users/qauser/bin\:/project/tools/bin/hp9000_700
	setenv PATH  /project/ot/newbin/hp9000_700\:$PATH\:/users/qauser/bin/HPUX/.\:/users/qauser/bin\:/project/tools/bin/hp9000_700\:/project/motif/qa/qauser/qatools/regression
	setenv context hp700_hpux
	setenv USER qauser
	breaksw
   default:
	setenv PATH  $PATH\:/users/qauser/bin
	breaksw
endsw
endif

setenv OT_PROJECT motif

if ( $MACHINE == "OSF1" && `uname -m` == "alpha" ) then
setenv XNLSPATH /usr/lib/X11/nls
else
setenv XNLSPATH /project/motif/ref_src/X11r5/mit/lib/nls/Xsi
endif

setenv PATH  $PATH\:/usr/bin/X11/

#
#	Alias commands
#
alias	bye	'.mylogout &; exit'
alias   rs      'set noglob; eval `resize`; unset noglob'
alias	cls	clear			# DOS type aliases for novices
alias	c	clear			
alias	hist	'history 15'
alias	grip    'grep -i'
alias	j	jobs
alias 	h	history
alias   pirs    'echo "]2;pirs"; echo "]1;pirs"; /users/ifmxmgr/npirs/4gi/newfglgo /users/ifmxmgr/npirs/4gi/pirs'
alias	ls	'ls -aF'
alias	lpr	'lpr -Pnorton'
alias	lpq	'lpq -Pnorton'
alias	lprmnort 'lprm -Pnorton'
alias	cd	'cd \!*; echo "]2;"${HOST}":"${cwd}; echo "]1;"${HOST}'
alias	pd	'pushd \!*; echo "]2;"\(pushd\)${HOST}":"${cwd}'
alias	po	'popd \!*; echo "]2;"\(popd\)${HOST}":"${cwd}'
alias 	amake	'make IMAKE_DEFINES=-DAUTOMATION Makefile'
alias	avmake	'make IMAKE_DEFINES="-DAUTOMATION -DUSE_VISTACC" Makefile'
alias   apmake  'make IMAKE_DEFINES="-DAUTOMATION -DUSE_PURIFY" Makefile'
alias	xt	'set term = xterm ; tset; resize'
alias	xtitle	'echo -n "]0;\!*"'

if ("$HOST" == "skeptic") then
	stty erase 
endif
