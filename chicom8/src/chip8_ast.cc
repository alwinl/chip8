/*
 * chip8_ast.cc Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include "chip8_ast.h"

std::ostream& operator<<( std::ostream& os, const Program& program )
{
    os << "Program: [\n";

    bool first = true;
    for( const auto& declaration : program.declarations ) {
        if( first )
            first = false;
        else
            os << ",\n";

        os << *declaration;
    }

    os << "\n]";

    return os;
}

bool equals(const Expr& a, const Expr& b);
bool equals(const Stmt& a, const Stmt& b);
bool equals(const Decl& a, const Decl& b);

template <typename T>
inline bool equals(const std::unique_ptr<T>& a, const std::unique_ptr<T>& b)
    requires (std::is_base_of_v<Expr, T> || std::is_base_of_v<Stmt, T> || std::is_base_of_v<Decl, T>)
{
    if (!a && !b) return true;
    if (!a || !b) return false;
    return equals(static_cast<const T&>(*a), static_cast<const T&>(*b));
}

template <typename T>
bool equals_vector(const std::vector<std::unique_ptr<T>>& a,
                   const std::vector<std::unique_ptr<T>>& b)
{
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (!equals(*a[i], *b[i])) return false;
    return true;
}

bool equals(const Expr& a, const Expr& b)
{
    if( typeid(a) != typeid(b) )
        return false;

    if( auto na = dynamic_cast<const Number*>(&a) )
        return na->value == dynamic_cast<const Number*>(&b)->value;

    if( auto ba = dynamic_cast<const Bool*>(&a) )
        return ba->value == dynamic_cast<const Bool*>(&b)->value;

    if( auto ia = dynamic_cast<const Identifier*>(&a) )
        return ia->value == dynamic_cast<const Identifier*>(&b)->value;

    if( auto ba = dynamic_cast<const BcdExpr*>(&a) ) {
        auto bb = dynamic_cast<const BcdExpr*>(&b);
        return equals( ba->identifier, bb->identifier );
    }

    if( auto ra = dynamic_cast<const RndExpr*>(&a) ) {
        auto rb = dynamic_cast<const RndExpr*>(&b);
        return equals( ra->number_type, rb->number_type );
    }

    if( auto ka = dynamic_cast<const KeyDownExpr*>(&a) ) {
        auto kb = dynamic_cast<const KeyDownExpr*>(&b);
        return equals( ka->identifier, kb->identifier );
    }

    if( dynamic_cast<const GetKeyExpr*>(&a) )
        return true; // stateless

    if( auto fa = dynamic_cast<const FuncCallExpr*>(&a) ) {
        auto fb = dynamic_cast<const FuncCallExpr*>(&b);

        if( !equals( fa->name, fb->name) )
            return false;

        return equals_vector(fa->args, fb->args);
    }

    if( auto ua = dynamic_cast<const UnaryExpr*>(&a) )
    {
        auto ub = dynamic_cast<const UnaryExpr*>(&b);
        return ua->op == ub->op && equals( ua->expr, ub->expr );
    }

    if( auto ba = dynamic_cast<const BinaryExpr*>(&a) )
    {
        auto bb = dynamic_cast<const BinaryExpr*>(&b);
        return ba->op == bb->op &&
            equals(ba->lhs, bb->lhs) &&
            equals(ba->rhs, bb->rhs);
    }

    if( auto br = dynamic_cast<const BracedExpr*>(&a) )
    {
        auto brb = dynamic_cast<const BracedExpr*>(&b);
        return equals(br->expr, brb->expr);
    }

    return false;
}

bool equals(const Stmt& a, const Stmt& b)
{
    if( typeid(a) != typeid(b) )
        return false;

    if( auto sa = dynamic_cast<const ExprStmt*>(&a) )
    {
        auto sb = dynamic_cast<const ExprStmt*>(&b);

        return equals(sa->expr, sb->expr);
    }

    if( auto ia = dynamic_cast<const IfStmt*>(&a) )
    {
        auto ib = dynamic_cast<const IfStmt*>(&b);

        bool cond = equals( ia->condition, ib->condition );
        bool then_eq = equals( ia->then_branch, ib->then_branch );
        bool else_eq = equals( ia->else_branch, ib->else_branch );
            
        return cond && then_eq && else_eq;
    }

    if( auto wa = dynamic_cast<const WhileStmt*>(&a) )
    {
        auto wb = dynamic_cast<const WhileStmt*>(&b);

        bool cond = equals(wa->condition, wb->condition);
        bool body_eq = equals( wa->body, wb->body );

        return cond && body_eq;
    }

    if( auto da = dynamic_cast<const DrawStmt*>(&a) )
    {
        auto db = dynamic_cast<const DrawStmt*>(&b);

        if( ! equals(da->sprite_id, db->sprite_id) ) return false;
        if( ! equals(da->x, db->x) ) return false;
        if( ! equals(da->y, db->y) ) return false;

        return equals(da->height, db->height);
    }

    if( auto ra = dynamic_cast<const ReturnStmt*>(&a) )
    {
        auto rb = dynamic_cast<const ReturnStmt*>(&b);

        return equals(ra->value, rb->value);
    }

    if( auto ba = dynamic_cast<const BlockStmt*>(&a) )
    {
        auto bb = dynamic_cast<const BlockStmt*>(&b);

        if( !equals_vector(ba->statements, bb->statements) )
            return false;

        return true;
    }

    return false;
}

bool equals(const Decl& a, const Decl& b)
{
    if( typeid(a) != typeid(b) )
        return false;

    if( auto va = dynamic_cast<const VarDecl*>(&a) )
    {
        auto vb = dynamic_cast<const VarDecl*>(&b);
        if( !equals( va->identifier, vb->identifier ) )
            return false;
        if( va->type != vb->type )
            return false;

        return equals(va->byte_size, vb->byte_size);
    }

    if( auto fa = dynamic_cast<const FuncDecl*>(&a) )
    {
        auto fb = dynamic_cast<const FuncDecl*>(&b);
        if( !equals( fa->identifier, fb->identifier) )
            return false;

        if( !equals_vector(fa->params, fb->params) )
            return false;

        if( !equals_vector(fa->vars, fb->vars) )
            return false;

        if( !equals_vector(fa->body, fb->body) )
            return false;

        return true;
    }

    return false; // unknown Decl type
}

bool operator==(const Program& a, const Program& b)
{
    if( a.declarations.size() != b.declarations.size() )
        return false;

    for (size_t i = 0; i < a.declarations.size(); ++i)
    {
        if( !equals(a.declarations[i], b.declarations[i]) )
            return false;
    }

    return true;
}
