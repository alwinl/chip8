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

add_library(
	chip8ir

	chip8ir.cc

	cmdlineparser.cc

	disassembler.cc
	targets.cc
	decoder.cc
)

add_library( chip8::ir ALIAS chip8ir )

target_compile_features( chip8ir PUBLIC cxx_std_20 )

target_include_directories( chip8ir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )