/*
 * numberparser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "numberparser.h"

NumberParser::NumberParser(std::string input) : value(strip_hash(std::move(input)))
{
}

std::string NumberParser::strip_hash(const std::string& input)
{
    if (!input.empty() && input[0] == '#')
        return input.substr(1);

    return input;
}

int NumberParser::parse_number(const std::string& input, int base)
{
    try {
        return std::stoi(input, nullptr, base);
    }
    catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid number format (" + input + ")");
    }
    catch (const std::out_of_range&) {
        throw std::runtime_error("Number out of range (" + input + ")");
    }
}

uint8_t NumberParser::to_byte()
{
    int byte;

    try {
        byte = parse_number(value, 0);
    }
    catch (const std::runtime_error&) {
        byte = parse_number(value, 16);
    }

    if (byte < 0 || byte > 0xFF)
        throw std::runtime_error("Byte value out of range (0-FF)");

    return static_cast<uint8_t>(byte);
}

uint8_t NumberParser::to_nibble()
{
    int nibble = parse_number(value, 16);

    if (nibble < 0 || nibble > 0x0F)
        throw std::runtime_error("Nibble value out of range (0-F)");

    return static_cast<uint8_t>(nibble);
}

uint16_t NumberParser::to_address(bool syscall)
{
    int address = 0;

    try {
        address = parse_number(value, 0);
    }
    catch (const std::runtime_error&) {
        throw std::runtime_error("Invalid address (" + value + ")");
    }

    if (syscall) {
        if (address >= 0x200)
            throw std::runtime_error("Syscall address out of range (must be < 0x200)");
    } else {
        if (address < 0x200 || address > 0xFFF)
            throw std::runtime_error("Address out of range (0x200 - 0xFFF)");
    }

    return static_cast<uint16_t>(address);
}
