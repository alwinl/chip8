/*
 * chip8instruction.h Copyright 2026 Alwin Leerling <dna.leerling@gmail.com>
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
#include <cassert>
#include <variant>
#include <span>
#include <ostream>
#include <algorithm>
#include <array>

enum class Opcode
{
	NOP, CLS, RET, JP, CALL, SE_Imm,
	SNE_Imm, SE_Reg, LD_Imm, ADD_Imm, LD_Reg, OR, AND,
	XOR, ADD_Reg, SUB, SHR, SUBN, SHL, SNE_Reg, LD_I,
	JP_V0, RND, DRW, SKP, SKNP, LD_DT,
	LD_ST, ST_KEY, ST_DT, ADD_I,
	LD_SPRITE, BCD, ST_REGS, LD_REGS
};

constexpr size_t opcode_count = (size_t)Opcode::LD_REGS + 1;

struct Reg
{
	explicit Reg( uint8_t index ) : index_(index)
	{
		if( index_ > 0x0F )
			throw std::out_of_range("Invalid CHIP-8 register");
	}

	uint8_t get_index() const { return index_; }

private:
	uint8_t index_;

	friend bool operator==( const Reg& a, const Reg& b );
};

struct Addr
{
	explicit Addr( uint16_t value ) : value_(value)
	{
		if( value_ > 0x0FFF )
			throw std::out_of_range("Invalid CHIP-8 Address");
	}
	uint16_t get_value() const { return value_; }

private:
	uint16_t value_;

	friend bool operator==( const Addr& a, const Addr& b );
};

struct Imm
{
	explicit Imm( uint8_t value ) : value_(value) {}

	uint8_t get_value() const { return value_; }

private:
	uint8_t value_;

	friend bool operator==( const Imm& a, const Imm& b );
};

struct Nibble
{
	explicit Nibble( uint8_t value ) : value_(value)
	{
		if( value_ > 0x0F )
			throw std::out_of_range("Invalid CHIP-8 nibble");
	}

	uint8_t get_value() const { return value_; }

private:
	uint8_t value_;

	friend bool operator==( const Nibble& a, const Nibble& b );
};

inline bool operator==( const Reg& a, const Reg& b )           { return a.index_ == b.index_; }
inline bool operator==( const Addr& a, const Addr& b )         { return a.value_ == b.value_; }
inline bool operator==( const Imm& a, const Imm& b )           { return a.value_ == b.value_; }
inline bool operator==( const Nibble& a, const Nibble& b )     { return a.value_ == b.value_; }

using Operand = std::variant< std::monostate, Reg, Addr, Imm, Nibble >;

class Instruction
{
public:
	static Instruction make_nop()                              { return Instruction( Opcode::NOP,       { } ); }
	static Instruction make_clear()                            { return Instruction( Opcode::CLS,       { } ); }
	static Instruction make_return()                           { return Instruction( Opcode::RET,       { } ); }
	static Instruction make_jump( Addr addr )                  { return Instruction( Opcode::JP,        { addr } ); }
	static Instruction make_call( Addr addr )                  { return Instruction( Opcode::CALL,      { addr } ); }
	static Instruction make_skip_eq( Reg x, Imm byte )         { return Instruction( Opcode::SE_Imm,    { x, byte } ); }
	static Instruction make_skip_neq( Reg x, Imm byte )        { return Instruction( Opcode::SNE_Imm,   { x, byte } ); }
	static Instruction make_skip_eq( Reg x, Reg y )            { return Instruction( Opcode::SE_Reg,    { x, y } ); }
	static Instruction make_ld( Reg x, Imm byte )              { return Instruction( Opcode::LD_Imm,    { x, byte } ); }
	static Instruction make_add( Reg x, Imm byte )             { return Instruction( Opcode::ADD_Imm,   { x, byte } ); }
	static Instruction make_ld( Reg x, Reg y )                 { return Instruction( Opcode::LD_Reg,    { x, y } ); }
	static Instruction make_or( Reg x, Reg y )                 { return Instruction( Opcode::OR,        { x, y } ); }
	static Instruction make_and( Reg x, Reg y )                { return Instruction( Opcode::AND,       { x, y } ); }
	static Instruction make_xor( Reg x, Reg y )                { return Instruction( Opcode::XOR,       { x, y } ); }
	static Instruction make_add( Reg x, Reg y )                { return Instruction( Opcode::ADD_Reg,   { x, y } ); }
	static Instruction make_sub( Reg x, Reg y )                { return Instruction( Opcode::SUB,       { x, y } ); }
	static Instruction make_shift_right( Reg x, Reg y )        { return Instruction( Opcode::SHR,       { x, y } ); }
	static Instruction make_subn( Reg x, Reg y )               { return Instruction( Opcode::SUBN,      { x, y } ); }
	static Instruction make_shift_left( Reg x, Reg y )         { return Instruction( Opcode::SHL,       { x, y } ); }
	static Instruction make_skip_neq( Reg x, Reg y )           { return Instruction( Opcode::SNE_Reg,   { x, y } ); }
	static Instruction make_ld_i( Addr addr )                  { return Instruction( Opcode::LD_I,      { addr } ); }
	static Instruction make_jump_indexed( Addr addr )          { return Instruction( Opcode::JP_V0,     { addr } ); }
	static Instruction make_rnd( Reg x, Imm byte )             { return Instruction( Opcode::RND,       { x, byte } ); }
	static Instruction make_drw( Reg x, Reg y, Nibble nibble ) { return Instruction( Opcode::DRW,       { x, y, nibble } ); }
	static Instruction make_skip_if_key( Reg x )               { return Instruction( Opcode::SKP,       { x } ); }
	static Instruction make_skip_not_key( Reg x )              { return Instruction( Opcode::SKNP,      { x } ); }
	static Instruction make_load_delay_timer( Reg x )          { return Instruction( Opcode::LD_DT,     { x } ); }
	static Instruction make_load_sound_timer( Reg x )          { return Instruction( Opcode::LD_ST,     { x } ); }
	static Instruction make_store_key( Reg x )                 { return Instruction( Opcode::ST_KEY,    { x } ); }
	static Instruction make_store_delay_timer( Reg x )         { return Instruction( Opcode::ST_DT,     { x } ); }
	static Instruction make_add_i( Reg x )                     { return Instruction( Opcode::ADD_I,     { x } ); }
	static Instruction make_sprite_for( Reg x )                { return Instruction( Opcode::LD_SPRITE, { x } ); }
	static Instruction make_bcd( Reg x )                       { return Instruction( Opcode::BCD,       { x } ); }
	static Instruction make_save_regs( Reg x )                 { return Instruction( Opcode::ST_REGS,   { x } ); }
	static Instruction make_load_regs( Reg x )                 { return Instruction( Opcode::LD_REGS,   { x } ); }


	Opcode opcode() const { return opcode_; }
	std::span<const Operand> operands() const { return { operands_.data(), operand_count_ }; }

	friend bool operator==(const Instruction& a, const Instruction& b);

	uint16_t to_binary( ) const;

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

// bool operator==( const Operand& a, const Operand& b );
bool operator==( const Instruction& a, const Instruction& b );

// std::ostream& operator<<(std::ostream& os, const Opcode& opcode );
std::ostream& operator<<(std::ostream& os, const Operand& op);
std::ostream& operator<<(std::ostream& os, const Instruction& instr);
