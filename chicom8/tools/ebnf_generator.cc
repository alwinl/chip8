/*
 * ebnf_generator.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "ebnf_generator.h"

#include <string>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <memory>
#include <cctype>
#include <algorithm>
#include <functional>

#include "ebnf_tarjan.h"

std::string capitalize( const std::string& s )
{
    std::string result = s;

    if( !result.empty() ) 
        result[0] = toupper( result[0] );

    return result;
}

std::string sanitize_type_name(const std::string& name)
{
    if( name == "string" )
        return "std::string";

    return capitalize(name);
}

std::string get_symbol_type( const Symbol& symbol, const std::string& parentRuleName, int groupIndex )
{
    std::string symbol_type("/* UnknownType */");

    if( !symbol.symbol_group.empty() )
        symbol_type = parentRuleName + "Group" + std::to_string(groupIndex);

    if( symbol.token.type == Token::Type::NONTERMINAL )
        symbol_type = "std::shared_ptr<" + sanitize_type_name(symbol.token.lexeme) + ">";
        
    if( symbol.token.type == Token::Type::STRING_LITERAL )
        symbol_type = "std::string";  // token literals can just be strings

    if( symbol.repeated )
        return "std::vector<" + symbol_type + ">";

    if( symbol.optional )
        return "std::optional<" + symbol_type + ">";

    return symbol_type;
}

std::string build_field( const Symbol & symbol, int spaces, const std::string& groupName, int index )
{
    // if( symbol.token.type == Token::Type::STRING_LITERAL )
    //     return "";

    std::string out( spaces, ' ' );

    out += get_symbol_type( symbol, groupName, index );
    out += " field" + std::to_string(index) + ";\n";

    return out;
}

std::string build_group_struct( const std::string& groupName, const Symbols& symbols  )
{
    std::string out = "struct " + groupName + "\n{\n";

    for( size_t i = 0; const auto& symbol : symbols ) {

        // out += build_field( symbol, 4, groupName, i++ );
        // if( symbol.token.type != Token::Type::STRING_LITERAL ) {
            std::string type = get_symbol_type(symbol, groupName, i);
            out += "    " + type + " field" + std::to_string(i) + ";\n";
            ++i;
        // }
    }

    out += "};\n\n";

    return out;
}

std::string build_forward_declarions( std::unordered_set<Node> forward_declarations )
{
    std::string out("// Forward declaration to break dependency cycles\n");

    for( const auto& node : forward_declarations )
        out += "struct " + capitalize(node) + ";\n"; 

    out += '\n';

    return out;
}

std::string display_graph( const Graph& graph )
{
    std::string out("graph = [\n");

    for( const std::pair<Node,Edges>& entry : graph ) {

        out += "    {\n\t\"name\": \"" + entry.first + "\",\n\t[\n";

        for( const Edge& edge : entry.second )
            out += "\t\t{ \"edge\": \"" + edge + "\" },\n";

        out += "\t],\n    },\n";
    }

    out += "]\n";

    return out;
}


void Generator::generateAstFiles( std::ostream& os, const Rules& rules)
{
    Graph graph = build_graph( rules);

    TarjanSCC tarjan(graph);
    tarjan.run();

    std::unordered_set<Node> forward_decls = tarjan.detect_cycles();
    std::vector<Node> sorted = tarjan.topologicalSort();

    RuleMap rule_map;
    for( const Rule& rule : rules )
        rule_map[rule.name] = rule;

    generateHeader( os );

    os << build_forward_declarions( forward_decls );

    for( const auto& node : sorted )
        generateAstClass( os, rule_map[node] );
}


