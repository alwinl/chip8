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

#include "numberparser.h"

#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>

#include <iostream>

std::unordered_map<std::string, std::function<std::unique_ptr<Instruction>(const std::vector<std::string>&, const SymbolTable&)>> keyword_dispatcher =
{
	{ "CLS",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<CLSInstruction >(args, sym_table); } },
	{ "RET",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<RETInstruction >(args, sym_table); } },
	{ "SYS",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SYSInstruction >(args, sym_table); } },
	{ "JP",   [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<JPInstruction  >(args, sym_table); } },
	{ "CALL", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<CALLInstruction>(args, sym_table); } },
	{ "SE",   [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SEInstruction  >(args, sym_table); } },
	{ "SNE",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SNEInstruction >(args, sym_table); } },
	{ "LD",   [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<LDInstruction  >(args, sym_table); } },
	{ "ADD",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<ADDInstruction >(args, sym_table); } },
	{ "OR",   [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<ORInstruction  >(args, sym_table); } },
	{ "AND",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<ANDInstruction >(args, sym_table); } },
	{ "XOR",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<XORInstruction >(args, sym_table); } },
	{ "SUB",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SUBInstruction >(args, sym_table); } },
	{ "SHR",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SHRInstruction >(args, sym_table); } },
	{ "SUBN", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SUBNInstruction>(args, sym_table); } },
	{ "SHL",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SHLInstruction >(args, sym_table); } },
	{ "RND",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<RNDInstruction >(args, sym_table); } },
	{ "DRW",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<DRWInstruction >(args, sym_table); } },
	{ "SKP",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SKPInstruction >(args, sym_table); } },
	{ "SKPN", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<SKPNInstruction>(args, sym_table); } },

	{ ".DB",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<DBInstruction  >(args, sym_table); } },
	{ ".DW",  [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<DWInstruction  >(args, sym_table); } },
	{ ".ORG", [](const std::vector<std::string>& args, const SymbolTable& sym_table) { return std::make_unique<DWInstruction  >(args, sym_table); } },
};

void Assembler::read_source( std::istream &source )
{
	uint16_t current_address = 0x200;
	int line_number = 0;
	std::string line;
	std::vector<std::string> tokens;

	while( std::getline( source, line ) ) {

		line_number++;

		if(
			line.empty() ||
			remove_slash_r( line ) ||
			remove_comments( line ) ||
			parse_tokens( line, tokens ) ||
			extract_label( tokens, current_address )
		)
			continue;

		to_upper( tokens[0] );

		if( (tokens.size() >= 3) && is_variable_indicator( tokens[1] ) )
			extract_variable( tokens );
		else
			current_address += extract_instruction( tokens );

		if( current_address > 0xFFF ) {
			std::cerr << "Program counter overflow at line " << line_number << "\n";
			return;
		}
	}
}

bool Assembler::remove_slash_r( std::string &input )
{
    if( input.back() == '\r' )
        input.pop_back();

	return input.empty();
}

bool Assembler::remove_comments( std::string &input )
{
    const size_t comment_start = input.find_first_of(';');

    if( comment_start != std::string::npos )
        input = input.substr(0, comment_start);

	return input.empty();
}

bool Assembler::parse_tokens( std::string &input, std::vector<std::string> &tokens )
{
    std::stringstream stream(input);
    std::string token;

	tokens.clear();

    while (stream >> token)
        tokens.push_back(token);

	return tokens.empty();
}

bool Assembler::extract_label( std::vector<std::string> &tokens, uint16_t current_address )
{
	if( !is_label(tokens[0]) )
		return false;

	symbol_table.add_label( tokens[0], current_address );
	tokens.erase( tokens.begin() );

	return tokens.empty();
}

void Assembler::to_upper( std::string& token )
{
	std::transform( token.begin(), token.end(), token.begin(), [](unsigned char c) { return std::toupper(c); } );
}

void Assembler::extract_variable( std::vector<std::string> &tokens )
{
	uint16_t value = NumberParser(tokens[2]).to_word();

	symbol_table.add_variable(tokens[0], value);
}

uint16_t Assembler::extract_instruction( std::vector<std::string> &tokens )
{
	auto entry = keyword_dispatcher.find( tokens[0] );

	if( entry == keyword_dispatcher.end() ) {
		std::cerr << "Unknown mnemonic: " << tokens[0] << "\n";
		return 0;
	}

	auto [_,dispatcher] = *entry;
	auto arguments = std::vector<std::string>(tokens.begin() + 1, tokens.end());

	instructions.push_back( dispatcher( arguments, symbol_table ) );

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
