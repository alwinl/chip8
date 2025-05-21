/*
 * ebnf_generator.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <ostream>
#include <unordered_map>
#include <string>
#include <unordered_set>

#include "ebnf_parser.h"
#include "ebnf_tarjan.h"

using RuleMap = std::unordered_map<std::string,Rule>;

class Generator
{
public:
    Generator( ) {};

    void generateAstFiles( std::ostream& os, const Grammar& rules);

	void generateHeader( std::ostream &os );
    void generateGroupStructures( std::ostream & os, std::string base_name, const Part& part );

	void generateAstClass( std::ostream &os, const Rule &rule );
	void generateVariant( std::ostream &os, const Production &production );
	void generateMultiProduction( std::ostream &os, std::string &className, const Rule &rule );
	void generateSingleProduction( std::ostream &os, std::string &className, const Rule &rule );

private:
    Graph build_graph( const Grammar& rules );


    void debugPrintDependencies( const Graph& graph ) const;
};

