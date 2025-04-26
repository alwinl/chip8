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

#include <numeric>

void push_uint16_t( std::ostream &target, uint16_t value )
{
    target << static_cast<uint8_t>(value >> 8) << static_cast<uint8_t>(value & 0xFF);
}

void push_uint8_t( std::ostream &target, uint8_t value )
{
    target << static_cast<uint8_t>(value);
}

std::pair<uint16_t, uint16_t> parse_register_pair(const std::vector<std::string>& parameters, const std::string& mnemonic)
{
    if( parameters.empty() )
        throw std::runtime_error(mnemonic + " requires two register arguments");

    if( parameters[0][0] != 'V' || parameters[1][0] != 'V' )
        throw std::runtime_error(mnemonic + ": Both arguments must be registers (e.g., V1)");

    uint16_t x_reg = static_cast<uint16_t>(std::stoi(&parameters[0][0], nullptr, 16));
    uint16_t y_reg = static_cast<uint16_t>(std::stoi(&parameters[1][0], nullptr, 16));

    if (x_reg > 0x0F || y_reg > 0x0F)
        throw std::runtime_error(mnemonic + ": Register index out of range (0-F)");

    return {x_reg, y_reg};
}

Instruction::Instruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : sym_table(sym_table)
{
    std::string joined = std::accumulate( arguments.begin(), arguments.end(), std::string() );

    joined.erase(std::remove_if(joined.begin(), joined.end(), ::isspace), joined.end());

    std::stringstream combined(joined);
    std::string parameter;
    
    while( std::getline( combined, parameter, ',' ) ) {

        if( !parameter.empty() )
            parameters.push_back( parameter );
    }
}

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
    if( parameters.empty() )
        throw std::runtime_error( "DB instruction requires values as arguments" );

    for( auto& value : parameters) {

        int parsed = std::stoi( value, nullptr, 0 );

        if( parsed < 0 || parsed > 0xFF )
            throw std::runtime_error("DB value out of 8-bit range: " + value);        

        data.push_back( static_cast<uint8_t>(parsed) );
    }
}

void DBInstruction::emit_binary( std::ostream &target )
{
    for( auto& value : data )
        target << value;
}

void CLSInstruction::emit_binary( std::ostream &target )
{
    // CLS = 00E0
    push_uint16_t( target, 0x00E0 );
}

void RETInstruction::emit_binary( std::ostream &target )
{
    // RET = 00EE
    push_uint16_t( target, 0x00EE );
}

void SYSInstruction::emit_binary( std::ostream &target )
{
    // SYS addr = 0nnn
    if( parameters.size() != 1 )
        throw std::runtime_error( "SYS instruction requires an address as an argument" );

    push_uint16_t( target, get_address( parameters[0] ) );
}

void JPInstruction::emit_binary( std::ostream &target )
{
    if( parameters.empty() )
        throw std::runtime_error( "JP instruction requires an address as an argument" );

    // 1nnn - JP addr
    // Bnnn - JP V0,addr
    uint16_t address = ( std::toupper(parameters[0][0]) == 'V') ? (0xB000 | get_address( parameters[1] )) : (0x1000 | get_address( parameters[0] ));

    push_uint16_t( target, address );
}

void CALLInstruction::emit_binary( std::ostream &target )
{
    // 2nnn - CALL addr
    if( parameters.empty() )
        throw std::runtime_error( "CALL instruction requires an address as an argument" );

    uint16_t address = (0x2000 | get_address( parameters[0] ) );
 
    push_uint16_t( target, address );
}

void SEInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SE instruction requires two registers or a register and a byte as arguments" );

    if( parameters[0][0] != 'V' )
        throw std::runtime_error( "SE instruction requires a register as the first argument" );

    uint16_t x_reg = std::stoi( &parameters[0][1], nullptr, 16 );

    if( parameters[1][0] == 'V' ) {
        // 5xy0 - SE Vx, Vy
        uint16_t y_reg = std::stoi( &parameters[1][1], nullptr, 16);
        uint16_t code = 0x5000 | (x_reg << 8) | (y_reg << 4);

        push_uint16_t(target, code);   
    } else {
        // 3xkk - SE Vx, byte
        uint16_t byte = std::stoi( parameters[1], nullptr, 0 );    
        uint16_t code = 0x3000 | (x_reg << 8) | byte;

        push_uint16_t( target, code );   
    }
}

void SNEInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SNE instruction requires two registers or a register and a byte as arguments" );

    if( parameters[0][0] != 'V' )
        throw std::runtime_error( "SNE instruction requires a register as the first argument" );

    uint16_t x_reg = std::stoi( &parameters[0][1], nullptr, 16 );

    if( parameters[1][0] == 'V' ) {
        // 9xy0 - SNE Vx, Vy
        uint16_t y_reg = std::stoi( &parameters[1][1], nullptr, 16);
        uint16_t code = 0x9000 | (x_reg << 8) | (y_reg << 4);

        push_uint16_t(target, code);   
    } else {
        // 4xkk - SNE Vx, byte
        uint16_t byte = std::stoi( parameters[1], nullptr, 0 );    
        uint16_t code = 0x4000 | (x_reg << 8) | byte;

        push_uint16_t( target, code );   
    }
}

void LDInstruction::emit_binary( std::ostream &target )
{
// 6xkk - LD Vx, byte
// 8xy0 - LD Vx, Vy
// Annn - LD I, addr
// Fx07 - LD Vx, DT
// Fx0A - LD Vx, K
// Fx15 - LD DT, Vx
// Fx18 - LD ST, Vx
// Fx29 - LD F, Vx
// Fx33 - LD B, Vx
// Fx55 - LD [I], Vx
// Fx65 - LD Vx, [I]

}

void ADDInstruction::emit_binary( std::ostream &target )
{
// 7xkk - ADD Vx, byte
// 8xy4 - ADD Vx, Vy
// Fx1E - ADD I, Vx

}

void ORInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "OR" );

    // 8xy1 - OR Vx, Vy
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x0001;
    push_uint16_t( target, code );
}

void ANDInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "AND" );

    // 8xy2 - AND Vx, Vy
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x0002;
    push_uint16_t( target, code );
}

void XORInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "XOR" );

    // 8xy3 - XOR Vx, Vy
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x0003;
    push_uint16_t( target, code );
}

void SUBInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "SUB" );

    // 8xy5 - SUB Vx, Vy
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x0005;
    push_uint16_t( target, code );
}

void SHRInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "SHR" );

    // 8xy6 - SHR Vx {, Vy}
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x0006;
    push_uint16_t( target, code );
}

void SUBNInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "SUBN" );

    // 8xy7 - SUBN Vx, Vy
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x0007;
    push_uint16_t( target, code );
}

void SHLInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "SHL" );

    // 8xyE - SHL Vx {, Vy}
    uint16_t code = 0x8000 | (vx << 8) | (vy << 4) | 0x000E;
    push_uint16_t( target, code );
}

void RNDInstruction::emit_binary( std::ostream &target )
{
// Cxkk - RND Vx, byte
    // if( parameter.empty() )
    //     throw std::runtime_error( "RND instruction requires a byte as an argument" );

    // uint8_t value = static_cast<uint8_t>(std::stoi( parameter, nullptr, 0 ));

    // target << static_cast<uint8_t>(0xC0 | (value & 0x0F));
    // target << value;
}

void DRWInstruction::emit_binary( std::ostream &target )
{
    auto [vx, vy] = parse_register_pair( parameters, "DRW" );

    // Dxyn - DRW Vx, Vy, nibble

}

void SKPInstruction::emit_binary( std::ostream &target )
{
// Ex9E - SKP Vx

}

void SKNPInstruction::emit_binary( std::ostream &target )
{
// ExA1 - SKNP Vx

}
