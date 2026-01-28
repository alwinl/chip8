/*
 * targets.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include "targets.h"

void Targets::add(std::optional<DecodedTarget> target)
{
	if( target )
		target_lists[target->kind].push_back( target->address );
}


void Targets::sort_vectors()
{
	for( auto& target_list : target_lists ) {
		std::sort( target_list.begin(), target_list.end() );
		target_list.erase( std::unique( target_list.begin(), target_list.end()), target_list.end() );
	}
	sorted = true;
}

std::string Targets::get_label( uint16_t address ) const
{
	assert( sorted );

	static constexpr std::array<const char *,eKind::COUNT> prefixes{ "DATA", "FUNC", "LABEL", "TABLE" };

	for( size_t kind = 0; kind < eKind::COUNT; ++ kind ) {

		auto it = std::lower_bound( target_lists[kind].begin(), target_lists[kind].end(), address );
		if( (it != target_lists[kind].end()) && (*it == address) )
			return std::string(prefixes[kind]) + std::to_string( std::distance( target_lists[kind].begin(), it) );
	}

	return {};
}