Graph Generator::build_graph( const Rules& rules )
{
    Graph graph;

    auto add_dependency = [&]( const Rule& rule, const Symbol & symbol )
    {
        if( symbol.token.type == Token::Type::NONTERMINAL )
            graph[rule.name].insert( symbol.token.lexeme );
    };

    auto process_production = [&]( const Rule& rule, const Symbols& symbols )
    {
        for( const auto& symbol : symbols )
            add_dependency( rule, symbol );
    };

    auto process_rule = [&]( const Rule& rule )
    {
        for( const auto& production : rule.productions )
            process_production( rule, production );
    };

    std::for_each( rules.begin(), rules.end(), process_rule );

    return graph;
}

void Generator::generateHeader( std::ostream & os )
{
    os << "/*\n";
    os << " * THIS FILE IS AUTOMATICALLY GENERATED. DO NOT MODIFY.\n";
    os << " */\n";
    os << "\n";
    os << "#pragma once\n";
    os << "\n";
    os << "#include <memory>\n";
    os << "#include <optional>\n";
    os << "#include <string>\n";
    os << "#include <variant>\n";
    os << "#include <vector>\n";
    os << "\n\n";
}

void Generator::generateAstClass( std::ostream& os, const Rule& rule)
{
    os << "// AST node for rule: " << rule.name << "\n";

    for( auto production : rule.productions )
        generateGroupStructures( os, rule.name, production );

    std::string className = capitalize(rule.name);

    os << "struct " + className + "\n";
    os << "{\n";

    if( rule.productions.size() == 1 ) {
        generateSingleProduction(os, className, rule);
    } else {
        generateMultiProduction(os, className, rule);
        generateVariant(os, rule);
    }

    os << "};\n\n";
}

void Generator::generateGroupStructures( std::ostream & os, std::string base_name, const Symbols& symbols )
{
    for( size_t i = 0; const auto& symbol : symbols ) {

        const auto& symbol_group = symbol.symbol_group;

        if( symbol_group.empty() )
            continue;

        std::string new_base = sanitize_type_name( base_name + "Group" + std::to_string(i) );

        generateGroupStructures( os, new_base, symbol_group );

        os << build_group_struct( new_base, symbol_group );

        ++i;
    }
}

void Generator::generateSingleProduction( std::ostream & os, std::string &className, const Rule & rule )
{
    // Single-production rule — plain struct
    for( size_t i = 0; const auto& symbol : rule.productions[0] )
        // os << build_field( symbol, 4, className, i++ );

        os << "    " + get_symbol_type( symbol, className, 0 ) + " field" + std::to_string(i++) + ";\n";
}

void Generator::generateMultiProduction( std::ostream & os, std::string &className, const Rule & rule )
{
    // os << "    using Ptr = std::shared_ptr<" + className + ">;\n";

    for( size_t i = 0; i < rule.productions.size(); ++i) {

        os << "    struct Production" + std::to_string(i) + "\n";
        os << "    {\n";

        for( size_t j = 0; const auto& symbol : rule.productions[i] )
            // os << build_field( symbol, 8, className, i++ );
            os << "        " + get_symbol_type( symbol, className, 0 ) + " field" + std::to_string(j++) + ";\n";

        os << "    };\n";
    }
}

void Generator::generateVariant(std::ostream & os, const Rule & rule)
{
    os << "    using Value = std::variant<";

    bool first = true;
    for( size_t i = 0; i < rule.productions.size(); ++i ) {
        if( first )
            first = false;
        else
            os << ", ";

        os << "Production" + std::to_string(i);
    }
    os << ">;\n\n";

    os << "    Value value;\n";
}

void Generator::debugPrintDependencies( const Graph& graph ) const
{
    std::cerr << "=== Dependencies ===\n";

    for( const auto& [rule_name, deps] : graph ) {

        std::cerr << rule_name << " depends on: ";

        if( deps.empty() )
            std::cerr << "(none)";
        else {
            bool first = true;
            for( const auto& dep : deps )
            {
                if(first)
                    first = false;
                else
                    std::cerr << ", ";
                
                std::cerr << dep;
            }
        }

        std::cerr << "\n";
    }

    std::cerr << "====================\n";
}

