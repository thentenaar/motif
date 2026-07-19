#!/bin/sh

set -eu

set -- \
"DerivedCoreProperties.txt"        \
"DerivedNormalizationProps.txt"    \
"CompositionExclusions.txt"        \
"UnicodeData.txt"                  \
"extracted/DerivedNumericType.txt" \
"extracted/DerivedLineBreak.txt"   \
"auxiliary/WordBreakProperty.txt"  \
"emoji/emoji-data.txt"

REDIRECT=0
DLOPTS=

if [ "x$(which wget)" != "x" ]
then
	DL=$(which wget)
elif [ "x$(which curl)" != "x" ]
then
	DL=$(which curl)
	REDIRECT=1
	DLOPTS="-s"
elif [ "x$(which GET)" != "x" ]
then
	DL=$(which GET)
	REDIRECT=1
elif [ "x$(which lwp-download)" != "x" ]
then
	DL=$(which lwp-download)
	REDIRECT=1
elif [ "x$(which lwp-request)" != "x" ]
then
	DL=$(which lwp-request)
	REDIRECT=1
else
	echo "ERROR: Expected wget, curl, or LWP to be available"
	exit 1
fi

for f in $@
do
	if [ -f ${f} ]
	then
		rm -f ${f}
	fi

	echo "Downloading ${f}"
	if [ $REDIRECT -eq 0 ]
	then
		${DL} ${DLOPTS} https://www.unicode.org/Public/UCD/latest/ucd/${f}
	else
		${DL} ${DLOPTS} https://www.unicode.org/Public/UCD/latest/ucd/${f} > ${f}
	fi
done

