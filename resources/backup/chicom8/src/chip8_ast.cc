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

#include "ir_builder.h"

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
    if (!a && !b)
		return true;

    if (!a || !b)
		return false;

    return equals(static_cast<const T&>(*a), static_cast<const T&>(*b));
}

template <typename T>
bool equals_vector(const std::vector<std::unique_ptr<T>>& a, const std::vector<std::unique_ptr<T>>& b)
{
    if (a.size() != b.size())
		return false;

    for (size_t i = 0; i < a.size(); ++i)
        if (!equals(*a[i], *b[i]))
			return false;

    return true;
}

bool equals(const Expr& a, const Expr& b)
{
    if( typeid(a) != typeid(b) )
        return false;

    if( const auto *na = dynamic_cast<const Number*>(&a) ) {
		const auto *nb = dynamic_cast<const Number*>(&b);

        return na->value == nb->value;
	}

    if( const auto *ba = dynamic_cast<const Bool*>(&a) ) {
		const auto *bb = dynamic_cast<const Bool*>(&b);

        return ba->value == bb->value;
	}

    if( const auto *ia = dynamic_cast<const Identifier*>(&a) ) {
		const auto *ib = dynamic_cast<const Identifier*>(&b);

        return ia->value == ib->value;
	}

    if( const auto *ba = dynamic_cast<const BcdExpr*>(&a) ) {
        const auto *bb = dynamic_cast<const BcdExpr*>(&b);

        return equals( ba->identifier, bb->identifier );
    }

    if( const auto *ra = dynamic_cast<const RndExpr*>(&a) ) {
        const auto *rb = dynamic_cast<const RndExpr*>(&b);

        return equals( ra->number_type, rb->number_type );
    }

    if( const auto *ka = dynamic_cast<const KeyDownExpr*>(&a) ) {
        const auto *kb = dynamic_cast<const KeyDownExpr*>(&b);

        return equals( ka->identifier, kb->identifier );
    }

    if( dynamic_cast<const GetKeyExpr*>(&a) != nullptr )
        return true; // stateless

    if( const auto *fa = dynamic_cast<const FuncCallExpr*>(&a) ) {
        const auto *fb = dynamic_cast<const FuncCallExpr*>(&b);

		return (
			equals( fa->name, fb->name) &&
			equals_vector(fa->args, fb->args)
		);
    }

    if( const auto *ua = dynamic_cast<const UnaryExpr*>(&a) ) {
        const auto *ub = dynamic_cast<const UnaryExpr*>(&b);

        return (
			( ua->op == ub->op ) &&
			equals( ua->expr, ub->expr )
		);
    }

    if( const auto *ba = dynamic_cast<const BinaryExpr*>(&a) ) {
        const auto *bb = dynamic_cast<const BinaryExpr*>(&b);

        return (
			( ba->op == bb->op ) &&
            equals( ba->lhs, bb->lhs ) &&
            equals( ba->rhs, bb->rhs )
		);
    }

    if( const auto *br = dynamic_cast<const BracedExpr*>(&a) ) {
        const auto *brb = dynamic_cast<const BracedExpr*>(&b);

        return equals( br->expr, brb->expr );
    }

    return false;
}

bool equals(const Stmt& a, const Stmt& b)
{
    if( typeid(a) != typeid(b) )
        return false;

    if( const auto *sa = dynamic_cast<const ExprStmt*>(&a) ) {
        const auto *sb = dynamic_cast<const ExprStmt*>(&b);

        return equals( sa->expr, sb->expr );
    }

    if( const auto *ia = dynamic_cast<const IfStmt*>(&a) ) {
        const auto *ib = dynamic_cast<const IfStmt*>(&b);

		return (
			equals( ia->condition, ib->condition ) &&
			equals( ia->then_branch, ib->then_branch ) &&
			equals( ia->else_branch, ib->else_branch )
		);
    }

    if( const auto *wa = dynamic_cast<const WhileStmt*>(&a) ) {
        const auto *wb = dynamic_cast<const WhileStmt*>(&b);

		return (
			equals( wa->condition, wb->condition ) &&
			equals( wa->body, wb->body )
		);
    }

    if( const auto *da = dynamic_cast<const DrawStmt*>(&a) ) {
        const auto *db = dynamic_cast<const DrawStmt*>(&b);

		return (
			equals( da->sprite_id, db->sprite_id ) &&
			equals( da->x, db->x ) &&
			equals( da->y, db->y ) &&
			equals( da->height, db->height )
		);
    }

    if( const auto *ra = dynamic_cast<const ReturnStmt*>(&a) ) {
        const auto *rb = dynamic_cast<const ReturnStmt*>(&b);

        return equals( ra->value, rb->value );
    }

    if( const auto *ba = dynamic_cast<const BlockStmt*>(&a) ) {
        const auto *bb = dynamic_cast<const BlockStmt*>(&b);

        return equals_vector( ba->statements, bb->statements );
    }

    return false;
}

