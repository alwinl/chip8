/*
 * cmdline_processor.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <string>

#include "../vendor/cxxopts/cxxopts.hpp"

#include "quirks.h"

class CmdlineProcessor
{
public:
	CmdlineProcessor( int argc, char ** argv );

	Quirks::eChipType get_chip_type() { return chip_type; };
	std::string get_program() { return program; };

private:
	Quirks::eChipType chip_type = Quirks::eChipType::CHIP8;
	std::string program;

	cxxopts::Options build_options();
	bool validate( cxxopts::Options options, cxxopts::ParseResult result );
	Quirks::eChipType decode_chip_type( cxxopts::ParseResult result );
};

