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

#include "../vendor/cxxopts/cxxopts.hpp"

#include <string>
#include <vector>

class CommandLineParser
{
public:
    CommandLineParser( std::vector<std::string> arguments );
    CommandLineParser( int argc, char ** argv );

    std::string get_source_name();
    std::string get_program_name();
	std::string get_output_name();
    bool is_verbose() const;
    bool show_help() const;

private:
    cxxopts::Options options;
    cxxopts::ParseResult result;

	const std::string get_output_prefix();
};
