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

find_package(GTest REQUIRED)

add_executable(
    ebnf_compiler_test

    ebnf_tokeniser_test.cc
    ebnf_parser_test.cc
    ebnf_generator_test.cc
)

add_executable(
    chip8_compiler_test
    
    chip8_cmdline_test.cc
    chip8_tokeniser_test.cc
    chip8_parser_test.cc
)

target_link_libraries( ebnf_compiler_test  ebnf_compiler_lib  GTest::gtest_main )
target_link_libraries( chip8_compiler_test chip8_compiler     GTest::gtest_main )

include(GoogleTest)
gtest_discover_tests( ebnf_compiler_test )
gtest_discover_tests( chip8_compiler_test )
