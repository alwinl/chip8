/*
 * instruction_generator.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "assembler/instruction_generator.h"
#include "assembler/expression.h"

#include <unordered_map>
#include <functional>

namespace {

	using InstructionGenerator = std::unordered_map< std::string, std::function<Instruction( const ASTInstruction&, const ASMSymbolTable& )> >;

	bool is_register( const ASTExpression& expr )
	{
		if( !std::holds_alternative<ASTIdentifierExpr>(expr.expression) )
			return false;

		const auto& id = std::get<ASTIdentifierExpr>(expr.expression).text;
		return !id.empty() && id[0] == 'V';
	}

	bool is_identifier( const ASTExpression& expr, std::string content )
	{
		if( !std::holds_alternative<ASTIdentifierExpr>(expr.expression) )
			return false;

		const auto& id = std::get<ASTIdentifierExpr>(expr.expression).text;
		return !id.empty() && content == id;
	}

	void require_operand_count( const ASTInstruction& instruction, std::size_t expected )
	{
		if (instruction.operands.size() != expected)
			throw std::runtime_error(
				"Incorrect operand count for " + instruction.mnemonic);
	}

	Reg parse_reg( const ASTExpression& expr )
	{
		if (!std::holds_alternative<ASTIdentifierExpr>(expr.expression))
			throw std::runtime_error("Expected register");

		const auto& id = std::get<ASTIdentifierExpr>(expr.expression).text;

		if (id.size() != 2 || id[0] != 'V')
			throw std::runtime_error("Invalid register: " + id);

		char c = id[1];

		int index = 0;

		if (c >= '0' && c <= '9')
			index = c - '0';
		else if (c >= 'A' && c <= 'F')
			index = 10 + (c - 'A');
		else if (c >= 'a' && c <= 'f')
			index = 10 + (c - 'a');
		else
			throw std::runtime_error("Invalid register: " + id);

		return Reg{ static_cast<uint8_t>(index) };
	}

	Addr parse_addr( const ASTExpression& expr, const ASMSymbolTable &symbols )
	{
		auto val = evaluate_expression( expr, symbols );
		if (val > 0xFFF)
			throw std::runtime_error("Address out of range");

		return Addr{ val };
	}

	Imm parse_imm( const ASTExpression& expr, const ASMSymbolTable &symbols )
	{
		auto val = evaluate_expression( expr, symbols );
		if (val > 0xFF)
			throw std::runtime_error("Immediate out of range");

		return Imm{ static_cast<uint8_t>(val) };
	}

	Nibble parse_nibble( const ASTExpression& expr, const ASMSymbolTable &symbols )
	{
		auto val = evaluate_expression( expr, symbols );
		if (val > 0xF)
			throw std::runtime_error("Nibble out of range");

		return Nibble{ static_cast<uint8_t>(val) };
	}

	const InstructionGenerator& get_instruction_generators()
	{
		static const InstructionGenerator generators =
		{
			{ "NOP",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				return Instruction::make_nop();
			} },

			{ "CLS",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				return Instruction::make_clear();
			} },

			{ "RET",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				return Instruction::make_return();
			} },

			{ "CALL", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 1 );
				return Instruction::make_call( parse_addr( ast_inst.operands[0], symbols ));
			} },

			{ "OR",   []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_or( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "AND",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_and( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "XOR",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_xor( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "SUB",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_sub( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "SHR",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_shift_right( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "SUBN", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_subn( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "SHL",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_shift_left( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
			} },

			{ "RND",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return Instruction::make_rnd( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ) );
			} },

			{ "DRW",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 3 );
				return Instruction::make_drw( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ), parse_nibble( ast_inst.operands[2], symbols ));
			} },

			{ "SKP",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 1 );
				return Instruction::make_skip_if_key( parse_reg( ast_inst.operands[0] ) );
			} },

			{ "SKNP", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 1 );
				return Instruction::make_skip_not_key( parse_reg( ast_inst.operands[0] ) );
			} },

			{ "SNE",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return ( is_register( ast_inst.operands[1] ) ) ?
					Instruction::make_skip_neq( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ) ) :
					Instruction::make_skip_neq( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ) );
			} },

			{ "SE", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
				require_operand_count( ast_inst, 2 );
				return ( is_register( ast_inst.operands[1] ) ) ?
					Instruction::make_skip_eq( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ) ) :
					Instruction::make_skip_eq( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ) );
			} },

			{ "JP", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {

				if( ast_inst.operands.size() == 0 )
					throw std::runtime_error("Need at least one operand in JP statement");

				if( is_identifier(ast_inst.operands[0], "V0") ) {
					require_operand_count( ast_inst, 2 );
					return Instruction::make_jump_indexed( parse_addr( ast_inst.operands[1], symbols ));
				}

				require_operand_count( ast_inst, 1 );
				return Instruction::make_jump( parse_addr( ast_inst.operands[0], symbols ));
			} },

			{ "ADD", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {

				require_operand_count( ast_inst, 2 );

				if( is_identifier( ast_inst.operands[0], "I" ) ) return Instruction::make_add_i( parse_reg( ast_inst.operands[1] ) );

				return ( is_register( ast_inst.operands[1] ) ) ?
					Instruction::make_add( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ) ) :
					Instruction::make_add( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ));
			} },
			{ "LD", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {

				require_operand_count( ast_inst, 2 );

				if( is_register( ast_inst.operands[0] ) ) {

					if( is_identifier( ast_inst.operands[1], "K" ) ) return Instruction::make_store_key( parse_reg( ast_inst.operands[0]) );
					if( is_identifier( ast_inst.operands[1], "DT" ) ) return Instruction::make_store_delay_timer( parse_reg( ast_inst.operands[0]) );
					if( is_identifier( ast_inst.operands[1], "[I]" ) ) return Instruction::make_load_regs( parse_reg( ast_inst.operands[0]) );

					return( is_register( ast_inst.operands[1] ) ) ?
						Instruction::make_ld( parse_reg( ast_inst.operands[0]), parse_reg( ast_inst.operands[1]) ) :
						Instruction::make_ld( parse_reg( ast_inst.operands[0]), parse_imm( ast_inst.operands[1], symbols) );
				}

				if( is_identifier( ast_inst.operands[0], "DT" ) ) return Instruction::make_load_delay_timer( parse_reg( ast_inst.operands[1]) );
				if( is_identifier( ast_inst.operands[0], "ST" ) ) return Instruction::make_load_sound_timer( parse_reg( ast_inst.operands[1]) );
				if( is_identifier( ast_inst.operands[0], "F" ) ) return Instruction::make_sprite_for( parse_reg( ast_inst.operands[1]) );
				if( is_identifier( ast_inst.operands[0], "B" ) ) return Instruction::make_bcd( parse_reg( ast_inst.operands[1]) );
				if( is_identifier( ast_inst.operands[0], "[I]" ) ) return Instruction::make_save_regs( parse_reg( ast_inst.operands[1] ) );
				if( is_identifier( ast_inst.operands[0], "I" ) ) return Instruction::make_ld_i( parse_addr( ast_inst.operands[1], symbols) );

				throw std::runtime_error("Invalid LD form");
			} },
		};

		return generators;
	}
}

Instruction generate_instruction( const ASTInstruction& instruction, const ASMSymbolTable& symbols )
{
    static auto& generators = get_instruction_generators();

    if( auto entry = generators.find(instruction.mnemonic); entry != generators.end() )
	    return entry->second(instruction, symbols);

    throw std::runtime_error( "Unknown mnemonic: " + instruction.mnemonic );
}
