/*
 * memory.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "memory.h"

#include <cassert>

void Memory::add_byte(uint16_t addr, uint8_t value)
{
    ensure_capacity(addr);
    bytes[addr - origin] = value;
}

uint8_t Memory::get_byte(uint16_t addr) const
{
	assert( contains( addr ) );

    return bytes[addr - origin];
}

void Memory::mark_instruction(uint16_t addr)
{
    if( contains( addr ) ) {
        instruction_flag[addr - origin] = true;
        instruction_flag[addr - origin + 1] = true;
	}
}

bool Memory::is_instruction(uint16_t addr) const
{
    return contains( addr ) ? instruction_flag[addr - origin] : false;
}

uint16_t Memory::get_word( uint16_t address )
{
	return ( get_byte(address) << 8 ) | get_byte( address + 1 );
}

void Memory::ensure_capacity(uint16_t addr)
{
    size_t index = addr - origin;

    if( index >= bytes.size() ) {
        bytes.resize( index + 1 );
        instruction_flag.resize( index + 1 );
    }
}

bool Memory::contains( uint16_t addr ) const
{
    return( (addr >= origin) && ( addr - origin < bytes.size() ) );
}
