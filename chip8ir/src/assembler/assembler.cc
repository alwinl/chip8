/*
 * assembler.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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


#include "assembler/assembler.h"

#include "assembler/tokeniser.h"
#include "assembler/parser.h"
#include "assembler/symbol_table.h"

uint16_t evaluate_expression( const IRBundle& bundle, const ASTExpression& expr )
{
	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);

	return std::visit( [&]( auto&& expression ) -> uint16_t
	{
		using T = std::decay_t<decltype(expression)>;

		if constexpr ( std::is_same_v<T, NumberExpr> )
		{
			return expression.value;
		}
		else if constexpr ( std::is_same_v<T, IdentifierExpr> )
		{
			return symbols.get_value( expression.text );
		}
		else if constexpr ( std::is_same_v<T, ASTBinaryExpr> )
		{
			auto lhs = evaluate_expression( bundle, *expression.lhs.get() );
			auto rhs = evaluate_expression( bundle, *expression.rhs.get() );

			switch( expression.op ) {
			case '+': return lhs + rhs;
			case '-': return lhs - rhs;
			case '*': return lhs * rhs;
			case '/': return rhs!= 0 ? lhs / rhs : 0;
			}

			return 0;
		}
		return 0;

	}, expr.expression );
}

bool is_register( const ASTExpression& expr )
{
    if( !std::holds_alternative<IdentifierExpr>(expr.expression) )
        return false;

    const auto& id = std::get<IdentifierExpr>(expr.expression).text;
    return !id.empty() && id[0] == 'V';
}

bool is_identifier( const ASTExpression& expr, std::string content )
{
    if( !std::holds_alternative<IdentifierExpr>(expr.expression) )
        return false;

    const auto& id = std::get<IdentifierExpr>(expr.expression).text;
    return !id.empty() && content == id;
}

Reg parse_reg( const IRBundle &bundle, const ASTExpression& expr )
{
    if (!std::holds_alternative<IdentifierExpr>(expr.expression))
        throw std::runtime_error("Expected register");

    const auto& id = std::get<IdentifierExpr>(expr.expression).text;

    if (id.size() < 2 || id[0] != 'V')
        throw std::runtime_error("Invalid register: " + id);

    int index = std::stoi(id.substr(1));

    if (index < 0 || index > 15)
        throw std::runtime_error("Register out of range");

    return Reg{ static_cast<uint8_t>(index) };
}

Addr parse_addr( const IRBundle &bundle, const ASTExpression& expr )
{
	return Addr{ evaluate_expression( bundle, expr) };
}

Imm parse_imm( const IRBundle &bundle, const ASTExpression& expr )
{
    return Imm{ static_cast<uint8_t>( evaluate_expression( bundle, expr) ) };
}

Nibble parse_nibble( const IRBundle &bundle, const ASTExpression& expr )
{
    auto val = evaluate_expression(bundle, expr);
    if (val > 0xF)
		throw std::runtime_error("Nibble out of range");

    return Nibble{ static_cast<uint8_t>(val) };
}

Key parse_key( const IRBundle &bundle, const ASTExpression& expr )
{
    auto val = evaluate_expression(bundle, expr);
    if (val > 0xF)
		throw std::runtime_error("Key out of range");

    return Key{ static_cast<uint8_t>(val) };
}

RegCount parse_regcount( const IRBundle &bundle, const ASTExpression& expr )
{
    auto val = evaluate_expression(bundle, expr);
    if (val > 0xF)
		throw std::runtime_error("RegCount out of range");

    return RegCount{ static_cast<uint8_t>(val) };
}

const auto& get_dispatcher()
{
	static const std::unordered_map<std::string, std::function<Instruction(const IRBundle &, const ASTInstruction &)>> dispatcher =
	{
		{ "NOP",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_nop();
		} },

		{ "CLS",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_clear();
		} },

		{ "RET",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_return();
		} },

		{ "CALL", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_call( parse_addr( bundle, ast_inst.operands[0] ));
		} },

		{ "OR",   []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_or( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "AND",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_and( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "XOR",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_xor( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "SUB",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_sub( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "SHR",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_shift_right( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "SUBN", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_subn( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "SHL",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_shift_left( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ));
		} },

		{ "RND",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_rnd( parse_reg( bundle, ast_inst.operands[0] ), parse_imm( bundle, ast_inst.operands[1] ) );
		} },

		{ "DRW",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_drw( parse_reg( bundle, ast_inst.operands[0] ),  parse_reg( bundle, ast_inst.operands[1] ), parse_nibble( bundle, ast_inst.operands[2] ));
		} },

		{ "SKP",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_skip_if_key( parse_key( bundle, ast_inst.operands[0] ) );
		} },

		{ "SKNP", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return Instruction::make_skip_not_key( parse_key( bundle, ast_inst.operands[0] ) );
		} },

		{ "SNE",  []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return ( is_register( ast_inst.operands[1] ) ) ?
				Instruction::make_skip_neq( parse_reg( bundle, ast_inst.operands[0] ), parse_reg( bundle, ast_inst.operands[1] ) ) :
				Instruction::make_skip_neq( parse_reg( bundle, ast_inst.operands[0] ), parse_imm( bundle, ast_inst.operands[1] ) );
		} },

		{ "SE", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			return ( is_register( ast_inst.operands[1] ) ) ?
				Instruction::make_skip_eq( parse_reg( bundle, ast_inst.operands[0] ), parse_reg( bundle, ast_inst.operands[1] ) ) :
				Instruction::make_skip_eq( parse_reg( bundle, ast_inst.operands[0] ), parse_imm( bundle, ast_inst.operands[1] ) );
		} },

		{ "JP", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			if( is_register( ast_inst.operands[0] ) ) {

				if( !is_identifier(ast_inst.operands[0], "V0") )
					throw std::runtime_error("JP indexed must use V0");

				return Instruction::make_jump_indexed( parse_addr( bundle, ast_inst.operands[1] ));
			}

			if( ast_inst.operands.size() != 1 )
				throw std::runtime_error("Error in JP statement");

			return Instruction::make_jump( parse_addr( bundle, ast_inst.operands[0] ));
		} },

		{ "ADD", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {
			if( is_identifier( ast_inst.operands[0], "I" ) ) return Instruction::make_add_i( parse_reg( bundle, ast_inst.operands[1] ) );

			return ( is_register( ast_inst.operands[1] ) ) ?
				Instruction::make_add( parse_reg( bundle, ast_inst.operands[0] ), parse_reg( bundle, ast_inst.operands[1] ) ) :
				Instruction::make_add( parse_reg( bundle, ast_inst.operands[0] ), parse_imm( bundle, ast_inst.operands[1] ));
		} },
		{ "LD", []( const IRBundle &bundle, const ASTInstruction &ast_inst ) {

			if( is_register( ast_inst.operands[0] ) ) {

				if( is_identifier( ast_inst.operands[1], "K" ) ) return Instruction::make_store_key( parse_reg(bundle, ast_inst.operands[0]) );
				if( is_identifier( ast_inst.operands[1], "DT" ) ) return Instruction::make_store_delay_timer( parse_reg(bundle, ast_inst.operands[0]) );
				if( is_identifier( ast_inst.operands[1], "[I]" ) ) return Instruction::make_load_regs( parse_regcount(bundle, ast_inst.operands[0]) );

				return( is_register( ast_inst.operands[1] ) ) ?
					Instruction::make_ld( parse_reg(bundle, ast_inst.operands[0]), parse_reg(bundle, ast_inst.operands[1]) ) :
					Instruction::make_ld( parse_reg(bundle, ast_inst.operands[0]), parse_imm(bundle, ast_inst.operands[1]) );
			}

			if( is_identifier( ast_inst.operands[0], "DT" ) ) return Instruction::make_load_delay_timer( parse_reg(bundle, ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "ST" ) ) return Instruction::make_load_sound_timer( parse_reg(bundle, ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "F" ) ) return Instruction::make_sprite_for( parse_reg(bundle, ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "B" ) ) return Instruction::make_bcd( parse_reg(bundle, ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "[I]" ) ) return Instruction::make_save_regs( parse_regcount(bundle, ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "I" ) ) return Instruction::make_ld_i( parse_addr(bundle, ast_inst.operands[1]) );

			throw std::runtime_error("Invalid LD form");
		} },
	};

	return dispatcher;
}

IRBundle Assembler::build_ir( ASMSource source )
{
    ASMTokens tokens = ASMTokeniser().tokenise(source);
    ASTProgram program = ASMParser().parse_asm_tokens(tokens);

	IRBundle bundle { {}, std::make_unique<ASMSymbolTable>() };

	process_pass1( bundle, program );
	process_pass2( bundle, program );

	return bundle;
}

void Assembler::process_pass1( IRBundle& bundle, const ASTProgram& program )
{
	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);
	uint16_t address = 0x200;

	for( const ASTElement& element : program )
	{
		if( element.label )
			symbols.define_label( element.label->name, address );

		std::visit( [&]( auto&& body )
		{
			using T = std::decay_t<decltype(body)>;

			if constexpr ( std::is_same_v<T, ASTInstruction> )
			{
				address += 2;
			}
			else if constexpr ( std::is_same_v<T, ASTDirective> )
			{
				if( body.name == ".DB" )
					address += body.args.size();
				if( body.name == ".ORG" )
					address = evaluate_expression( bundle, body.args[0] );
			}
			else if constexpr ( std::is_same_v<T, ASTEqu> )
			{
				symbols.define_constant( body.name, evaluate_expression( bundle, body.value ) );
			}

		}, element.body );
	}
}

void Assembler::process_pass2( IRBundle& bundle, const ASTProgram& program )
{
	uint16_t address = 0x200;

	for( const ASTElement& element : program ) {

		std::visit( [&]( auto&& body )
		{
			using T = std::decay_t<decltype(body)>;

			if constexpr ( std::is_same_v<T, ASTInstruction> )
			{
				auto entry = get_dispatcher().find( body.mnemonic );

				if( entry == get_dispatcher().end() )
					throw std::runtime_error( "Unknown mnemonic: " + body.mnemonic );

				bundle.ir.elements.push_back( InstructionElement { address, entry->second( bundle, body ) } );

				address += 2;
			}
			else if constexpr ( std::is_same_v<T, ASTDirective> )
			{
				if( body.name == ".DB" ) {

					std::vector<uint8_t> byte_run;

					for( size_t i = 0; i < body.args.size(); ++i )
						byte_run.emplace_back( evaluate_expression( bundle, body.args[i] ));

					bundle.ir.elements.push_back( DataElement {address, byte_run } );

					address += body.args.size();
				}

				if( body.name == ".ORG" )
					address = evaluate_expression( bundle, body.args[0] );
			}

		}, element.body );

	}
}
