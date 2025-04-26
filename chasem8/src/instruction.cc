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
 *
 *
 */

#include "instruction.h"

uint16_t Instruction::get_address( const std::string& argument )
{
    uint16_t address;
    try {
        address = std::stoi( argument, nullptr, 0 );

        if( (address < 0x200) || (address > 0xFFF) )
            throw std::runtime_error( "Address out of range" );
    }
    catch( const std::invalid_argument &ex ) {
        address = sym_table.get_address( argument );    // is it a label?
        if( address == (uint16_t)-1 )
            throw std::runtime_error( "Invalid address" );
    }
    catch( const std::out_of_range &ex ) {
        throw std::runtime_error( "Address out of range" );
    }

    return address;
}

DBInstruction::DBInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table )
{
    if( ! arguments.size() )
        throw std::runtime_error( "DB instruction requires values as arguments" );

    for( auto& argument : arguments ) {

        std::stringstream ss(argument);
        std::string value;

        while (std::getline(ss, value, ',')) {

            value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

            if (!value.empty())
                data.push_back( static_cast<uint8_t>(std::stoi( value, nullptr, 0 )) );
        }
    }            
}

void DBInstruction::emit_binary( std::ostream &target )
{
    for( auto& value : data )
        target << value;
}

void CLSInstruction::emit_binary( std::ostream &target )
{
    target << static_cast<uint8_t>(0x00) << static_cast<uint8_t>(0xE0);
}

void RETInstruction::emit_binary( std::ostream &target )
{
    target << static_cast<uint8_t>(0x00) << static_cast<uint8_t>(0xEE);
}

void SYSInstruction::emit_binary( std::ostream &target )
{
    if( arguments.size() != 1 )
        throw std::runtime_error( "SYS instruction requires an address as an argument" );

    uint16_t address = get_address( arguments[0] );
 
    address &= ~0x1000; // Ensure the high nibble  is 0
    target << static_cast<uint8_t>(address >> 8) << static_cast<uint8_t>(address & 0xFF);
}

void JPInstruction::emit_binary( std::ostream &target )
{
    if( arguments.size() != 1 )
        throw std::runtime_error( "JP instruction requires an address as an argument" );

    uint16_t address = get_address( arguments[0] );
 
    address |= 0x1000; // Set the high nibble to 1
    target << static_cast<uint8_t>(address >> 8) << static_cast<uint8_t>(address & 0xFF);
}
