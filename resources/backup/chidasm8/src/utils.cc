/*
 * utils.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <iomanip>
#include <sstream>
#include <string>

 std::string format_register( unsigned int reg )
 {
     std::ostringstream oss;
     oss << "V" << std::uppercase << std::hex << reg;
     return oss.str();
 }

 std::string format_byte( unsigned int byte )
 {
     std::ostringstream oss;
     oss << "0x" << std::setfill( '0' ) << std::setw( 2 ) << std::uppercase << std::hex << byte;
     return oss.str();
 }

 std::string format_naked_byte( unsigned int byte )
 {
     std::ostringstream oss;
     oss << std::setfill( '0' ) << std::setw( 2 ) << std::uppercase << std::hex << byte;
     return oss.str();
 }

 std::string format_address( unsigned int address )
 {
     std::ostringstream oss;
     oss << "0x" << std::setfill( '0' ) << std::setw( 3 ) << std::uppercase << std::hex << address;
     return oss.str();
 }

 std::string format_mnemonic( std::string mnemonic )
 {
     std::ostringstream oss;
     oss << std::setfill( ' ' ) << std::setw( 4 ) << std::left << mnemonic;
     return oss.str();
 }

