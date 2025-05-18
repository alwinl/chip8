/*
 * parser.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <vector>
#include <string>
#include <ostream>

#include "tokeniser.h"
// #include "ast.h"

class Parser {
public:
    Parser(Tokeniser tokeniser);

    // std::unique_ptr<Stmt> parse_statement();
    // std::unique_ptr<Expr> parse_expression();

private:
    Tokeniser tokeniser;
    Token current;

    void advance();
    bool match(Token::Type type);
    Token consume(Token::Type type, const std::string& message);
};