/*
 * ebnf_parser.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <vector>
#include <ostream>

#include "ebnf_tokeniser.h"

struct Symbol
{
    Token token = { Token::Type::INVALID, "" };
    bool optional = false;
    bool repeated = false;
    std::vector<Symbol> symbol_group;
};

using Symbols = std::vector<Symbol>;
using Production = Symbols;
using Productions = std::vector<Production>;

struct Rule
{
    std::string name;
    Productions productions;
};

using Rules = std::vector<Rule>;

std::ostream& operator<<( std::ostream& os, const Symbol& symbol );
std::ostream& operator<<( std::ostream& os, const Production& production );
std::ostream& operator<<( std::ostream& os, const Rule& rule );
std::ostream& operator<<( std::ostream& os, const Rules& rules );

class Parser
{
public:
    Parser( const Tokens& tokens ) : tokens(tokens ) {}

    Rules parse_rules();

private:
    const Tokens& tokens;
    Tokens::const_iterator cursor;

    std::string parse_rule_name();
    void parse_colon_eq();
    Symbols parse_production( bool in_group );

    Symbol apply_modifier( Symbol& symbol);
    const Token& peek() { return (cursor != tokens.end()) ? *cursor : tokens.back(); }
    void forward_cursor();
};
