/*
 * generator.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "generator.h"

void Generator::setup()
{
    constexpr uint16_t CHIP8_START = 0x200;
    
    binary.reserve(4096 - CHIP8_START);    
}

void Generator::emit_startup()
{
    uint16_t mainAddress = 0x202; //symbolTable["main"].address;

    // Encode CALL main (2-byte instruction)
    uint16_t callInstr = 0x2000 | (mainAddress & 0x0FFF);
    binary.push_back((callInstr >> 8) & 0xFF);
    binary.push_back(callInstr & 0xFF);
}

void Generator::emitFunction( FuncDecl *func ) {}

void Generator::evaluate()
{
    for( auto decl : program.declarations ) {
        if( auto func = dynamic_cast<FuncDecl*>(decl) )
            emitFunction( func );
    }
}

void Generator::write_to_file( std::string file_name )
{
    std::ofstream out( file_name, std::ios::binary );

    out.write( reinterpret_cast<const char*>(binary.data()), binary.size() );    
}

