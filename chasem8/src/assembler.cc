/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
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

#include "assembler.h"

#include "linepreprocessor.h"

#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>

#include <iostream>

std::unordered_map<std::string, std::function<std::unique_ptr<Instruction>(const std::vector<std::string>&, const SymbolTable&)>> keyword_dispatcher =
{
	{ ".DB", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<DBInstruction >(args, sym_table); } },
	{ "CLS", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<CLSInstruction>(args, sym_table); } },
	{ "RET", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<RETInstruction>(args, sym_table); } },
	{ "SYS", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SYSInstruction>(args, sym_table); } },
	{ "JP",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<JPInstruction >(args, sym_table); } }
};

void Assembler::read_source( std::istream &source )
{
	uint16_t current_address = 0x200;
	int line_number = 0;
	std::string line;

	while( std::getline( source, line ) ) {

		line_number++;

		std::vector<std::string> tokens = LinePreprocessor::tokenise( line );

		extract_label( tokens, current_address );

		current_address += extract_instruction( tokens );

		if( current_address > 0xFFF ) {
			std::cerr << "Program counter overflow at line " << line_number << "\n";
			return;
		}
	}
}

void Assembler::extract_label( std::vector<std::string> &tokens, uint16_t current_address )
{
	if( tokens.empty() )
		return;

	if( tokens[0].back() == ':' ) {
		symbol_table.add_label( tokens[0], current_address );
		tokens.erase( tokens.begin() );
	}
}

uint16_t Assembler::extract_instruction( std::vector<std::string> &tokens )
{
	if( tokens.empty() )
		return 0;

	std::transform( tokens[0].begin(), tokens[0].end(), tokens[0].begin(), [](unsigned char c) { return std::toupper(c); } );

	auto entry = keyword_dispatcher.find( tokens[0] );

	if( entry == keyword_dispatcher.end() ) {
		std::cerr << "Unknown mnemonic: " << tokens[0] << "\n";
		return 0;
	}

	instructions.push_back( entry->second( std::vector<std::string>(tokens.begin() + 1, tokens.end()), symbol_table ) );

	return instructions.back()->length();
}

void Assembler::write_binary( std::ostream &target )
{
	std::for_each( instructions.begin(), instructions.end(), 
		[&target]( const std::unique_ptr<Instruction>& instruction )
		{
			instruction->emit_binary( target );
		}
	);
}

void Assembler::write_listing( std::ostream &target )
{
	std::for_each( instructions.begin(), instructions.end(), 
		[&target]( const std::unique_ptr<Instruction>& instruction )
		{
			instruction->emit_listing( target );
		}
	);
}