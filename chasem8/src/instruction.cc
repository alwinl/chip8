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
#include "numberparser.h"

#include <numeric>

void push_uint16_t( std::ostream &target, uint16_t value )
{
    target << static_cast<uint8_t>(value >> 8) << static_cast<uint8_t>(value & 0xFF);
}

void push_uint8_t( std::ostream &target, uint8_t value )
{
    target << static_cast<uint8_t>(value);
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

uint16_t Instruction::get_address(const std::string& argument, bool syscall)
{
    try {
        return NumberParser(argument).to_address(syscall);
    }
    catch (const std::runtime_error&) {
        return sym_table.get_address(argument);
    }
}

uint8_t Instruction::get_register(const std::string& argument)
{
    if( argument[0] != 'V' && argument[0] != 'v')
        throw std::runtime_error("Invalid register name (" + argument + ")");

    return NumberParser(argument.substr(1)).to_nibble();
}

uint8_t Instruction::get_nibble(const std::string& argument)
{
    try {
        return NumberParser(argument).to_nibble();
    }
    catch (const std::runtime_error&) {
        return sym_table.get_nibble(argument);
    }
}

uint8_t Instruction::get_byte(const std::string& argument)
{
    try{
        return NumberParser(argument).to_byte();
    }
    catch (const std::runtime_error&) {
        return sym_table.get_byte(argument);
    }
}

uint16_t Instruction::get_word( const std::string &argument )
{
    try{
        return NumberParser(argument).to_word();
    }
    catch (const std::runtime_error&) {
        return sym_table.get_word(argument);
    }
}

DBInstruction::DBInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table )
{
    if( parameters.empty() )
        throw std::runtime_error( "DB instruction requires values as arguments" );

    for( auto& value : parameters)
        data.push_back( get_byte(value) );
}

void DBInstruction::emit_binary( std::ostream &target )
{
    for( auto& value : data )
        target << value;
}

DWInstruction::DWInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table )
{
    if( parameters.empty() )
        throw std::runtime_error( "DW instruction requires values as arguments" );

    for( auto& value : parameters) {

        uint16_t word = get_word(value);

        data.push_back( static_cast<uint8_t>(word >> 8) );
        data.push_back( static_cast<uint8_t>(word & 0xFF) );
    }
}

void DWInstruction::emit_binary( std::ostream &target )
{
    for( auto& value : data )
        target << value;
}

void CLSInstruction::emit_binary( std::ostream &target )
{
    if( ! parameters.empty() )
        throw std::runtime_error( "CLS instruction requires no arguments" );

    // CLS = 00E0
    push_uint16_t( target, 0x00E0 );
}

void RETInstruction::emit_binary( std::ostream &target )
{
    if( ! parameters.empty() )
        throw std::runtime_error( "RET instruction requires no arguments" );

    // RET = 00EE
    push_uint16_t( target, 0x00EE );
}

void SYSInstruction::emit_binary( std::ostream &target )
{
    // SYS addr = 0nnn
    if( parameters.size() != 1 )
        throw std::runtime_error( "SYS instruction requires an address as an argument" );

    push_uint16_t( target, get_address( parameters[0], true ) );
}

void JPInstruction::emit_binary( std::ostream &target )
{
    if( parameters.empty() )
        throw std::runtime_error( "JP instruction requires an address as an argument" );

    // 1nnn - JP addr
    if( std::toupper(parameters[0][0]) != 'V') {
        push_uint16_t( target, (0x1000 | get_address( parameters[0] )) );
        return;
    }

    // Bnnn - JP V0,addr
    if( parameters.size() != 2 || parameters[0][1] != '0')
        throw std::runtime_error( "JP instruction requires the register to be V0" );

    push_uint16_t( target, (0xB000 | get_address( parameters[1] )) );
}

void CALLInstruction::emit_binary( std::ostream &target )
{
    // 2nnn - CALL addr
    if( parameters.empty() )
        throw std::runtime_error( "CALL instruction requires an address as an argument" );

    push_uint16_t( target, (0x2000 | get_address( parameters[0] ) ) );
}

void SEInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SE instruction requires two registers or a register and a byte as arguments" );

    if( parameters[0][0] != 'V' )
        throw std::runtime_error( "SE instruction requires a register as the first argument" );

    uint8_t x_reg = get_register( parameters[0] );

    if( parameters[1][0] == 'V' ) {

        // 5xy0 - SE Vx, Vy
        uint8_t y_reg = get_register( parameters[1] );
        push_uint16_t( target, (0x5000 | (x_reg << 8) | (y_reg << 4)) );

    } else {

        // 3xkk - SE Vx, byte
        uint8_t byte = get_byte(parameters[1]);
        push_uint16_t( target, (0x3000 | (x_reg << 8) | byte) );
    }
}

void SNEInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SNE instruction requires two registers or a register and a byte as arguments" );

    if( parameters[0][0] != 'V' )
        throw std::runtime_error( "SNE instruction requires a register as the first argument" );

    uint16_t x_reg = get_register( parameters[0] );

    if( parameters[1][0] == 'V' ) {
        // 9xy0 - SNE Vx, Vy
        uint8_t y_reg = get_register( parameters[1] );
        push_uint16_t( target, (0x9000 | (x_reg << 8) | (y_reg << 4)) );
    } else {
        // 4xkk - SNE Vx, byte
        uint8_t byte = get_byte(parameters[1]);
        push_uint16_t( target, (0x4000 | (x_reg << 8) | byte) );
    }
}

void LDInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "LD requires two operands" );

    if( parameters[0][0] == 'V' ) {

        uint8_t x_reg = get_register(parameters[0]);

        // 8xy2 - LD Vx, Vy
        if( parameters[1][0] == 'V' ) {
            uint8_t y_reg = get_register( parameters[1] );
            push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4)) );
            return;
        }

        // Fx07 - LD Vx, DT
        if( parameters[1] == "DT") {
            push_uint16_t( target, (0xF007 | (x_reg << 8)) );
            return;
        }

        // Fx0A - LD Vx, K
        if( parameters[1] == "K") {
            push_uint16_t( target, (0xF00A | (x_reg << 8)) );
            return;
        }

        // Fx65 - LD Vx, [I]
        if( parameters[1] == "[I]") {
            push_uint16_t( target, (0xF065 | (x_reg << 8)) );
            return;
        }

        // 6xkk - LD Vx, byte
        uint8_t byte = get_byte(parameters[1]);
        push_uint16_t( target, (0x6000 | (x_reg << 8) | byte) );
        return;
    }

    if( parameters[0] == "I" ) {
        // LD I, Addr
        uint16_t address = get_address( parameters[1] );
        push_uint16_t( target, (0xA000 | address) );
        return;
    }

    if( parameters[0] == "DT" ) {
        // LD DT, Vx
        if( parameters[1][0] != 'V' )
            throw std::runtime_error( "LD instruction requires a register as the second argument" );

        uint8_t x_reg = get_register( parameters[1] );
        push_uint16_t( target, (0xF015 | (x_reg << 8)) );
        return;
    }

    if( parameters[0] == "ST" ) {
        // LD ST, Vx
        if( parameters[1][0] != 'V' )
            throw std::runtime_error( "LD instruction requires a register as the second argument" );

        uint8_t x_reg = get_register( parameters[1] );
        push_uint16_t( target, (0xF018 | (x_reg << 8)) );
        return;
    }

    if( parameters[0] == "F" ) {
        // LD F, Vx
        if( parameters[1][0] != 'V' )
            throw std::runtime_error( "LD instruction requires a register as the second argument" );

        uint8_t x_reg = get_register( parameters[1] );
        push_uint16_t( target, (0xF029 | (x_reg << 8)) );
        return;
    }

    if( parameters[0] == "B" ) {
        // LD B, Vx
        if( parameters[1][0] != 'V' )
            throw std::runtime_error( "LD B instruction requires a register as the second argument" );

        uint8_t x_reg = get_register( parameters[1] );
        push_uint16_t( target, (0xF033 | (x_reg << 8)) );
        return;
    }

    if( parameters[0] == "[I]" ) {
        // LD [I], Vx
        if( parameters[1][0] != 'V' )
            throw std::runtime_error( "LD [I] instruction requires a register as the second argument" );

        uint8_t x_reg = get_register( parameters[1] );
        push_uint16_t( target, (0xF055 | (x_reg << 8)) );
        return;
    }

    throw std::runtime_error( "Unknown LD instruction" );
}

void ADDInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "ADD requires two operands" );

    // Fx1E - ADD I, Vx
    if(std::toupper(parameters[0][0]) == 'I') {
        uint8_t x_reg = get_register( parameters[1] );
        push_uint16_t( target, (0xF01E | (x_reg << 8)) );
        return;
    }

    if( std::toupper(parameters[0][0]) != 'V' )
        throw std::runtime_error( "ADD instruction requires a register or 'I' as the first argument" );

    uint8_t x_reg = get_register( parameters[0] );

    if( parameters[1][0] == 'V' ) {
        // 8xy4 - ADD Vx, Vy
        uint8_t y_reg = get_register( parameters[1] );
        push_uint16_t( target, (0x8004 | (x_reg << 8) | (y_reg << 4)) );
    } else {
        // 7xkk - ADD Vx, byte
        uint8_t byte = get_byte(parameters[1]);
        push_uint16_t( target, (0x7000 | (x_reg << 8) | byte) );
    }
}

void ORInstruction::emit_binary( std::ostream &target )
{
    // auto [vx, vy] = parse_register_pair( parameters, "OR" );

    if( parameters.size() != 2 )
        throw std::runtime_error( "OR requires two register arguments" );

    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    // 8xy1 - OR Vx, Vy
    // uint16_t code = (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0001);
    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0001) );
}

void ANDInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "AND requires two register arguments" );

    // 8xy2 - AND Vx, Vy
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0002) );
}

void XORInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "XOR requires two register arguments" );

    // 8xy3 - XOR Vx, Vy
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0003) );
}

void SUBInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SUB requires two register arguments" );

    // 8xy5 - SUB Vx, Vy
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0005) );
}

void SHRInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SHR requires two register arguments" );

    // 8xy6 - SHR Vx {, Vy}
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0006) );
}

void SUBNInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SUBN requires two register arguments" );

    // 8xy7 - SUBN Vx, Vy
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x0007) );
}

void SHLInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "SHL requires two register arguments" );

    // 8xyE - SHL Vx {, Vy}
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );

    push_uint16_t( target, (0x8000 | (x_reg << 8) | (y_reg << 4) | 0x000E) );
}

void RNDInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 2 )
        throw std::runtime_error( "RND requires a register and a byte argument" );

    // Cxkk - RND Vx, byte
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t mask = get_byte(parameters[1]);

    push_uint16_t( target, (0xC000 | (x_reg << 8) | mask) );
}

void DRWInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 3 )
        throw std::runtime_error( "DRW requires two register and a nibble argument" );

    // Dxyn - DRW Vx, Vy, nibble
    uint8_t x_reg = get_register( parameters[0] );
    uint8_t y_reg = get_register( parameters[1] );
    uint8_t lines = get_nibble(parameters[2]);

    push_uint16_t( target, (0xD000 | (x_reg << 8) | (y_reg << 4) | lines) );
}

void SKPInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 1 )
        throw std::runtime_error( "SKP requires a register argument" );

    // Ex9E - SKP Vx
    uint8_t x_reg = get_register( parameters[0] );

    push_uint16_t( target, (0xE09E | (x_reg << 8)) );
}

void SKPNInstruction::emit_binary( std::ostream &target )
{
    if( parameters.size() != 1 )
        throw std::runtime_error( "SKNP requires a register argument" );

    // ExA1 - SKNP Vx
    uint8_t x_reg = get_register( parameters[0] );

    push_uint16_t( target, (0xE0A1 | (x_reg << 8)) );
}
