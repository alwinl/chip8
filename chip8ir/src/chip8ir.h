/*
 * chip8ir.h Copyright 2026 Alwin Leerling <dna.leerling@gmail.com>
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

#pragma once

#include <cstdint>
#include <cstddef>
#include <variant>
#include <span>
#include <cassert>
#include <ostream>
#include <algorithm>
#include <vector>
#include <optional>

enum class Opcode
{
	NOP, CLS, RET, JP, CALL, SE,
	SNE, LD, ADD, OR, AND,
	XOR, SUB, SHR, SUBN, SHL, LD_I,
	JP_V0, RND, DRW, SKP, SKNP, LD_DT,
	LD_ST, ST_KEY, ST_DT, ADD_I,
	LD_SPRITE, BCD, ST_REGS, LD_REGS
};

constexpr size_t opcode_count = (size_t)Opcode::LD_REGS + 1;

struct Reg      { uint8_t index;  }; // V0–VF
struct Addr     { uint16_t value; }; // 0x000–0xFFF
struct Imm      { uint8_t value;  }; // NN
struct Nibble   { uint8_t value;  }; // N (sprite height)
struct Key      { uint8_t index;  }; // key id
struct RegCount { uint8_t count;  }; // 0 - 15

inline bool operator==( const Reg& a, const Reg& b )           { return a.index == b.index; }
inline bool operator==( const Addr& a, const Addr& b )         { return a.value == b.value; }
inline bool operator==( const Imm a, const Imm& b )            { return a.value == b.value; }
inline bool operator==( const Nibble& a, const Nibble& b )     { return a.value == b.value; }
inline bool operator==( const Key& a, const Key& b )           { return a.index == b.index; }
inline bool operator==( const RegCount& a, const RegCount& b ) { return a.count == b.count; }

using Operand = std::variant< Reg, Addr, Imm, Nibble, Key, RegCount >;

class Instruction
{
public:
	static Instruction make_nop()                              {                                         return Instruction( Opcode::NOP,       { } ); }
	static Instruction make_clear()                            {                                         return Instruction( Opcode::CLS,       { } ); }
	static Instruction make_return()                           {                                         return Instruction( Opcode::RET,       { } ); }
	static Instruction make_jump( Addr addr )                  { assert( addr.value < 0x1000 );          return Instruction( Opcode::JP,        { addr } ); }
	static Instruction make_call( Addr addr )                  { assert( addr.value < 0x1000 );          return Instruction( Opcode::CALL,      { addr } ); }
	static Instruction make_skip_eq( Reg x, Imm byte )         { assert( x.index < 16 );                 return Instruction( Opcode::SE,        { x, byte } ); }
	static Instruction make_skip_neq( Reg x, Imm byte )        { assert( x.index < 16 );                 return Instruction( Opcode::SNE,       { x, byte } ); }
	static Instruction make_skip_eq( Reg x, Reg y )            { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::SE,        { x, y } ); }
	static Instruction make_ld( Reg x, Imm byte )              { assert( x.index < 16);                  return Instruction( Opcode::LD,        { x, byte } ); }
	static Instruction make_add( Reg x, Imm byte )             { assert( x.index < 16);                  return Instruction( Opcode::ADD,       { x, byte } ); }
	static Instruction make_ld( Reg x, Reg y )                 { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::LD,        { x, y } ); }
	static Instruction make_or( Reg x, Reg y )                 { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::OR,        { x, y } ); }
	static Instruction make_and( Reg x, Reg y )                { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::AND,       { x, y } ); }
	static Instruction make_xor( Reg x, Reg y )                { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::XOR,       { x, y } ); }
	static Instruction make_add( Reg x, Reg y )                { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::ADD,       { x, y } ); }
	static Instruction make_sub( Reg x, Reg y )                { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::SUB,       { x, y } ); }
	static Instruction make_shift_right( Reg x, Reg y )        { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::SHR,       { x, y } ); }
	static Instruction make_subn( Reg x, Reg y )               { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::SUBN,      { x, y } ); }
	static Instruction make_shift_left( Reg x, Reg y )         { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::SHL,       { x, y } ); }
	static Instruction make_skip_neq( Reg x, Reg y )           { assert( x.index < 16 && y.index < 16 ); return Instruction( Opcode::SNE,       { x, y } ); }
	static Instruction make_ld_i( Addr addr )                  { assert( addr.value < 0x1000 );          return Instruction( Opcode::LD_I,      { addr } ); }
	static Instruction make_jump_indexed( Addr addr )          { assert( addr.value < 0x1000 );          return Instruction( Opcode::JP_V0,     { addr } ); }
	static Instruction make_rnd( Reg x, Imm byte )             { assert( x.index < 16 );                 return Instruction( Opcode::RND,       { x, byte } ); }
	static Instruction make_drw( Reg x, Reg y, Nibble nibble ) { assert( x.index < 16 && y.index < 16 && nibble.value < 16 );
		                                                                                                 return Instruction( Opcode::DRW,       { x, y, nibble } ); }
	static Instruction make_skip_if_key( Key key )             { assert( key.index < 16 );               return Instruction( Opcode::SKP,       { key } ); }
	static Instruction make_skip_not_key( Key key )            { assert( key.index < 16 );               return Instruction( Opcode::SKNP,      { key } ); }
	static Instruction make_load_delay_timer( Reg x )          { assert( x.index < 16 );                 return Instruction( Opcode::LD_DT,     { x } ); }
	static Instruction make_load_sound_timer( Reg x )          { assert( x.index < 16 );                 return Instruction( Opcode::LD_ST,     { x } ); }
	static Instruction make_store_key( Reg x )                 { assert( x.index < 16 );                 return Instruction( Opcode::ST_KEY,    { x } ); }
	static Instruction make_store_delay_timer( Reg x )         { assert( x.index < 16 );                 return Instruction( Opcode::ST_DT,     { x } ); }
	static Instruction make_add_i( Reg x )                     { assert( x.index < 16 );                 return Instruction( Opcode::ADD_I,     { x } ); }
	static Instruction make_sprite_for( Reg x )                { assert( x.index < 16 );                 return Instruction( Opcode::LD_SPRITE, { x } ); }
	static Instruction make_bcd( Reg x )                       { assert( x.index < 16 );                 return Instruction( Opcode::BCD,       { x } ); }
	static Instruction make_save_regs( RegCount rc )           { assert( rc.count < 16 );                return Instruction( Opcode::ST_REGS,   { rc } ); }
	static Instruction make_load_regs( RegCount rc )           { assert( rc.count < 16 );                return Instruction( Opcode::LD_REGS,   { rc } ); }


	Opcode opcode() const { return opcode_; }
	std::span<const Operand> operands() const { return { operands_.data(), operand_count_ }; }

	friend bool operator==(const Instruction& a, const Instruction& b);

private:
    Opcode opcode_;
    std::array<Operand,3> operands_;
	size_t operand_count_ = 0;

	Instruction( Opcode opcode, std::initializer_list<Operand> operands ) : opcode_(opcode), operand_count_(operands.size())
	{
		assert( operands.size() <= 3 );
        std::copy_n( operands.begin(), operand_count_, operands_.begin() );
	}
};

bool operator==(const Instruction& a, const Instruction& b);

std::ostream& operator<<(std::ostream& os, const Opcode& mnemonic );
std::ostream& operator<<(std::ostream& os, const Operand& op);
std::ostream& operator<<(std::ostream& os, const Instruction& instr);

struct DataElement
{
	uint16_t address;
	std::vector<uint8_t> byte_run;
};

struct InstructionElement
{
	uint16_t address;
	Instruction instruction;
};

using ASMElement = std::variant<InstructionElement, DataElement>;

enum eSymbolKind { I_TARGET, SUBROUTINE, JUMP, INDEXED, COUNT };

struct DecodedSymbol
{
	uint16_t address;
	eSymbolKind kind;
};


class SymbolTable
{
public:
	SymbolTable() = default;

	void add( std::optional<DecodedSymbol> symbol )
	{
		if( symbol )
			symbol_lists[symbol->kind].push_back( symbol->address );
	}

	void sort_vectors()
	{
		for( auto& symbol_list : symbol_lists ) {
			std::sort( symbol_list.begin(), symbol_list.end() );
			symbol_list.erase( std::unique( symbol_list.begin(), symbol_list.end()), symbol_list.end() );
		}
		sorted = true;
	}

	std::string get_label( uint16_t address ) const
	{
		assert( sorted );

		static constexpr std::array<const char *,eSymbolKind::COUNT> prefixes{ "DATA", "FUNC", "LABEL", "TABLE" };

		for( size_t kind = 0; kind < eSymbolKind::COUNT; ++ kind ) {

			auto it = std::lower_bound( symbol_lists[kind].begin(), symbol_lists[kind].end(), address );
			if( (it != symbol_lists[kind].end()) && (*it == address) )
				return std::string(prefixes[kind]) + std::to_string( std::distance( symbol_lists[kind].begin(), it) );
		}

		return {};
	}


	std::vector<uint16_t> get_index_list( ) { return symbol_lists[INDEXED]; };

private:
	using ListArray = std::array<std::vector<uint16_t>, eSymbolKind::COUNT>;

	bool sorted = false;
	ListArray symbol_lists;

};

using ASMSource = std::vector<std::string>;
using C8CSource = std::vector<std::string>;

struct IRProgram
{
    uint16_t origin = 0x200;
    std::vector<ASMElement> elements;
    SymbolTable symbols;
};
