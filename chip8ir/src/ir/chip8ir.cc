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

#include "ir/chip8ir.h"

#include <algorithm>
#include <cassert>
#include <variant>
#include <iomanip>

bool operator==( const IRProgram& a, const IRProgram& b )
{
	if( a.origin != b.origin )
		return false;

	if( a.elements.size() != b.elements.size() )
		return false;

	for( size_t i = 0; i < a.elements.size(); ++i )
		if( a.elements[i] != b.elements[i] )
			return false;

	return true;
}

bool operator==( const ASMElement& a, const ASMElement& b )
{
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

bool operator==( const DataElement& a, const DataElement& b )
{
	if( a.address != b.address )
		return false;

	if( a.byte_run.size() != b.byte_run.size() )
		return false;

	for( size_t i = 0; i < a.byte_run.size(); ++i )
		if( a.byte_run[i] != b.byte_run[i] )
			return false;

	return true;
}

bool operator==( const InstructionElement& a, const InstructionElement& b )
{
	if( a.address != b.address )
		return false;

	if( a.instruction != b.instruction )
		return false;

	return true;
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


std::ostream& operator<<(std::ostream& os, const IRProgram& ir)
{
	os << "\n{\n";
	os << "    \"origin\":\"" << ir.origin << "\",\n";
	os << "    \"elements\":[\n";

	bool first = true;

	for( const auto& element : ir.elements ) {
		os << "        ";

		std::visit( [&]( auto&& element ) {
			os << element;
		}, element );

		os << ",\n";
	}

	os << "    ]\n";
	os << "}";

	return os;
}

// std::ostream& operator<<(std::ostream& os, const ASMElement& element)
// {
// }

std::ostream& operator<<(std::ostream& os, const DataElement& data_element)
{
	os << "{ \"address\": \""
		<< "0x" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << +data_element.address
	<< "\", ";

	bool first = true;
	for( const char byte : data_element.byte_run ) {
		if( first )
			first = false;
		else
			os << ", ";
		os << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +byte;
	}
	os << "}";

	return os;
}

std::ostream& operator<<(std::ostream& os, const InstructionElement& instr_element)
{
	os << "{ \"address\": \""
		<< "0x" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << +instr_element.address
	<< "\", ";

	os << instr_element.instruction;

	os << "}";

	return os;
}


std::ostream& operator<<(std::ostream& os, const Instruction& instr)
{
    os << instr.opcode() << " Operands: [";

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

std::ostream& operator<<(std::ostream& os, const Opcode& opcode )
{
	static std::array<std::string, opcode_count> mnemonics = {
		"NOP", "CLS", "RET", "JP", "CALL", "SE",
		"SNE", "SE", "LD", "ADD", "LD", "OR", "AND",
		"XOR", "ADD", "SUB", "SHR", "SUBN", "SHL", "SNE", "LD I",
		"JP V0", "RND", "DRW", "SKP", "SKNP", "LD DT",
		"LD ST", "ST K", "ST DT", "ADD I",
		"LD F", "LD B", "ST [I]", "LD [I]"
	};

    os << "Opcode: " << mnemonics[static_cast<int>(opcode)];

	return os;
}

std::ostream& operator<<(std::ostream& os, const Operand& op)
{
    std::visit( [&os](auto&& v)
	{
        using T = std::decay_t<decltype(v)>;

		auto flags = os.flags();
		auto fill  = os.fill();

		if constexpr( std::is_same_v<T, Reg> )
			os << "V" << std::uppercase << std::hex << +v.index;

		else if constexpr( std::is_same_v<T, Addr> )
			os << "0x" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << +v.value;

		else if constexpr( std::is_same_v<T, Imm> )
			os << "#" << std::dec << +v.value;

		else if constexpr( std::is_same_v<T, Nibble> )
			os << "0x" << std::uppercase << std::hex << +v.value;

		else if constexpr( std::is_same_v<T, Key> )
			os << "K" << std::uppercase << std::hex << +v.index;

		else if constexpr( std::is_same_v<T, RegCount> )
			os << "V0-V" << std::uppercase << std::hex << +v.count;

		os.fill(fill);
		os.flags(flags);

    }, op);

    return os;
}
