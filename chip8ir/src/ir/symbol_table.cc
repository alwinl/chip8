/*
 * symbol_table.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "ir/symbol_table.h"

#include <algorithm>
#include <cassert>
#include <memory>

void SymbolTable::add( std::optional<DecodedSymbol> symbol )
{
	if( symbol )
		symbol_lists[symbol->kind].push_back( symbol->address );
}

void SymbolTable::sort_vectors()
{
	for( auto& symbol_list : symbol_lists ) {
		std::sort( symbol_list.begin(), symbol_list.end() );
		symbol_list.erase( std::unique( symbol_list.begin(), symbol_list.end()), symbol_list.end() );
	}
	sorted = true;
}

std::string SymbolTable::get_label( uint16_t address ) const
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
