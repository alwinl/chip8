/*
 * parser.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "parser.h"

#include <unordered_map>

std::ostream& operator<<( std::ostream& os, const Expr& expr )
{
    return os;
}

std::ostream& operator<<( std::ostream& os, const Stmt& stmt )
{
    return os;
}

std::ostream& operator<<( std::ostream& os, const Type& type_decl )
{
    std::unordered_map<Type::eTypeTypes, std::string> type_to_string = {
        { Type::eTypeTypes::NIBBLE, "nibble" },
        { Type::eTypeTypes::BYTE, "byte" },
        { Type::eTypeTypes::SNACK, "snack" },
        { Type::eTypeTypes::NUMBER, "number" },
        { Type::eTypeTypes::BOOL, "bool" },
        { Type::eTypeTypes::KEY, "key" },
        { Type::eTypeTypes::SPRITE, "sprite" },
    };

    os << type_to_string.at( type_decl.type );
    if( type_decl.type == Type::eTypeTypes::SPRITE )
        os << " " << type_decl.sprite_size;

    return os;
}

std::ostream& operator<<( std::ostream& os, const VarDecl& var_decl )
{
    os << "  VarDecl( " << var_decl.identifier << ": " << var_decl.type << ")";
    return os;
}

std::ostream& operator<<( std::ostream& os, const TypedIdentifier& typed_identifier )
{
    os << typed_identifier.identifier << ": " << typed_identifier.type;
    return os;
}

std::ostream& operator<<( std::ostream& os, const ParamList& params )
{
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0)
            os << ", ";

        os << params[i];
    }

    return os;
}

std::ostream& operator<<( std::ostream& os, const FuncDecl& func_decl )
{
    os << "FuncDecl(" << func_decl.identifier << "(" << func_decl.params << ")\n{";

    for (size_t i = 0; i < func_decl.body.size(); ++i) {
        if (i > 0)
            os << ", ";

        os << func_decl.body[i];
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const Decl& declaration )
{
    std::visit([&os](const auto& value) {
        os << value;  // calls the correct operator<< for VarDecl or FuncDecl
    }, declaration);

    return os;
}

std::ostream& operator<<( std::ostream& os, const Program& program )
{
    os << "Program: [\n";

    bool first = true;
    for( const auto& declaration : program.declarations ) {
        if( first )
            first = false;
        else
            os << ",\n";

        os << declaration;
    }

    os << "\n]";

    return os;
}


void Parser::forward_cursor()
{
    ++cursor;

    while( peek().type == Token::Type::COMMENT )
        ++cursor;
}

bool Parser::match( Token::Type type )
{
    if( peek().type == type ) {
        forward_cursor();
        return true;
    }

    return false;
}

Token Parser::consume(Token::Type type, const std::string& message)
{
    if( peek().type == type ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error("Parse error: " + message);
}

Token Parser::consume( Token::Type type, const std::string &lexeme, const std::string &message )
{
    const Token& t = peek();

    if( t.type == type && t.lexeme == lexeme ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error(
        "Parse error: " + message +
        "(expected '" + lexeme + "', got '" + t.lexeme + "')" +
        " at line " + std::to_string(t.line) );
}

Program Parser::AST()
{
    Program program;

    cursor = tokens.begin();

    while( peek().type == Token::Type::COMMENT )
        ++cursor;

    while (peek().type != Token::Type::END_OF_INPUT)
        program.declarations.push_back(parse_decl());

    return program;
}

Decl Parser::parse_decl()
{
    if( peek().lexeme == "func" )
        return parse_func_decl();

    if( peek().lexeme == "var" )
        return parse_var_decl();

    throw std::runtime_error("Expected declaration (var or func)");
}

FuncDecl Parser::parse_func_decl()
{
    ParamList params;
    std::vector<VarDecl> vars;
    std::vector<std::unique_ptr<Stmt>> body;

    consume(Token::Type::KEYWORD, "func", "Expected 'func' keyword");
    Token name = consume(Token::Type::IDENTIFIER, "Expected function name");

    consume(Token::Type::PUNCTUATION, "(", "Unexpected token");
    if( peek().type != Token::Type::PUNCTUATION )
        params = parse_param_list();
    consume(Token::Type::PUNCTUATION, ")", "Unexpected token");

    consume(Token::Type::PUNCTUATION, "{", "Unexpected token");

    while (peek().type != Token::Type::PUNCTUATION) {
        if( peek().lexeme == "var" )
            vars.push_back( parse_var_decl() );
        else
            body.push_back( parse_stmt() );
    }

    consume(Token::Type::PUNCTUATION, "}", "Unexpected token");

    return FuncDecl{ name.lexeme, params, vars, std::move(body) };
}

ParamList Parser::parse_param_list()
{
    ParamList params;

    while( peek().type != Token::Type::PUNCTUATION ) {

        params.push_back( parse_typed_identifier() );

        Token t = peek();
        if( t.type == Token::Type::PUNCTUATION && t.lexeme == "," )
            consume( Token::Type::PUNCTUATION, "Expected comma");
    }

	return params;
}

TypedIdentifier Parser::parse_typed_identifier()
{
    TypedIdentifier typed_identifier;

    Token name = consume(Token::Type::IDENTIFIER, "Expected variable name");
    consume(Token::Type::PUNCTUATION, ":", "Unexpected token");

    Type type_value = parse_type();

    return TypedIdentifier{ .identifier = name.lexeme, .type = type_value };
}

VarDecl Parser::parse_var_decl()
{
    consume( Token::Type::KEYWORD, "var", "Expected 'var' keyword" );
    Token name = consume( Token::Type::IDENTIFIER, "Expected variable name" );
    consume(Token::Type::PUNCTUATION, ":", "Unexpected token");

    Type type_value = parse_type();

    consume(Token::Type::PUNCTUATION, ";", "Unexpected token");

    return VarDecl{ name.lexeme, type_value };
}

Type Parser::parse_type()
{
    static std::unordered_map<std::string, Type::eTypeTypes> string_to_type = {
        { "nibble", Type::eTypeTypes::NIBBLE  },
        { "byte"  , Type::eTypeTypes::BYTE    },
        { "snack" , Type::eTypeTypes::SNACK   },
        { "number", Type::eTypeTypes::NUMBER  },
        { "bool"  , Type::eTypeTypes::BOOL    },
        { "key"   , Type::eTypeTypes::KEY     },
        { "sprite", Type::eTypeTypes::SPRITE  },
    };
    Type value;

    Token type = consume(Token::Type::TYPE_KEYWORD, "Expected type keyword name");

    value.type = string_to_type.at(type.lexeme);

    if( type.lexeme == "sprite" ) {
        Token nibble = consume(Token::Type::NUMBER, "Expected a nibble");
        value.sprite_size = std::atoi( nibble.lexeme.c_str() );
    }

    return value;
}

std::unique_ptr<Stmt> Parser::parse_stmt()
{
    static std::unordered_map<std::string, std::unique_ptr<Stmt> (Parser::*)()> string_to_stmt_func = {
        { "if",     &Parser::parse_if_stmt     },
        { "while",  &Parser::parse_while_stmt  },
        { "draw",   &Parser::parse_draw_stmt   },
        { "return", &Parser::parse_return_stmt },
        { "getkey", &Parser::parse_getkey_stmt },
        { "{",      &Parser::parse_block       }
    };

    auto it = string_to_stmt_func.find( peek().lexeme );

    if( it == string_to_stmt_func.end() )
        throw std::runtime_error("Expected statement");

    return (this->*(it->second))();
}

std::unique_ptr<Stmt> Parser::parse_if_stmt()
{
    // consume 'if'
    consume(Token::Type::KEYWORD, "Expected 'if' keyword");

    // consume '('
    consume(Token::Type::PUNCTUATION, "Expected '(' after 'if'");

    // parse condition
    auto condition = parse_expr();

    // consume ')'
    consume(Token::Type::PUNCTUATION, "Expected ')' after if condition");

    // parse the 'then' branch
    auto then_branch = parse_stmt();

    // optional 'else'
    std::unique_ptr<Stmt> else_branch = nullptr;
    if (peek().lexeme == "else") {
        forward_cursor(); // consume 'else'
        else_branch = parse_stmt();
    }

    // build and return

    return std::make_unique<Stmt>( IfStmt { std::move(condition), std::move(then_branch), std::move(else_branch) } );
}


std::unique_ptr<Stmt> Parser::parse_while_stmt()
{
    return std::make_unique<Stmt>(Stmt{DummyStmt()});
}

std::unique_ptr<Stmt> Parser::parse_draw_stmt()
{
    return std::make_unique<Stmt>(Stmt{DummyStmt()});
}

std::unique_ptr<Stmt> Parser::parse_return_stmt()
{
    return std::make_unique<Stmt>(Stmt{DummyStmt()});
}

std::unique_ptr<Stmt> Parser::parse_getkey_stmt()
{
    return std::make_unique<Stmt>( DummyStmt() );
}

std::unique_ptr<Stmt> Parser::parse_block()
{
    consume(Token::Type::PUNCTUATION, "Expected '{' to start block");

    Block block;

    while (peek().type != Token::Type::END_OF_INPUT &&
           peek().lexeme != "}")
    {
        block.statements.push_back(parse_stmt());
    }

    consume(Token::Type::PUNCTUATION, "Expected '}' after block");

    return std::make_unique<Stmt>( block );
}

std::unique_ptr<Expr> Parser::parse_expr()
{
	return std::make_unique<Expr>(Expr{});
}

// std::unique_ptr<Stmt> Parser::parse_statement()
// {
    // if (match(Token::Type::KEYWORD)) {
    //     Token name = consume(Token::Type::Identifier, "Expected variable name.");
    //     consume(Token::Type::Assign, "Expected '=' after variable name.");
    //     auto value = parse_expression();
    //     consume(Token::Type::Semicolon, "Expected ';' after expression.");
    //     return std::make_unique<LetStmt>(name.lexeme, std::move(value));
    // }

//     throw std::runtime_error("Expected statement.");
// }

// std::unique_ptr<Expr> Parser::parse_expression()
// {
    // if (current.type == Token::Type::NUMBER) {

    //     int value = std::stoi(current.lexeme);
    //     current = tokeniser.next_token();
    //     return std::make_unique<NumberLiteral>(value);

    // } else if (current.type == Token::Type::IDENTIFIER) {

    //     std::string name = current.lexeme;
    //     current = tokeniser.next_token();
    //     return std::make_unique<Variable>(name);
        
    // }

//     throw std::runtime_error("Expected expression.");
// }
