/*
 * memory.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <cstdint>
#include <vector>

class Memory {
public:
    Memory( uint16_t origin ) : origin( origin ) {}

    void add_byte( uint16_t addr, uint8_t value );
    void mark_instruction( uint16_t addr );

    uint8_t get_byte( uint16_t addr ) const;
	uint16_t get_word( uint16_t address);
    uint16_t mem_start() const { return origin; }
    uint16_t mem_end() const { return origin + bytes.size(); }
    bool is_instruction( uint16_t addr ) const;
    bool contains( uint16_t addr ) const;

private:
    uint16_t origin;
    std::vector<uint8_t> bytes;
    std::vector<uint8_t> instruction_flag;

    void ensure_capacity(uint16_t addr);
};
