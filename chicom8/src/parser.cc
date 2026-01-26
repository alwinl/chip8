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

#include "unordered_map"

std::unordered_map<std::string, VarDecl::eTypes> string_to_type = {
    { "nibble", VarDecl::eTypes::NIBBLE  },
    { "byte"  , VarDecl::eTypes::BYTE    },
    { "snack" , VarDecl::eTypes::SNACK   },
    { "number", VarDecl::eTypes::NUMBER  },
    { "bool"  , VarDecl::eTypes::BOOL    },
    { "key"   , VarDecl::eTypes::KEY     },
    { "sprite", VarDecl::eTypes::SPRITE  },
};


void Parser::forward_cursor()
{
    ++cursor;

    while( peek().type == Token::Type::COMMENT )
        ++cursor;
}

bool Parser::match( Token::Type type, std::string lexeme )
{
    if( peek().type != type )
        return false;

    if( !lexeme.empty() && peek().lexeme != lexeme )
        return false;

    return true;
}

Token Parser::consume( Token::Type type, const std::string& message )
{
    if( match( type ) ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error("Parse error: " + message);
}

Token Parser::consume( Token::Type type, const std::string &lexeme, const std::string &message )
{
    if( match( type, lexeme ) ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error(
        "Parse error: " + message +
        "(expected '" + lexeme + "', got '" + peek().lexeme + "')" +
        " at line " + std::to_string(peek().line) );
}

Program Parser::make_AST( const Tokens tokens )
{
    Program program;

	this->tokens = tokens;

    cursor = tokens.begin();

    while( match( Token::Type::COMMENT ) )
        ++cursor;

    while( ! match( Token::Type::END_OF_INPUT ) )
        program.declarations.push_back(parse_decl());

    return program;
}

Program Parser::make_AST( const std::string& source )
{
	Tokeniser tokeniser( source );
    Program program;

	this->tokens = tokeniser.tokenise_all();

    cursor = tokens.begin();

    while( match( Token::Type::COMMENT ) )
        ++cursor;

    while( ! match( Token::Type::END_OF_INPUT ) )
        program.declarations.push_back(parse_decl());

    return program;
}

Program Parser::make_AST( std::filesystem::path file_path )
{
	Tokeniser tokeniser( file_path );
    Program program;

	this->tokens = tokeniser.tokenise_all();

    cursor = tokens.begin();

    while( match( Token::Type::COMMENT ) )
        ++cursor;

    while( ! match( Token::Type::END_OF_INPUT ) )
        program.declarations.push_back(parse_decl());

    return program;
}

std::unique_ptr<Decl> Parser::parse_decl()
{
    if( match( Token::Type::KEYWORD, "func" ) )
        return parse_func_decl();

    if( match( Token::Type::KEYWORD, "var" ) )
        return parse_var_decl();

    throw std::runtime_error("Expected declaration (var or func)" );
}

std::unique_ptr<Decl> Parser::parse_func_decl()
{
    std::vector<std::unique_ptr<VarDecl>> params;
    std::vector<std::unique_ptr<VarDecl>> vars;
    std::vector<std::unique_ptr<Stmt>> body;

    consume( Token::Type::KEYWORD, "func", "Expected 'func' keyword" );

    std::unique_ptr<Identifier> name = parse_identifier( "Expected function name" );

    consume( Token::Type::PUNCTUATION, "(", "Unexpected token" );

    while( ! match( Token::Type::PUNCTUATION ) && ! match( Token::Type::END_OF_INPUT ) ) {

        std::unique_ptr<Identifier> name = parse_identifier( "Expected variable name" );
        // Token name = consume( Token::Type::IDENTIFIER, "Expected variable name" );
        consume( Token::Type::PUNCTUATION, ":", "Unexpected token" );

        Token const tok = consume( Token::Type::TYPE_KEYWORD, "Expected type keyword name" );

        VarDecl::eTypes const type = string_to_type.at( tok.lexeme );
        std::unique_ptr<Expr> byte_size = ( tok.lexeme == "sprite" ) ? parse_expr() : nullptr;

        params.push_back( std::make_unique<VarDecl>( std::move(name), type, std::move(byte_size) ) );

        if( match( Token::Type::PUNCTUATION, "," ))
            consume( Token::Type::PUNCTUATION, "Expected comma" );
    }

    consume( Token::Type::PUNCTUATION, ")", "Unexpected token" );

    consume( Token::Type::PUNCTUATION, "{", "Unexpected token" );

    while( ! match( Token::Type::PUNCTUATION ) && ! match( Token::Type::END_OF_INPUT ) ) {

        if( match( Token::Type::KEYWORD, "var" ) )
            vars.push_back( parse_var_decl() );
        else
            body.push_back( parse_stmt() );
    }

    consume( Token::Type::PUNCTUATION, "}", "Unexpected token" );

    return std::make_unique<FuncDecl>(  std::move(name), std::move(params), std::move(vars), std::move(body) );
}

std::unique_ptr<VarDecl> Parser::parse_var_decl()
{
    consume( Token::Type::KEYWORD, "var", "Expected 'var' keyword" );
    std::unique_ptr<Identifier> name = parse_identifier( "Expected variable name" );
    consume( Token::Type::PUNCTUATION, ":", "Unexpected token" );

    Token const tok = consume( Token::Type::TYPE_KEYWORD, "Expected type keyword name" );

    VarDecl::eTypes const type = string_to_type.at( tok.lexeme );
    std::unique_ptr<Expr> byte_size = ( tok.lexeme == "sprite" ) ? parse_expr() : nullptr;
    // int byte_size = 0;

    // if( tok.lexeme == "sprite" ) {
    //     Token nibble = consume( Token::Type::NUMBER, "Expected a nibble" );
    //     byte_size = std::atoi( nibble.lexeme.c_str() );
    // }

    consume( Token::Type::PUNCTUATION, ";", "Unexpected token" );

    return std::make_unique<VarDecl>( std::move(name), type, std::move(byte_size) );
}


std::unique_ptr<Stmt> Parser::parse_stmt()
{
    static std::unordered_map<std::string, std::unique_ptr<Stmt> (Parser::*)()> string_to_stmt_func = {
        { "if",     &Parser::parse_if_stmt     },
        { "while",  &Parser::parse_while_stmt  },
        { "draw",   &Parser::parse_draw_stmt   },
        { "return", &Parser::parse_return_stmt },
        { "{",      &Parser::parse_block       }
    };

    auto it = string_to_stmt_func.find( peek().lexeme );

    if( it != string_to_stmt_func.end() )
        return (this->*(it->second))();

    return parse_expr_stmt();
}

std::unique_ptr<Stmt> Parser::parse_expr_stmt()
{
    auto expr = parse_expr();

    consume( Token::Type::PUNCTUATION, "Expected ';' after statement" );

    return std::make_unique<ExprStmt>( std::move( expr) );
}

std::unique_ptr<Stmt> Parser::parse_if_stmt()
{
    consume( Token::Type::KEYWORD, "if", "Expected 'if' keyword" );
    consume( Token::Type::PUNCTUATION, "(", "Expected '(' after 'if'" );

    auto condition = parse_expr();

    consume( Token::Type::PUNCTUATION, ")", "Expected ')' after if condition" );

    auto then_branch = parse_stmt();

    // optional 'else'
    std::unique_ptr<Stmt> else_branch = nullptr;

    if( match(Token::Type::KEYWORD, "else") ) {
        consume( Token::Type::KEYWORD, "else", "Expected 'else'" );
        else_branch = parse_stmt();
    }

    return std::make_unique<IfStmt>( std::move(condition), std::move(then_branch), std::move(else_branch) );
}

std::unique_ptr<Stmt> Parser::parse_while_stmt()
{
    consume( Token::Type::KEYWORD, "while", "Expected 'while' keyword" );
    consume( Token::Type::PUNCTUATION, "(", "Expected '(' after 'while'" );

    auto condition = parse_expr();

    consume( Token::Type::PUNCTUATION, ")", "Expected ')' after while condition" );

    std::unique_ptr<Stmt> body = parse_stmt();

    return std::make_unique<WhileStmt>( std::move(condition), std::move(body) );
}

std::unique_ptr<Stmt> Parser::parse_draw_stmt()
{
    consume( Token::Type::KEYWORD, "draw", "Expected 'draw' keyword" );

    consume( Token::Type::PUNCTUATION, "(", "Expected '(' after draw keyword" );

    std::unique_ptr<Identifier> sprite = parse_identifier( "Expected sprite identifier" );

    consume( Token::Type::PUNCTUATION, "Expected ',' between identifier and x" );
    std::unique_ptr<Expr> x = parse_expr();

    consume( Token::Type::PUNCTUATION, "Expected ',' between x and y" );
    std::unique_ptr<Expr> y = parse_expr();

    std::unique_ptr<Expr> height = nullptr;

    if( match(Token::Type::PUNCTUATION, ",") ) {
        consume( Token::Type::PUNCTUATION, ",", "" );
        height = parse_expr();
    }

    consume( Token::Type::PUNCTUATION, ")", "Expected ')' after draw arguments" );
    consume( Token::Type::PUNCTUATION, ";", "Expected ';' after draw statement" );

    return std::make_unique<DrawStmt>( std::move(sprite), std::move(x), std::move(y), std::move(height) );
}

std::unique_ptr<Stmt> Parser::parse_return_stmt()
{
    consume( Token::Type::KEYWORD, "Expected 'return' keyword" );

    std::unique_ptr<Expr> value = nullptr;

    if( !match(Token::Type::PUNCTUATION, ";"))
        value = parse_expr();

    consume( Token::Type::PUNCTUATION, "Expected ';' after return statement" );

    return std::make_unique<ReturnStmt>( std::move(value) );
}

std::unique_ptr<Stmt> Parser::parse_block()
{
    consume( Token::Type::PUNCTUATION, "Expected '{' to start block" );

    BlockStmt block;

    while( !match(Token::Type::PUNCTUATION, "}") && peek().type != Token::Type::END_OF_INPUT )
        block.statements.push_back(parse_stmt());

    consume( Token::Type::PUNCTUATION, "Expected '}' after block" );

    return std::make_unique<BlockStmt>( std::move( block ) );
}


std::unique_ptr<Expr> Parser::parse_expr()
{
    std::unique_ptr<Expr> lhs = parse_logic_or();

    if( auto *id = dynamic_cast<Identifier*>(lhs.get()) ) {

        if( match(Token::Type::OPERATOR, "=") ) {

            Token const eq = consume( Token::Type::OPERATOR, "=", "Expected '=' in assignment" );
            std::unique_ptr<Expr> rhs = parse_expr();

            return std::make_unique<BinaryExpr>(
                std::move(lhs),
                BinaryExpr::Operator::ASSIGN,
                std::move(rhs)
            );
        }
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_logic_or()
{
    std::unique_ptr<Expr> lhs = parse_logic_and();

    while( match( Token::Type::OPERATOR, "||") ) {

        Token const tok = consume( Token::Type::OPERATOR, "||", "Expected '||' operator" );
        std::unique_ptr<Expr> rhs = parse_logic_and();
        lhs = std::make_unique<BinaryExpr>( std::move(lhs), BinaryExpr::Operator::LOGIC_OR, std::move( rhs) );
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_logic_and()
{
    std::unique_ptr<Expr> lhs = parse_equality();

    while( match( Token::Type::OPERATOR, "&&") ) {

        Token const tok = consume( Token::Type::OPERATOR,  "&&", "Expected '&&' operator" );
        std::unique_ptr<Expr> rhs = parse_equality();
        lhs = std::make_unique<BinaryExpr>( std::move(lhs), BinaryExpr::Operator::LOGIC_AND, std::move( rhs) );
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_equality()
{
    std::unique_ptr<Expr> lhs = parse_comparison();

    while( match( Token::Type::OPERATOR, "==") || match( Token::Type::OPERATOR, "!=") ) {

        Token const tok = consume( Token::Type::OPERATOR, "" );
        std::unique_ptr<Expr> rhs = parse_comparison();
        lhs = std::make_unique<BinaryExpr>(
            std::move(lhs),
            ( tok.lexeme == "==" ) ? BinaryExpr::Operator::EQUALS : BinaryExpr::Operator::NOT_EQ,
            std::move( rhs)
        );
    }

    return lhs;
}


std::unique_ptr<Expr> Parser::parse_comparison()
{
    std::unique_ptr<Expr> lhs = parse_term();

    while( match( Token::Type::OPERATOR, ">") || match( Token::Type::OPERATOR, ">=")
        || match( Token::Type::OPERATOR, "<") || match( Token::Type::OPERATOR, "<=") ) {

        Token const tok = consume( Token::Type::OPERATOR, "" );
        std::unique_ptr<Expr> rhs = parse_term();
        lhs = std::make_unique<BinaryExpr>(
            std::move(lhs),
            ( tok.lexeme == ">" ) ? BinaryExpr::Operator::GREATER_THAN :
                ( tok.lexeme == ">=" ) ? BinaryExpr::Operator::GREATER_EQ :
                    ( tok.lexeme == "<" ) ? BinaryExpr::Operator::LESS_THAN :
                        BinaryExpr::Operator::LESS_EQ,
            std::move( rhs)
        );
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_term()
{
    std::unique_ptr<Expr> lhs = parse_factor();

    while( match( Token::Type::OPERATOR, "+") || match( Token::Type::OPERATOR, "-") ) {

        Token const tok = consume( Token::Type::OPERATOR, "" );
        std::unique_ptr<Expr> rhs = parse_factor();
        lhs = std::make_unique<BinaryExpr>(
            std::move(lhs),
            ( tok.lexeme == "+" ) ? BinaryExpr::Operator::ADD : BinaryExpr::Operator::SUBTRACT,
            std::move( rhs)
        );
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_factor()
{
    std::unique_ptr<Expr> lhs = parse_unary();

    while( match( Token::Type::OPERATOR, "*") || match( Token::Type::OPERATOR, "/") ) {

        Token const tok = consume( Token::Type::OPERATOR, "" );
        std::unique_ptr<Expr> rhs = parse_unary();
        lhs = std::make_unique<BinaryExpr>(
            std::move(lhs),
            ( tok.lexeme == "*" ) ? BinaryExpr::Operator::MULTIPLY : BinaryExpr::Operator::DIVIDE,
            std::move( rhs)
        );
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_unary()
{
    if( match( Token::Type::OPERATOR, "!") )
        return std::make_unique<UnaryExpr>( parse_unary(), UnaryExpr::Operator::NOT );

    if( match( Token::Type::OPERATOR, "-") )
        return std::make_unique<UnaryExpr>( parse_unary(), UnaryExpr::Operator::NEGATIVE );

    return parse_primary();
}

std::unique_ptr<Expr> Parser::parse_primary()
{
    if( match( Token::Type::NUMBER) ) {

        Token const number = consume( Token::Type::NUMBER, "Expected number" );
        int const value = std::stoi( number.lexeme );
        return std::make_unique<Number>( value );
    }

    if( match( Token::Type::KEYWORD, "true" ) ) {

        consume( Token::Type::KEYWORD, "true", "Expected 'true'" );
        return std::make_unique<Bool>( true );
    }

    if( match( Token::Type::KEYWORD, "false" ) ) {

        consume( Token::Type::KEYWORD, "false", "Expected 'false'" );
        return std::make_unique<Bool>( false );
    }

    if( match( Token::Type::KEYWORD, "bcd" ) ) {

        consume( Token::Type::KEYWORD, "bcd", "Expected 'bcd'" );
        Token const id = consume( Token::Type::IDENTIFIER, "Expected identifier after 'bcd'" );
        return std::make_unique<BcdExpr>( std::make_unique<Identifier>(id.lexeme));
    }

    if( match( Token::Type::KEYWORD, "rnd" ) ) {

        consume( Token::Type::KEYWORD, "rnd", "Expected 'rnd'" );
        // Token id = consume( Token::Type::IDENTIFIER, "Expected identifier after 'rnd'" );
        // return std::make_unique<RndExpr>( std::make_unique<Identifier>(id.lexeme));
        return std::make_unique<RndExpr>( parse_expr() );
    }

    if( match( Token::Type::KEYWORD, "keydown" ) ) {

        consume( Token::Type::KEYWORD, "keydown", "Expected 'keydown'" );
        Token const id = consume( Token::Type::IDENTIFIER, "Expected identifier after 'keydown'" );
        return std::make_unique<KeyDownExpr>( std::make_unique<Identifier>(id.lexeme));
    }

    if( match( Token::Type::KEYWORD, "getkey" ) ) {

        consume( Token::Type::KEYWORD, "getkey", "Expected 'getkey'" );
        return std::make_unique<GetKeyExpr>();
    }

    if( match( Token::Type::PUNCTUATION, "(" ) ) {

        consume( Token::Type::PUNCTUATION, "Expected '('" );
        std::unique_ptr<Expr> expr = parse_expr( );
        consume( Token::Type::PUNCTUATION, "Expected ')'" );

        return std::make_unique<BracedExpr>( std::move(expr) );
    }

    if( match( Token::Type::IDENTIFIER ) ) {

        std::unique_ptr<Identifier> name = parse_identifier( "Expected an identifier" );

        if( !match( Token::Type::PUNCTUATION, "(" ) )
            return name;

        consume( Token::Type::PUNCTUATION, "(", "Expected '(' for function call" );

        std::vector<std::unique_ptr<Expr>> args;

        if( !match(Token::Type::PUNCTUATION, ")") ) {

            while( true ) {
                args.push_back(parse_expr());

                if( match( Token::Type::PUNCTUATION, "," ) )
                    consume( Token::Type::PUNCTUATION, ",", "Expected ',' between function arguments" );
                else
                    break;
            }
        }

        consume( Token::Type::PUNCTUATION, "Expected ')' to close function call" );

        return std::make_unique<FuncCallExpr>( std::move(name), std::move(args) );
    }

    throw std::runtime_error("Expected expression." );
}

std::unique_ptr<Identifier> Parser::parse_identifier( std::string error_message )
{
    Token const name = consume( Token::Type::IDENTIFIER, error_message );

    return std::make_unique<Identifier>(name.lexeme);
}

std::unique_ptr<Number> Parser::parse_number( std::string error_message )
{
    Token const tok = consume(Token::Type::NUMBER, error_message);

    int const value = 42; //parse_int_literal(tok.lexeme); // handle hex, decimal, etc.

    return std::make_unique<Number>(value);
}