bool equals(const Decl& a, const Decl& b)
{
    if( typeid(a) != typeid(b) )
        return false;

    if( const auto *va = dynamic_cast<const VarDecl*>(&a) ) {
        const auto *vb = dynamic_cast<const VarDecl*>(&b);

		return (
			( va->type == vb->type ) &&
			equals( va->identifier, vb->identifier ) &&
			equals( va->byte_size, vb->byte_size )
		);
    }

    if( const auto *fa = dynamic_cast<const FuncDecl*>(&a) ) {
        const auto *fb = dynamic_cast<const FuncDecl*>(&b);

		return (
			equals( fa->identifier, fb->identifier ) &&
			equals_vector( fa->params, fb->params ) &&
			equals_vector( fa->vars, fb->vars ) &&
			equals_vector( fa->body, fb->body )
		);
    }

    return false; // unknown Decl type
}

bool operator==(const Program& a, const Program& b)
{
    if( a.declarations.size() != b.declarations.size() )
        return false;

    for( size_t i = 0; i < a.declarations.size(); ++i ) {

        if( !equals(a.declarations[i], b.declarations[i]) )
            return false;
    }

    return true;
}

bool ExprStmt::emit( IRBuilder& ir ) const
{
	expr->emit( ir );

	return true;
}

bool IfStmt::emit( IRBuilder &ir ) const
{
	auto end_label = ir.new_label( "endif" );
	auto else_label = ir.new_label( "else" );

	ir.emit_jump_if_false( condition.get(), else_label );

	then_branch->emit( ir );

	ir.emit_jump( end_label );

	ir.emit_label( else_label );

	if( else_branch )
		else_branch->emit( ir );

	ir.emit_label( end_label );

	return true;
}

bool WhileStmt::emit( IRBuilder &ir ) const
{
	std::string while_label = ir.new_label("while");
	std::string end_label = ir.new_label("endwhile");

	ir.emit_label( while_label );
	ir.emit_jump_if_false( condition.get(), end_label );

    body->emit( ir );

	ir.emit_jump( while_label );
	ir.emit_label( end_label );

	return true;
}

bool ReturnStmt::emit( IRBuilder& ir ) const
{
	ir.emit_return();
	return true;
}

bool DrawStmt::emit( IRBuilder &ir ) const
{
    // std::unique_ptr<Identifier> sprite_id;
    // std::unique_ptr<Expr> x;
    // std::unique_ptr<Expr> y;
    // std::unique_ptr<Expr> height; // optional

	x->emit( ir );
	y->emit( ir );
	if (height) height->emit(ir);

	ir.emit_draw(sprite_id->value, x.get(), y.get(), height.get());

	return true;
}

bool BlockStmt::emit( IRBuilder &ir ) const
{
	for( const auto& stmt : statements ) {
		if( !stmt->emit( ir ) )
			return false;
	}

	return true;
}

bool FuncDecl::emit( IRBuilder &ir ) const
{
    // std::unique_ptr<Identifier> identifier;
    // std::vector<std::unique_ptr<VarDecl>> params;
    // std::vector<std::unique_ptr<VarDecl>> vars;
    // std::vector<std::unique_ptr<Stmt>> body;

	std::string func_label = ir.new_label( identifier.get()->value );
	std::string func_end = ir.new_label( identifier.get()->value + "_end" );

	ir.emit_label( func_label );

	for (auto& param : params)
        param->emit(ir); // can be a no-op in IR

    for (auto& var : vars)
        var->emit(ir);

	for( const auto& stmt : body )
		stmt->emit( ir );

	ir.emit_label( func_end );
	ir.emit_return();

	return true;
}

bool VarDecl::emit( IRBuilder &ir ) const
{
    // std::unique_ptr<Identifier> identifier;
    // eTypes type;
    // std::unique_ptr<Expr> byte_size;

	ir.emit_comment( "var " + identifier->value + ": " + type_to_string.at(type) );

	return true;
}

bool BinaryExpr::emit( IRBuilder &ir ) const
{
    // std::unique_ptr<Expr> lhs;
    // Operator op;
    // std::unique_ptr<Expr> rhs;

	// ir.emit_( )
	return true;
}
