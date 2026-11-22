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

bool operator==( const IRInstruction& lhs, const IRInstruction& rhs )
{
	if( lhs.address != rhs.address )
		return false;

	if( lhs.instruction != rhs.instruction )
		return false;

	return true;
}

bool operator==( const IRDataBlock& lhs, const IRDataBlock& rhs )
{
	if( lhs.address != rhs.address )
		return false;

	if( lhs.bytes.size() != rhs.bytes.size() )
		return false;

	for( size_t i = 0; i < lhs.bytes.size(); ++i )
		if( lhs.bytes[i] != rhs.bytes[i] )
			return false;

	return true;
}

bool operator==( const IRElement& lhs, const IRElement& rhs )
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
		}, lhs, rhs
	);
}

bool operator==( const IRProgram& lhs, const IRProgram& rhs )
{
	if( lhs.origin != rhs.origin )
		return false;

	if( lhs.elements.size() != rhs.elements.size() )
		return false;

	for( size_t i = 0; i < lhs.elements.size(); ++i )
		if( lhs.elements[i] != rhs.elements[i] )
			return false;

	return true;
}



std::ostream& operator<<(std::ostream& os, const IRInstruction& instr_element)
{
	os << R"({ "address": ")"
		<< "0x" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << +instr_element.address
	<< R"(", )";

	os << instr_element.instruction;

	os << "}";

	return os;
}

std::ostream& operator<<(std::ostream& os, const IRDataBlock& data_element)
{
	os << R"({ "address": ")"
		<< "0x" << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << +data_element.address
	<< R"(", )";

	bool first = true;
	for( const uint8_t byte : data_element.bytes ) {
		if( first )
			first = false;
		else
			os << ", ";
		os << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << +byte;
	}
	os << "}";

	return os;
}

std::ostream& operator<<(std::ostream& os, const IRElement& element)
{
	os << "        ";
	std::visit( [&]( auto&& element ) { os << element; }, element );
	os << ",\n";

	return os;
}

std::ostream& operator<<(std::ostream& os, const IRProgram& ir)
{
	os << R"(
{
	"origin":")" << ir.origin << R"(",
    "elements":[
)";
	// os << "\n{\n";
	// os << "    \"origin\":\"" << ir.origin << "\",\n";
	// os << "    \"elements\":[\n";

	for( const auto& element : ir.elements )
		os << element;

	os << "    ]\n";
	os << "}";

	return os;
}
