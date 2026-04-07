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

template<typename T>
uint16_t evaluate_expression( const T&, const ASMSymbolTable& )
{
    static_assert(std::is_same_v<T, void>, "Unhandled expression type");
    return 0;
}

uint16_t evaluate_expression( const NumberExpr& expr, const ASMSymbolTable& symbols )
{
	return expr.value;
}

uint16_t evaluate_expression( const IdentifierExpr& expr, const ASMSymbolTable& symbols )
{
	return symbols.get_value( expr.text );
}

uint16_t evaluate_expression( const ASTExpression& expr, const ASMSymbolTable& symbols )
{
	return std::visit( [&]( auto&& expression ) -> uint16_t { return evaluate_expression( expression, symbols ); }, expr.expression );
}

uint16_t evaluate_expression( const ASTBinaryExpr& expr, const ASMSymbolTable& symbols )
{
	auto lhs = evaluate_expression( *expr.lhs, symbols );
	auto rhs = evaluate_expression( *expr.rhs, symbols );

	switch( expr.op ) {
	case '+': return lhs + rhs;
	case '-': return lhs - rhs;
	case '*': return lhs * rhs;
	case '/': return rhs!= 0 ? lhs / rhs : 0;
	}

	return 0;
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


Reg parse_reg( const ASTExpression& expr )
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

Addr parse_addr( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
	return Addr{ evaluate_expression( expr, symbols ) };
}

Imm parse_imm( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    return Imm{ static_cast<uint8_t>( evaluate_expression( expr, symbols) ) };
}

Nibble parse_nibble( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    auto val = evaluate_expression( expr, symbols );
    if (val > 0xF)
		throw std::runtime_error("Nibble out of range");

    return Nibble{ static_cast<uint8_t>(val) };
}

Key parse_key( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    auto val = evaluate_expression(expr, symbols);
    if (val > 0xF)
		throw std::runtime_error("Key out of range");

    return Key{ static_cast<uint8_t>(val) };
}

RegCount parse_regcount( const ASTExpression& expr, const ASMSymbolTable &symbols )
{
    auto val = evaluate_expression( expr, symbols );
    if (val > 0xF)
		throw std::runtime_error("RegCount out of range");

    return RegCount{ static_cast<uint8_t>(val) };
}

const Dispatcher& get_dispatcher()
{
	static const Dispatcher dispatcher =
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
			return Instruction::make_call( parse_addr( ast_inst.operands[0], symbols ));
		} },

		{ "OR",   []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_or( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "AND",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_and( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "XOR",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_xor( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "SUB",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_sub( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "SHR",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_shift_right( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "SUBN", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_subn( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "SHL",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_shift_left( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ));
		} },

		{ "RND",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_rnd( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ) );
		} },

		{ "DRW",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_drw( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ), parse_nibble( ast_inst.operands[2], symbols ));
		} },

		{ "SKP",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_skip_if_key( parse_key( ast_inst.operands[0], symbols ) );
		} },

		{ "SKNP", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return Instruction::make_skip_not_key( parse_key( ast_inst.operands[0], symbols ) );
		} },

		{ "SNE",  []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return ( is_register( ast_inst.operands[1] ) ) ?
				Instruction::make_skip_neq( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ) ) :
				Instruction::make_skip_neq( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ) );
		} },

		{ "SE", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			return ( is_register( ast_inst.operands[1] ) ) ?
				Instruction::make_skip_eq( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ) ) :
				Instruction::make_skip_eq( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ) );
		} },

		{ "JP", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			if( is_register( ast_inst.operands[0] ) ) {

				if( !is_identifier(ast_inst.operands[0], "V0") )
					throw std::runtime_error("JP indexed must use V0");

				return Instruction::make_jump_indexed( parse_addr( ast_inst.operands[1], symbols ));
			}

			if( ast_inst.operands.size() != 1 )
				throw std::runtime_error("Error in JP statement");

			return Instruction::make_jump( parse_addr( ast_inst.operands[0], symbols ));
		} },

		{ "ADD", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {
			if( is_identifier( ast_inst.operands[0], "I" ) ) return Instruction::make_add_i( parse_reg( ast_inst.operands[1] ) );

			return ( is_register( ast_inst.operands[1] ) ) ?
				Instruction::make_add( parse_reg( ast_inst.operands[0] ), parse_reg( ast_inst.operands[1] ) ) :
				Instruction::make_add( parse_reg( ast_inst.operands[0] ), parse_imm( ast_inst.operands[1], symbols ));
		} },
		{ "LD", []( const ASTInstruction &ast_inst, const ASMSymbolTable &symbols ) {

			if( is_register( ast_inst.operands[0] ) ) {

				if( is_identifier( ast_inst.operands[1], "K" ) ) return Instruction::make_store_key( parse_reg( ast_inst.operands[0]) );
				if( is_identifier( ast_inst.operands[1], "DT" ) ) return Instruction::make_store_delay_timer( parse_reg( ast_inst.operands[0]) );
				if( is_identifier( ast_inst.operands[1], "[I]" ) ) return Instruction::make_load_regs( parse_regcount( ast_inst.operands[0], symbols) );

				return( is_register( ast_inst.operands[1] ) ) ?
					Instruction::make_ld( parse_reg( ast_inst.operands[0]), parse_reg( ast_inst.operands[1]) ) :
					Instruction::make_ld( parse_reg( ast_inst.operands[0]), parse_imm( ast_inst.operands[1], symbols) );
			}

			if( is_identifier( ast_inst.operands[0], "DT" ) ) return Instruction::make_load_delay_timer( parse_reg( ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "ST" ) ) return Instruction::make_load_sound_timer( parse_reg( ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "F" ) ) return Instruction::make_sprite_for( parse_reg( ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "B" ) ) return Instruction::make_bcd( parse_reg( ast_inst.operands[1]) );
			if( is_identifier( ast_inst.operands[0], "[I]" ) ) return Instruction::make_save_regs( parse_regcount( ast_inst.operands[1], symbols ) );
			if( is_identifier( ast_inst.operands[0], "I" ) ) return Instruction::make_ld_i( parse_addr( ast_inst.operands[1], symbols) );

			throw std::runtime_error("Invalid LD form");
		} },
	};

	return dispatcher;
}

