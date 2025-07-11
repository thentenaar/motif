#!/bin/sh 

exec >/dev/null 2>/dev/null

xrdb $HOME/.Xdefaults
xhost +

xset s off
xset b off

xsetroot -solid mediumaquamarine

if [ ${MACHINE} = "sparc" ] ||
   [ ${MACHINE} = "SunOS" ]
then
	xmodmap $HOME/bin/Sparc/.xmodmaprc

	cp $HOME/bin/Sparc/.motifbind $HOME
	$HOME/bin/Sparc/mwm &

fi

if [ ${MACHINE} = "mips" ] ||
   [ ${MACHINE} = "ULTRIX" ]
then

	cp $HOME/bin/Ultrix/.motifbind $HOME
	$HOME/bin/Ultrix/mwm &
	
fi

if [ ${MACHINE}  = "hp700" ] ||
   [ ${MACHINE}  = "HP-UX" ]
then

	xmodmap $HOME/bin/HPUX/.xmodmaprc
	cp $HOME/bin/HPUX/.motifbind $HOME
	$HOME/bin/HPUX/mwm &
	
fi

if [ ${MACHINE}  = "osf1" ] ||
   [ ${MACHINE}  = "OSF1" ]
then

   if [ "`machine`" = "alpha" ]
   then
	xset fp /project/motif/qa/qauser/fonts/OSF1.alpha
	xset fp rehash
#	xmodmap $HOME/bin/OSF1.alpha/.xmodmaprc
	cp $HOME/bin/OSF1.alpha/.motifbind $HOME
	$HOME/bin/OSF1.alpha/mwm &
   else
	xmodmap $HOME/bin/OSF1/.xmodmaprc
	cp $HOME/bin/OSF1/.motifbind $HOME
	$HOME/bin/OSF1/mwm &
  fi

fi

if [ ${MACHINE}  = "sgi" ] ||
   [  ${MACHINE} = "IRIX" ]
then

	xmodmap $HOME/bin/IRIX/.xmodmaprc
	cp $HOME/bin/IRIX/.motifbind $HOME
	$HOME/bin/IRIX/mwm &
fi



xterm -geometry =80x24+801+700 &
xterm -geometry =80x24+0+175 &

if [ ${MACHINE}  =  "hp300" ] ||
   [ ${MACHINE}  =  "HP-UX" ]
then

	xterm -T Console -n Console -C -iconic &
fi

if [ ${MACHINE}  =  "sparc" ] ||
   [ ${MACHINE}  =  "SunOS" ]
then

	xterm -T Console -n Console -C -iconic &
fi

sleep 20

rm -f $HOME/.motifbind

exec xterm -iconic -n login -T login
