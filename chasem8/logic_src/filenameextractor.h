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

#ifndef FILENAMEEXTRACTOR_H
#define FILENAMEEXTRACTOR_H

#include <string>
#include <vector>

class FilenameExtractor
{
public:
    FilenameExtractor( std::vector<std::string> arguments );
    FilenameExtractor( int argc, char ** argv );

    std::string get_source_name();
    std::string get_binary_name();
    std::string get_listing_name();

private:
    std::vector<std::string> arguments;
};

#endif // FILENAMEEXTRACTOR_H
