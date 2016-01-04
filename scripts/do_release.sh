#!/bin/bash

# Build script to compile all language versions of SiEd-Dana from the Master
# branch on GitHub.
#
# USAGE:  do_release <version number>
#

function make_release()
{
	echo $1;
	echo $2;
	echo $3;
	make clean
	make "$2"
	zip sied-dana-"$1"-"$3".prc.zip sied-dana.prc
	mv sied-dana-"$1"-"$3".prc.zip ../../SiEd-Dana-"$1"/
}


if [ "$#" -ne 1 ]
then
	echo -e "\nERROR - Usage:  do_release <release version number>\n"
else
	cd /tmp
	git clone https://github.com/rtiangha/SiEd-Dana.git
	mkdir SiEd-Dana-$1
	cd SiEd-Dana/src/
	make clean
	make
	cp sied-dana.prc ../../SiEd-Dana-$1/sied-dana-current.prc
	zip sied-dana-$1.prc.zip sied-dana.prc
	mv sied-dana-$1.prc.zip ../../SiEd-Dana-$1/
	cp ../docs/changelog ../../SiEd-Dana-$1/changelog.txt
	cp ../docs/install ../../SiEd-Dana-$1/install.txt

	make_release $1 french fr
	make_release $1 german de
	make_release $1 italian it
	make_release $1 portuguese pt
	make_release $1 norwegian no
	make_release $1 dutch nl
	make_release $1 catalan ca
	make_release $1 spanish es
	make_release $1 turkish tr
	make_release $1 russian ru
	make_release $1 polish pl
	make_release $1 swedish se
	make_release $1 czech cz

	make clean
	cd /tmp/
	mv SiEd-Dana sied-dana
	tar zcvf sied-dana-$1-src.tar.gz sied-dana/
	mv sied-dana-$1-src.tar.gz SiEd-Dana-$1/
	zip -r SiEd-Dana-$1.zip SiEd-Dana-$1
	rm -rf sied-dana/
fi

