/*
 * parser.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
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

void Parser::forward_cursor()
{
    ++cursor;

    while( peek().type == Token::Type::COMMENT )
        ++cursor; // consume the comment
}

bool Parser::match(Token::Type type, std::string lexeme )
{
    if( peek().type != type )
        return false;

    if( !lexeme.empty() && peek().lexeme != lexeme )
        return false;

    return true;
}

Token Parser::consume(Token::Type type, const std::string& lexeme, const std::string& message)
{
    if( match( type, lexeme ) ) {
        Token token = *cursor;
        forward_cursor();
        return token;
    }

    throw std::runtime_error(
        "FileName:" + std::to_string(peek().line) + ":" + std::to_string(peek().column)
        + ": Parse error: " + message + " (expected '" + lexeme + "', got '" + peek().lexeme + "')" );
}


/*
<line>          ::= <label> | <variable_def> | <instruction> | <directive>

<instruction>   ::= <label>? IDENTIFIER <argument_list>?
<directive>     ::= <label>? DIRECTIVE <argument_list>?
<variable_def>  ::= IDENTIFIER ("=" | "EQU") <argument_list>
<label>         ::= LABEL

<argument_list> ::= <argument> ( COMMA <argument> )*
<argument>      ::= IDENTIFIER | NUMBER
*/

// LineNode Parser::parse_variable_def()
// {

// }

// LineNode Parser::parse_instruction()
// {
// 	LineNode node = parse_arguments();

// 	node.kind = LineNode::Kind::INSTRUCTION;

// 	return node;
// }


// LineNode Parser::parse_directive()
// {
// 	forward_cursor();		// no need to consume

// 	LineNode node = parse_arguments();
// 	node.kind = LineNode::Kind::DIRECTIVE;

// 	return node;
// }

// LineNode Parser::parse_identifier_line()
// {
// 	std::string identifier = consume( Token::Type::IDENTIFIER, "", "Expected identifier" ).lexeme;

// 	if( match( Token::Type::ASSIGNMENT ) ) {
// 		LineNode node = parse_variable_def();
// 		node.variable_name = identifier;
// 		return node;
// 	}

// 	LineNode node = parse_arguments( );
// 	node.mnemonic = identifier;

// 	return node;
// }

// LineNode Parser::parse_labeled_line()
// {
// 	int line_number = peek().line;
// 	std::string label = consume( Token::Type::LABEL, "", "Expected label" ).lexeme;

// 	if( peek().line != line_number ) {
// 		LineNode node;
// 		node.label = label;
// 		return node;
// 	}

// 	if( match( Token::Type::DIRECTIVE) ) {
// 		LineNode node =  parse_directive( );
// 		node.label = label;
// 		return node;
// 	}

// 	if( match( Token::Type::IDENTIFIER) ) {
// 		LineNode node =  parse_instruction( );
// 		node.mnemonic = label;
// 		return node;
// 	}

// 	// parser error
// }

// LineNode Parser::parse_line()
// {
// 	std::string label;

// 	if( match( Token::Type::LABEL) )
// 		return parse_labeled_line();

// 	if( match( Token::Type::DIRECTIVE) )
// 		return parse_directive();

// 	if( match( Token::Type::IDENTIFIER) )
// 		return parse_identifier_line();

// 	// parser error
// }

// Program Parser::parse_program()
// {
//     Program program;

//     while( ! match( Token::Type::END_OF_INPUT ) )
//         program.lines.push_back( parse_line() );

// }


Program Parser::parse_all()
{
    Program program;

    while( match( Token::Type::COMMENT ) )
        ++cursor;

    while( ! match( Token::Type::END_OF_INPUT ) )
        program.lines.push_back( next_line() );

    return program;
}

LineNode Parser::next_line()
{
    LineNode node;
    int start_line;

    node.line_number = start_line = peek().line;

	if( match( Token::Type::LABEL) )
		node.label = consume( Token::Type::LABEL, "", "Expected label" ).lexeme;

    if( !end_of_current_line(start_line) && match( Token::Type::IDENTIFIER)  )
	{
        auto first = cursor;
        auto second = first;
        ++second;

        if( is_variable_definition( second ) ) {

            node.kind = LineNode::Kind::VARIABLE;

            node.variable_name = consume( Token::Type::IDENTIFIER, "", "Expected symbol name" ).lexeme;
            node.mnemonic = consume( Token::Type::IDENTIFIER, "EQU", "Expected EQU" ).lexeme;

            while( !end_of_current_line(start_line) )
                node.arguments.push_back( consume( peek().type, "", "Expected expression").lexeme );

        } else {

			node.mnemonic = consume( Token::Type::IDENTIFIER, "", "Expected mnemonic" ).lexeme;
			node.kind = node.mnemonic.starts_with(".") ? LineNode::Kind::DIRECTIVE : LineNode::Kind::INSTRUCTION;

			while( !end_of_current_line(start_line) && (match(Token::Type::IDENTIFIER) || match(Token::Type::NUMBER)) )
				node.arguments.push_back( consume(peek().type, "", "Expected argument").lexeme );

		}
    }

    while( !end_of_current_line(start_line) )
        forward_cursor();

    return node;
}

bool Parser::end_of_current_line(int start_line)
{
	return match(Token::Type::END_OF_INPUT) || peek().line != start_line;
}

bool Parser::is_variable_definition( Tokens::const_iterator &token )
{
	if( token == tokens.end() )
		return false;

	if( token->type != Token::Type::ASSIGNMENT )
		return false;

	return true;
}
