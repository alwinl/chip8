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

include(GoogleTest)

add_executable(
	ir_test

	ir/asm_emitter_test.cc
	ir/binary_emitter_test.cc
	ir/cfg_emitter_test.cc
	ir/chip8ir_test.cc
	ir/decoder_test.cc
	ir/encoder_test.cc
	ir/integration_test.cc
)

target_link_libraries( ir_test PRIVATE gtest gtest_main chip8::ir )

add_executable(
	assembler_test

	assembler/asm_tokeniser_test.cc
	assembler/asm_parser_test.cc
	assembler/asm_parser_integration.cc
	assembler/asm_integration_test.cc
)

target_link_libraries( assembler_test PRIVATE gtest gtest_main chip8::ir )

add_executable(
	disassembler_test

	disassembler/symbols_test.cc
	disassembler/memory_test.cc
	disassembler/integration_test.cc
)

target_link_libraries( disassembler_test PRIVATE gtest gtest_main chip8::ir )