uint16_t process_pass1( const ASTInstruction& instruction, uint16_t address, ASMSymbolTable& symbols )
{
	return address + 2;
}

uint16_t process_pass2( const ASTInstruction& instruction, uint16_t address, IRBundle& bundle )
{
	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);
	const auto& dispatcher = get_dispatcher();
	auto entry = dispatcher.find( instruction.mnemonic );

	if( entry == dispatcher.end() )
		throw std::runtime_error( "Unknown mnemonic: " + instruction.mnemonic );

	bundle.ir.elements.push_back( InstructionElement { address, entry->second( instruction, symbols ) } );

	return address + 2;
}

uint16_t process_pass1( const ASTDirective& directive, uint16_t address, ASMSymbolTable& symbols )
{
	if( directive.name == ".DB" )
		address += directive.args.size();

	if( directive.name == ".ORG" )
		address = evaluate_expression( directive.args[0], symbols );

	return address;
}

uint16_t process_pass2( const ASTDirective& directive, uint16_t address, IRBundle& bundle )
{
	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);

	if( directive.name == ".DB" ) {

		std::vector<uint8_t> byte_run;

		for( size_t i = 0; i < directive.args.size(); ++i )
			byte_run.emplace_back( evaluate_expression( directive.args[i], symbols ));

		bundle.ir.elements.push_back( DataElement {address, byte_run } );

		address += directive.args.size();
	}

	if( directive.name == ".ORG" )
		address = evaluate_expression( directive.args[0], symbols );

	return address;
}

uint16_t process_pass1( const ASTEqu& equ, uint16_t address, ASMSymbolTable& symbols )
{
	symbols.define_constant( equ.name, evaluate_expression( equ.value, symbols ) );

	return address;
}

uint16_t process_pass2( const ASTEqu&, uint16_t address, IRBundle& bundle )
{
	return address;
}

uint16_t process_pass1( const ASTEmpty&, uint16_t address, ASMSymbolTable& symbols )
{
	return address;
}

uint16_t process_pass2( const ASTEmpty&, uint16_t address, IRBundle& bundle )
{
	return address;
}

IRBundle Assembler::build_ir( ASMSource source )
{
    ASMTokens tokens = ASMTokeniser().tokenise(source);
    ASTProgram program = ASMParser().parse_asm_tokens(tokens);

	IRBundle bundle { {}, std::make_unique<ASMSymbolTable>() };

	ASMSymbolTable& symbols = static_cast<ASMSymbolTable&>(*bundle.resolver);

	// Pass 1
	uint16_t address = 0x200;

	for( const ASTElement& element : program ) {

		if( element.label )
			symbols.define_label( element.label->name, address );

		address = std::visit( [&]( auto&& body ) -> uint16_t { return process_pass1( body, address, symbols ); }, element.body );
	}

	// Pass 2
	address = 0x200;

	for( const ASTElement& element : program )
		address = std::visit( [&]( auto&& body ) -> uint16_t { return process_pass2( body, address, bundle ); }, element.body );

	return bundle;
}
