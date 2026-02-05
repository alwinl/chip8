/*
 * disassembler.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "disassembler.h"

#include "decoder.h"

#include <variant>
#include <stack>

IRProgram Disassembler::build_ir( const BinImage &binary )
{
	IRProgram ir {};

	ir.origin = configuration.origin;
	ir.binary = &binary;

	DisasmMemory memory;
	memory.bind( binary, ir.origin );

	collect_instructions( ir, memory, ir.symbols );

	collect_data_bytes( ir, memory, ir.symbols );

    std::sort( ir.elements.begin(), ir.elements.end(),
        [](const ASMElement& a, const ASMElement& b)
		{
            return std::visit(
				[]( auto& lhs, auto& rhs )
					{ return lhs.address < rhs.address; }
				, a, b );
        });

	return ir;
}

void Disassembler::collect_instructions( IRProgram& ir, DisasmMemory& memory, SymbolTable& symbols )
{
	std::stack<uint16_t> address_stack;
	std::unordered_set<uint16_t> decoded_instructions;
	Decoder decoder;

	address_stack.push( configuration.origin );	// Push the start address

	while( !address_stack.empty() ) {

		while( !address_stack.empty() ) {

			uint16_t address = address_stack.top();
			address_stack.pop();

			if( !decoded_instructions.insert(address).second )
				continue; // already decoded

			if( !memory.contains(address) || !memory.contains(address + 1) )
				continue; // implicit termination

			auto result = decoder.decode( address, memory.get_word( address ) );

			if( !result.valid )
				continue;

			memory.mark_instruction( address );

			ir.elements.push_back( InstructionElement { address, result.instruction } );

			for( auto next_address : result.next_addresses )
				address_stack.push( next_address );

			symbols.add( result.target );
		}

		symbols.sort_vectors();

		for( auto table_address : symbols.get_index_list() ) {

			while( memory.contains(table_address) && symbols.get_label(table_address).empty() ) {

				address_stack.push( table_address );
				table_address += 2;
			}
		}
	}
}

void Disassembler::collect_data_bytes( IRProgram& ir, DisasmMemory& memory, SymbolTable& symbols )
{
    std::vector<uint8_t> datarun;
    uint16_t run_start = 0;

    const uint16_t start = memory.start();
    const uint16_t end   = memory.end();

	auto flush = [&]()
	{
		if( !datarun.empty() ) {
			ir.elements.push_back( DataElement { run_start, datarun } );
			datarun.clear();
		}
	};

	// The raw bytes that have not been marked as instruction bytes are thus data bytes
	// Chunk them together into DataElement objects

    for( uint16_t addr = start; addr < end; ++addr ) {

        if( memory.is_data(addr) ) {		// If byte is NOT part of an instruction → it's data

            // If this byte is a jump target → end previous run
            if( ! symbols.get_label( addr ).empty() )
				flush();

            if( datarun.empty() )
                run_start = addr;

            datarun.push_back( memory.get_byte(addr) );

        } else            // Instruction byte interrupts data run
			flush();
    }

	flush();
}
