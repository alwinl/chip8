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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <vector>
#include <istream>
#include <ostream>

#include "instruction.h"

class Program
{
public:
    Program();

    void read_source( std::istream& is );

    void write_binary( std::ostream& os );
    void write_listing( std::ostream& os );

private:
    void add_instruction( Instruction& inst ) { instructions.push_back( inst ); }

    std::vector<Instruction> instructions;
};

#endif // PROGRAM_H
