#!/bin/bash

# EPP-2_PROG - Linux EPP-2 EPROM Programmer Application
# Copyright (C) 2024 Jason Birch
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

gcc AddBinToROM.c -o AddBinToROM
gcc BinToMotorola.c -o BinToMotorola
gcc EPP-2_PROG.c -o EPP-2_PROG
