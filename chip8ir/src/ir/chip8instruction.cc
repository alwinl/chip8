/*
* chip8ir.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/chip8instruction.h"

#include <cassert>

#include <iomanip>
#include <algorithm>

struct OperandSpec
{
	enum class Type {  Addr, Reg, Imm, Nibble };

	Type type;
	uint8_t shift_by;
};

struct OpData
{
	std::string mnemonic;
	uint16_t base;
	uint16_t op_count;
	std::array<OperandSpec, 3> operands;
};

static const std::array<OpData, opcode_count> base =
{
	/* OPCODE::NOP */		OpData{ "NOP",    0x0000, 0, {{ {}, {}, {} }} },
	/* Opcode::CLS */		OpData{ "CLS",    0x00E0, 0, {{ {}, {}, {} }} },
	/* Opcode::RET */		OpData{ "RET",    0x00EE, 0, {{ {}, {}, {} }} },
	/* Opcode::JP */		OpData{ "JP",     0x1000, 1, {{ { OperandSpec::Type::Addr, 0 }, {}, {} }} },
	/* Opcode::CALL */		OpData{ "CALL",   0x2000, 1, {{ { OperandSpec::Type::Addr, 0 }, {}, {} }} },
	/* Opcode::SE_Imm */	OpData{ "SE",     0x3000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Imm, 0 }, {} }} },
	/* Opcode::SNE_Imm */	OpData{ "SNE",    0x4000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Imm, 0 }, {} }} },
	/* Opcode::SE_Reg */	OpData{ "SE",     0x5000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::LD_Imm */	OpData{ "LD",     0x6000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Imm, 0 }, {} }} },
	/* Opcode::ADD_Imm */	OpData{ "ADD",    0x7000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Imm, 0 }, {} }} },
	/* Opcode::LD_Reg */	OpData{ "LD",     0x8000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::OR */		OpData{ "OR",     0x8001, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::AND */		OpData{ "AND",    0x8002, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::XOR */		OpData{ "XOR",    0x8003, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::ADD_Reg */	OpData{ "ADD",    0x8004, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::SUB */		OpData{ "SUB",    0x8005, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::SHR */		OpData{ "SHR",    0x8006, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::SUBN */		OpData{ "SUBN",   0x8007, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::SHL */		OpData{ "SHL",    0x800E, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::SNE_Reg */	OpData{ "SNE",    0x9000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, {} }} },
	/* Opcode::LD_I */		OpData{ "LD I",   0xA000, 1, {{ { OperandSpec::Type::Addr, 0 }, {}, {} }} },
	/* Opcode::JP_V0 */		OpData{ "JP V0",  0xB000, 1, {{ { OperandSpec::Type::Addr, 0 }, {}, {} }} },
	/* Opcode::RND */		OpData{ "RND",    0xC000, 2, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Imm, 0 }, {} }} },
	/* Opcode::DRW */		OpData{ "DRW",    0xD000, 3, {{ { OperandSpec::Type::Reg, 8 }, { OperandSpec::Type::Reg, 4 }, { OperandSpec::Type::Nibble, 0 } }} },
	/* Opcode::SKP */		OpData{ "SKP",    0xE09E, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::SKNP */		OpData{ "SKNP",   0xE0A1, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::LD_DT */		OpData{ "LD DT",  0xF015, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::LD_ST */		OpData{ "LD ST",  0xF018, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::ST_KEY */	OpData{ "ST K",   0xF00A, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::ST_DT */		OpData{ "ST DT",  0xF007, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::ADD_I */		OpData{ "ADD I",  0xF01E, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::LD_SPRITE */	OpData{ "LD F",   0xF029, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::BCD */		OpData{ "LD B",   0xF033, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::ST_REGS */	OpData{ "ST [I]", 0xF055, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} },
	/* Opcode::LD_REGS */	OpData{ "LD [I]", 0xF065, 1, {{ { OperandSpec::Type::Reg, 8 }, {}, {} }} }
};

bool operator==( const Operand& a, const Operand& b )
{
    if( a.index() != b.index() )
		return false;

    return std::visit(
		[]( auto&& lhs, auto&& rhs ) -> bool
		{
			using L = std::decay_t<decltype(lhs)>;
			using R = std::decay_t<decltype(rhs)>;

			if constexpr (!std::is_same_v<L,R>)
				return false;
			else
				return lhs == rhs;
    	}, a, b
	);
}

bool operator==(const Instruction& a, const Instruction& b)
{
    if( a.opcode_ != b.opcode_ )
        return false;

    if( a.operand_count_ != b.operand_count_ )
        return false;

    for( size_t i = 0; i < a.operand_count_; ++i )
		if( !(a.operands_[i] == b.operands_[i]) )
			return false;

    return true;
}

std::ostream& operator<<(std::ostream& os, const Operand& op)
{
    std::visit( [&os](auto&& v)
	{
        using T = std::decay_t<decltype(v)>;

		auto flags = os.flags();
		auto fill  = os.fill();

		if constexpr( std::is_same_v<T, Reg> )
			os << "V" << std::uppercase << std::hex << +v.get_index();

		else if constexpr( std::is_same_v<T, Addr> )
			os << "0x" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << +v.get_value();

		else if constexpr( std::is_same_v<T, Imm> )
			os << "#" << std::dec << +v.get_value();

		else if constexpr( std::is_same_v<T, Nibble> )
			os << "0x" << std::uppercase << std::hex << +v.get_value();

		os.fill(fill);
		os.flags(flags);

    }, op);

    return os;
}

std::ostream& operator<<(std::ostream& os, const Instruction& instr)
{
    os << "Opcode: " << base.at(static_cast<size_t>(instr.opcode())).mnemonic;
    os << " Operands: [";

    bool first = true;
    for( auto& op : instr.operands() )
	{
        if(!first)
			os << ", ";
		else
			first = false;

        os << op;
    }

    os << "]";

    return os;
}

uint16_t Instruction::to_binary( ) const
{
	const auto& op_data = base.at(static_cast<size_t>(opcode_));

	assert( operand_count_ == op_data.op_count );

	uint16_t result = op_data.base;

	for( size_t idx = 0; idx < op_data.op_count; ++idx ) {

		const auto shift = op_data.operands[idx].shift_by;

		switch( op_data.operands[idx].type ) {
		case OperandSpec::Type::Addr:
			result |= (std::get<Addr>(operands_[idx]).get_value() << shift);
			break;
		case OperandSpec::Type::Reg:
			result |= (std::get<Reg>(operands_[idx]).get_index() << shift );
			break;
		case OperandSpec::Type::Imm:
			result |= (std::get<Imm>(operands_[idx]).get_value() << shift );
			break;
		case OperandSpec::Type::Nibble:
			result |= (std::get<Nibble>(operands_[idx]).get_value() << shift );
			break;
		}
	}

	return result;
}
