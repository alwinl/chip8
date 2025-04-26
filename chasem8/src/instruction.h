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
 */

#pragma once

#include "symboltable.h"

#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <algorithm>

class Instruction
{
public:
    Instruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : arguments( std::move( arguments ) ), sym_table(sym_table) {};
    virtual ~Instruction() {};

    virtual void emit_binary( std::ostream &target ) = 0;
    virtual void emit_listing( std::ostream &target ) {};
    virtual size_t length() const = 0;

protected:
    std::vector<std::string> arguments;
    const SymbolTable& sym_table;

    uint16_t get_address( const std::string& argument );
};

class DBInstruction : public Instruction
{
public:
    DBInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table );

    void emit_binary( std::ostream &target ) override;
    size_t length() const override { return data.size(); }

private:
    std::vector<uint8_t> data;
};

class CLSInstruction : public Instruction
{
public:
    CLSInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table ) {};

    void emit_binary( std::ostream &target ) override;
    size_t length() const override { return 2; }
};

class RETInstruction : public Instruction
{
public:
    RETInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table ) {};

    void emit_binary( std::ostream &target ) override;
    size_t length() const override { return 2; }
};

class SYSInstruction : public Instruction
{
public:
    SYSInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table ) {};

    void emit_binary( std::ostream &target ) override;
    size_t length() const override { return 2; }
};

class JPInstruction : public Instruction
{
public:
    JPInstruction( const std::vector<std::string>& arguments, const SymbolTable& sym_table ) : Instruction( arguments, sym_table ) {};

    void emit_binary( std::ostream &target ) override;
    size_t length() const override { return 2; }
};

