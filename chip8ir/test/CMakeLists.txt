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

	ir/unit_asm_emitter.cc
	ir/unit_cfg_emitter.cc
	ir/unit_instruction.cc
	ir/unit_decoder.cc
	ir/unit_encoder.cc
	ir/int_irprogram.cc
)

target_link_libraries( ir_test PRIVATE gtest gtest_main chip8::ir )

add_executable(
	assembler_test

	assembler/unit_loader.cc
	assembler/unit_tokeniser.cc
	assembler/unit_parser.cc
	assembler/int_astgeneration.cc
	assembler/unit_expression.cc
	assembler/unit_symbol_table.cc
	assembler/unit_instruction_generator.cc
	assembler/int_assembler.cc
	assembler/unit_symbol_collector.cc
)

target_link_libraries( assembler_test PRIVATE gtest gtest_main chip8::ir )

add_executable(
	disassembler_test

	disassembler/unit_symbol_table.cc
	disassembler/unit_memory.cc
	disassembler/int_disassembler.cc
)

target_link_libraries( disassembler_test PRIVATE gtest gtest_main chip8::ir )
