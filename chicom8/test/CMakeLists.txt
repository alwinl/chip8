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

include( GoogleTest )

add_executable(
    chip8_compiler_test

    chip8_cmdline_test.cc
    chip8_tokeniser_test.cc
    chip8_parser_test.cc
    chip8_ostream_test.cc
    chip8_generator_test.cc
	chip8_irbuilder_test.cc
	chip8_irbuilder2_test.cc
)

target_link_libraries( chip8_compiler_test chip8_compiler gtest gtest_main )
