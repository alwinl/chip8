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
    ebnf_compiler_lib STATIC

    ebnf_tokeniser.cc
    ebnf_parser.cc
    ebnf_generator.cc
    ebnf_tarjan.cc
)

target_include_directories(ebnf_compiler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
    ebnf_compiler

    ebnf_compiler.cc
)

target_link_libraries( ebnf_compiler ebnf_compiler_lib )
