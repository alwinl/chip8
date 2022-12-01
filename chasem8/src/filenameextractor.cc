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

#include "filenameextractor.h"

#include <stdexcept>

FilenameExtractor::FilenameExtractor( std::vector<std::string> arguments )
{
    if( arguments.empty() )
        throw std::invalid_argument( "Need at least a source file name" );

    this->arguments = arguments;
}

FilenameExtractor::FilenameExtractor( int argc, char ** argv )
{
    if( argc < 2 )
        throw std::invalid_argument( "Need at least a source file name" );

    arguments = std::vector<std::string>( argv+1, argv+argc );
}


std::string FilenameExtractor::get_source_name()
{
    return arguments[0];
}

std::string FilenameExtractor::get_binary_name()
{
    if( arguments.size() > 1 )
        return arguments[1];

    /* construct name from source file name */
    std::string name = arguments[0];

    std::size_t fullstop = name.find( '.' );
    if( fullstop != std::string::npos )
        name.erase( fullstop, std::string::npos );

    return name += ".bin";
}

std::string FilenameExtractor::get_listing_name()
{
    if( arguments.size() > 2 )
        return arguments[2];

    /* construct name from source file name */
    std::string name = arguments[0];

    std::size_t fullstop = name.find( '.' );
    if( fullstop != std::string::npos )
        name.erase( fullstop, std::string::npos );

    return name += ".lst";
}


