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

if [[ $PS3DEV == "" ]]; then
	echo -e "\t\t[FAIL]"
	echo "\$PS3DEV is not defined, please install ps3toolchain"
	echo "instruction : https://github.com/ooPo/ps3toolchain"
	exit 1;
fi

