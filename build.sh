#!/bin/bash
#
#    This file is part of GEFPS3.
#
#    GEFPS3 is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    GEFPS3 is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with GEFPS3.  If not, see <http://www.gnu.org/licenses/>.
#

NAME="GEFPS3"
#version="2010-12-29.BETA"

check()
{
	#depends
	echo "Check Depends..."
	for file in ./depends/*.sh; do
		bash $file || exit 1
	done
	echo -e "\t\t[OK]"
}

version()
{
	version_major=`cat include/GEFPS3/GEFPS3.h | grep GEFPS3_VERSION_MAJOR | cut -f3 -d\ `
	version_minor=`cat include/GEFPS3/GEFPS3.h | grep GEFPS3_VERSION_MINOR | cut -f3 -d\ `
	echo "MAJOR : $version_major"
	echo "MINOR : $version_minor"
	echo "MAJOR = YEARS_MONTH"
	echo "MINOR = DAY_NUMBER"
}

build()
{
	#build
	echo "Build it..."
	make 1> /dev/null 2> make.log || { echo -e "\t\t[FAIL]" && cat make.log && exit 1; }
	echo -e "\t\t[OK]"
	echo "$NAME is now build"
}

install()
{
	#build
	build
	#install
	echo "Install it..."
	make install 1> /dev/null 2> make.log || { echo -e "\t\t[FAIL]" && cat make.log && exit 1; }
	echo -e "\t\t[OK]"
	echo "$NAME is now intalled"
}

uninstall()
{
	echo "Remove it..."
	make uninstall 1> /dev/null 2> make.log || { echo -e "\t\t[FAIL]" && cat make.log && exit 1; }
	echo -e "\t\t[OK]"
	echo "$NAME is now removed"
}

sample()
{
	echo "Build sample..."
	old_pwd=$PWD
	{ cd sample 2> make.log && make 1> /dev/null 2> make.log; } || { echo -e "\t\t[FAIL]" && cat make.log && cd $old_pwd && exit 1; }
	echo -e "\t\t[OK]"
	cd $old_pwd
	echo "$NAME sample is now build"
}

runsample()
{
	echo "Run sample..."
	old_pwd=$PWD
	{ cd sample 2> make.log && make run 1> /dev/null 2> make.log; } || { echo -e "\t\t[FAIL]" && cat make.log && cd $old_pwd && exit 1; }
	cd $old_pwd
	errors=`cat sample/make.log`
	if [[ $errors != "" ]]; then
		echo -e "\t\t[FAIL]"
		echo $errors
		exit 1
	fi
	echo -e "\t\t[OK]"
	echo "$NAME sample is now running on your PS3"
}

if [[ $1 == "help" ]]; then
	echo "build.sh help"
	echo -e "\tDisplay this help"
	echo "build.sh version"
	echo -e "\tDisplay version of $NAME"
	echo "build.sh"
	echo -e "\tBuild $NAME"
	echo "build.sh install"
	echo -e "\tInstall $NAME"
	echo "build.sh uninstall"
	echo -e "\tRemove $NAME"
	echo "build.sh sample"
	echo -e "\tBuild a sample of how to using $NAME"
	echo "build.sh run"
	echo -e "\tRun a sample of how to using $NAME"
	exit 0
elif [[ $1 == "version" ]]; then
	version
	exit 0
elif [[ $1 == "clean" ]]; then
	echo "Cleaning..."
	make clean 1> /dev/null 2> make.log || { echo -e "\t\t[FAIL]" && cat make.log && exit 1; }
	rm -f make.log
	rm -f sample/make.log
	echo -e "\t\t[OK]"
	exit 0
elif [[ $1 == "install" ]]; then
	check || exit 1
	install || exit 1
	exit 0
elif [[ $1 == "uninstall" ]]; then
	check || exit 1
	uninstall || exit 1
	exit 0
elif [[ $1 == "sample" ]]; then
	check || exit 1
	install || exit 1
	sample || exit 1
	exit 0
elif [[ $1 == "run" ]]; then
	check || exit 1
	install || exit 1
	sample || exit 1
	runsample || exit 1
	exit 0
fi

check || exit 1
build || exit 1

