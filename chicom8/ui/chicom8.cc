/*
 * chicom8.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "chip8_compiler.h"
#include "commandlineparser.h"

#include <iostream>

int main(int argc, char* argv[])
{
	CommandLineParser args(argc, argv);

    std::string input_file = args.get_input_name();
    std::string output_file = args.get_output_name();

    if (args.is_verbose()) {
        std::cout << "Compiling: " << input_file << " -> " << output_file << std::endl;
    }

    try {
        Chip8Compiler compiler;
        compiler.compile_file(input_file, output_file);
    } catch (const std::exception& ex) {
        std::cerr << "Compilation failed: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
