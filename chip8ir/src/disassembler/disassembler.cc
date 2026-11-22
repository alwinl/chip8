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

#include "disassembler/disassembler.h"

#include <variant>
#include <stack>
#include <unordered_set>
#include <istream>

#include "ir/chip8ir.h"
#include "ir/chip8formats.h"
#include "ir/decoder.h"

#include "disassembler/memory.h"
#include "disassembler/symbol_table.h"

namespace {

	void collect_instructions( std::vector<IRElement>& elements, DisasmMemory& memory, DisasmSymbolTable& symbols )
	{
		std::stack<uint16_t> address_stack;
		std::unordered_set<uint16_t> decoded_instructions;
		Decoder decoder;

		address_stack.push( memory.start() );	// Push the start address

		while( !address_stack.empty() ) {

			while( !address_stack.empty() ) {

				const uint16_t address = address_stack.top();
				address_stack.pop();

				if( !decoded_instructions.insert(address).second )
					continue; // already decoded

				if( !memory.contains(address) || !memory.contains(address + 1) )
					continue; // implicit termination

				auto result = decoder.decode( address, memory.get_word( address ) );

				if( !result.valid )
					continue;

				elements.emplace_back( IRInstruction { address, result.instruction } );

				for( auto next_address : result.next_addresses )
					address_stack.push( next_address );

				switch( result.role ) {
				case AddressRole::None: break;
				case AddressRole::JumpTarget:       symbols.add( { result.referenced_address, eSymbolKind::JUMP       } ); break;
				case AddressRole::SubroutineTarget: symbols.add( { result.referenced_address, eSymbolKind::SUBROUTINE } ); break;
				case AddressRole::ILoadTarget:      symbols.add( { result.referenced_address, eSymbolKind::I_TARGET   } ); break;
				case AddressRole::IndexedBase:      symbols.add( { result.referenced_address, eSymbolKind::INDEXED    } ); break;
				}

				memory.mark_instruction( address );
			}

			for( auto table_address : symbols.get_index_list() ) {

				while( memory.contains(table_address) && symbols.get_label(table_address).empty() ) {

					address_stack.push( table_address );
					table_address += 2;
				}
			}
		}

		symbols.sort_vectors();
	}

	void collect_data_bytes( std::vector<IRElement>& elements, DisasmMemory& memory, const DisasmSymbolTable& symbols )
	{
		std::vector<uint8_t> datarun;
		uint16_t run_start = 0;

		const uint16_t start = memory.start();
		const uint16_t end   = memory.end();

		auto flush = [&]()
		{
			if( !datarun.empty() ) {
				elements.emplace_back( IRDataBlock { run_start, datarun } );
				datarun.clear();
			}
		};

		// The raw bytes that have not been marked as instruction bytes are thus data bytes
		// Chunk them together into IRDataBlock objects

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

	void sort_elements( std::vector<IRElement>& elements )
	{
		std::sort( elements.begin(), elements.end(),
			[]( const IRElement& lhs, const IRElement& rhs )
			{
				return std::visit(
					[]( auto& lhs, auto& rhs )
						{ return lhs.address < rhs.address; }
					, lhs, rhs );
			});
	}

} // namespace

namespace disassembler
{
	IRBundle build_ir( std::istream& is, std::string program_name, uint16_t start_address )
	{
		DisasmMemory memory{ is, start_address };
		DisasmSymbolTable symbols;
		IRProgram ir_rep{ start_address, {} };

		collect_instructions( ir_rep.elements, memory, symbols );
		collect_data_bytes( ir_rep.elements, memory, symbols );
		sort_elements( ir_rep.elements );

		return { program_name, std::move(ir_rep), std::make_unique<DisasmSymbolTable>(symbols) };
	}

} // namespace disassembler