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

#include "chip8_ast.h"
#include "tokeniser.h"

class Parser {
public:
    Parser( const Tokens& tokens ) : tokens(tokens ) {}

    Program AST();

protected:
    const Tokens& tokens;
    Tokens::const_iterator cursor;

    void forward_cursor();
    const Token& peek() { return (cursor != tokens.end()) ? *cursor : tokens.back(); }
    bool match(Token::Type type, std::string lexeme = "" );
    Token consume(Token::Type type, const std::string& message);
    Token consume(Token::Type type, const std::string& lexeme, const std::string& message);

    std::unique_ptr<Decl> parse_decl();
    std::unique_ptr<Decl> parse_func_decl();
    std::unique_ptr<VarDecl> parse_var_decl();

    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<Stmt> parse_expr_stmt();
    std::unique_ptr<Stmt> parse_if_stmt();
    std::unique_ptr<Stmt> parse_while_stmt();
    std::unique_ptr<Stmt> parse_draw_stmt();
    std::unique_ptr<Stmt> parse_return_stmt();
    std::unique_ptr<Stmt> parse_block();

    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_logic_or();
    std::unique_ptr<Expr> parse_logic_and();
    std::unique_ptr<Expr> parse_equality();
    std::unique_ptr<Expr> parse_comparison();
    std::unique_ptr<Expr> parse_term();
    std::unique_ptr<Expr> parse_factor();
    std::unique_ptr<Expr> parse_unary();
    std::unique_ptr<Expr> parse_primary();

    std::unique_ptr<Identifier> parse_identifier( std::string error_message );
    std::unique_ptr<Number> parse_number( std::string error_message );
};