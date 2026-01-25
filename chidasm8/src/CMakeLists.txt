#
# CMakeLists.txt Copyright 2024 Alwin Leerling dna.leerling@gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#

set(CMAKE_CXX_STANDARD 20)

add_library(
	chidasm8_logic

	disassembler.cc
	data_bytes.cc
	# instruction.cc
	# target.cc
	utils.cc
	memory.cc
	commandlineparser.cc
	# filenameextractor.cc
	# instruction.cc
	# program.cc
	# token.cc
)

target_include_directories( chidasm8_logic INTERFACE ./ )

