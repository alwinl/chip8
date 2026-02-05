/*
 * asm_emitter_test.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
#include <gtest/gtest.h>

#include "chip8ir.h"
#include "asm_emitter.h"

TEST(ASMEmitterTest, manual_visual_inspection)
{
	BinImage image { 0x00, 0xE0, 0x12, 0x02, 0xF4, 0x29, 0x12, 0x34, 0x56, 0x78 };

    IRProgram ir;
    ir.origin = 0x200;
	ir.binary = &image;

    ir.elements.push_back(
        InstructionElement{
            0x200,
            Instruction::make_clear()
        }
    );

    ir.elements.push_back(
        InstructionElement{
            0x202,
            Instruction::make_jump( Addr {0x202} )
        }
    );
    ir.elements.push_back(
        InstructionElement{
            0x204,
            Instruction::make_sprite_for( Reg {4} )
        }
    );
	ir.symbols.add( DecodedSymbol { 0x202, eSymbolKind::JUMP } );

    ir.elements.push_back(
        DataElement{
            0x206,
            { 0x12, 0x34, 0x56, 0x78 }
        }
    );
	ir.symbols.add( DecodedSymbol { 0x204, eSymbolKind::I_TARGET } );

	ir.symbols.sort_vectors();

	{
		std::cout << "\n===== ASM EMITTER OUTPUT =====\n";
		ASMEmitter emitter;
		emitter.configure( { "Test program", false } );

		emitter.emit(std::cout, ir);

		std::cout << "=============================\n";
	}
	{
		std::cout << "\n===== ASM EMITTER OUTPUT =====\n";
		ASMEmitter emitter;
		emitter.configure( { "Test program (clean)", true } );

		emitter.emit(std::cout, ir);

		std::cout << "=============================\n";
	}
}

TEST(ASMEmitterTest, all_opcodes_visual_inspection)
{
    BinImage image;  // dummy binary for optional byte printing

    IRProgram ir;
    ir.origin = 0x200;
    ir.binary = &image;

    uint16_t addr = ir.origin;

    auto push_instruction = [&](Instruction inst, std::vector<uint8_t> dummy_bytes = {0x12,0x34}) {
        ir.elements.push_back(InstructionElement{addr, inst});
        image.insert(image.end(), dummy_bytes.begin(), dummy_bytes.end());
        addr += 2;
    };

    // Build one of each opcode using your exact make_XXX methods
    push_instruction(Instruction::make_nop());
    push_instruction(Instruction::make_clear());
    push_instruction(Instruction::make_return());
    push_instruction(Instruction::make_jump(Addr{0x300}));
    push_instruction(Instruction::make_call(Addr{0x300}));
    push_instruction(Instruction::make_skip_eq(Reg{1}, Imm{0x42}));
    push_instruction(Instruction::make_skip_neq(Reg{2}, Imm{0x99}));
    push_instruction(Instruction::make_skip_eq(Reg{3}, Reg{4}));
    push_instruction(Instruction::make_ld(Reg{0}, Imm{0x10}));
    push_instruction(Instruction::make_add(Reg{1}, Imm{0x20}));
    push_instruction(Instruction::make_ld(Reg{2}, Reg{3}));
    push_instruction(Instruction::make_or(Reg{4}, Reg{5}));
    push_instruction(Instruction::make_and(Reg{6}, Reg{7}));
    push_instruction(Instruction::make_xor(Reg{0}, Reg{1}));
    push_instruction(Instruction::make_add(Reg{2}, Reg{3}));
    push_instruction(Instruction::make_sub(Reg{4}, Reg{5}));
    push_instruction(Instruction::make_shift_right(Reg{6}, Reg{7}));
    push_instruction(Instruction::make_subn(Reg{0}, Reg{1}));
    push_instruction(Instruction::make_shift_left(Reg{2}, Reg{3}));
    push_instruction(Instruction::make_skip_neq(Reg{4}, Reg{5}));
    push_instruction(Instruction::make_ld_i(Addr{0x400}));
    push_instruction(Instruction::make_jump_indexed(Addr{0x400}));
    push_instruction(Instruction::make_rnd(Reg{1}, Imm{0xAA}));
    push_instruction(Instruction::make_drw(Reg{2}, Reg{3}, Nibble{0xF}));
    push_instruction(Instruction::make_skip_if_key(Key{0x5}));
    push_instruction(Instruction::make_skip_not_key(Key{0x6}));
    push_instruction(Instruction::make_load_delay_timer(Reg{0}));
    push_instruction(Instruction::make_load_sound_timer(Reg{1}));
    push_instruction(Instruction::make_store_key(Reg{2}));
    push_instruction(Instruction::make_store_delay_timer(Reg{3}));
    push_instruction(Instruction::make_add_i(Reg{4}));
    push_instruction(Instruction::make_sprite_for(Reg{5}));
    push_instruction(Instruction::make_bcd(Reg{6}));
    push_instruction(Instruction::make_save_regs(RegCount{3}));
    push_instruction(Instruction::make_load_regs(RegCount{2}));

    // Add some sample data bytes
    ir.elements.push_back(DataElement{addr, {0xDE, 0xAD, 0xBE, 0xEF}});
    image.insert(image.end(), {0xDE, 0xAD, 0xBE, 0xEF});
    addr += 4;

    // Add labels for some instructions to see labeling
    ir.symbols.add(DecodedSymbol{0x202, eSymbolKind::JUMP});
    ir.symbols.add(DecodedSymbol{0x220, eSymbolKind::I_TARGET});
    ir.symbols.sort_vectors();

    // Emit full and clean versions
    std::cout << "\n===== ASM EMITTER OUTPUT =====\n";
    ASMEmitter emitter;
    emitter.configure({ "All opcodes test", false });
    emitter.emit(std::cout, ir);
    std::cout << "=============================\n";

    std::cout << "\n===== ASM EMITTER OUTPUT (clean) =====\n";
    ASMEmitter emitter_clean;
    emitter_clean.configure({ "All opcodes test (clean)", true });
    emitter_clean.emit(std::cout, ir);
    std::cout << "=============================\n";
}
