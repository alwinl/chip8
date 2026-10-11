/*
 * binary_encoder.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

#include <array>
#include <cassert>

#include "ir/encoder.h"
#include "encoder.h"

BinImage encode( const IRProgram &ir )
{
	BinImage image;

	for( const auto& element : ir.elements )
        std::visit( [&]( auto&& v )
		{
			using T = std::decay_t<decltype(v)>;

			if constexpr( std::is_same_v<T, IRInstruction> ) {

				uint16_t word = v.instruction.to_binary();

				image.push_back( (word >> 8) & 0xFF );
				image.push_back( (word     ) & 0xFF );

			}
			else if constexpr( std::is_same_v<T, IRDataBlock> ) {

				image.insert( image.end(), v.bytes.begin(), v.bytes.end() );

			}
			else
				static_assert( std::is_same_v<T, void>, "Unhandled IRElement type" );

		}, element );

	return image;
}
