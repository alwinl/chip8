/*
 * linepreprocessor.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "linepreprocessor.h"
#include <sstream>

std::vector<std::string> LinePreprocessor::tokenise( std::string input )
{
    std::vector<std::string> tokens;

    remove_slash_r(input);
    remove_comments(input);

	parse_tokens( input, tokens );

    return tokens;
}

void LinePreprocessor::parse_tokens( std::string &input, std::vector<std::string> &tokens )
{
    if (input.empty())
        return;

    std::stringstream stream(input);
    std::string token;

    while (stream >> token)
        tokens.push_back(token);
}

void LinePreprocessor::remove_slash_r( std::string &input )
{
    if (!input.empty() && input.back() == '\r')
        input.pop_back();
}

void LinePreprocessor::remove_comments( std::string &input )
{
    const size_t comment_start = input.find_first_of(';');

    if (comment_start != std::string::npos)
        input = input.substr(0, comment_start);
}
