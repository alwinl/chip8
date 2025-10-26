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
#include <variant>

#include "tokeniser.h"

struct Stmt;

struct Expr
{
    Token token;

    auto operator<=>(const Expr&) const = default;
};

struct DummyStmt
{
    DummyStmt() = default;
    DummyStmt(const DummyStmt& other) = default;
    DummyStmt& operator=(const DummyStmt& other) = default;
    DummyStmt(DummyStmt&&) noexcept = default;
    DummyStmt& operator=(DummyStmt&&) noexcept = default;
};

struct ExprStmt
{
    ExprStmt(const ExprStmt& other)
        : expr( other.expr ? std::make_unique<Expr>(*other.expr ) : nullptr )
    {}

    ExprStmt& operator=(const ExprStmt& other) {
        if (this == &other) return *this;
        expr = other.expr ? std::make_unique<Expr>(*other.expr ) : nullptr;
        return *this;
    }

    ExprStmt(ExprStmt&&) noexcept = default;
    ExprStmt& operator=(ExprStmt&&) noexcept = default;

    std::unique_ptr<Expr> expr;
};

struct IfStmt
{
    IfStmt(const IfStmt& other)
        : condition( other.condition ? std::make_unique<Expr>(*other.condition ) : nullptr )
        , then_branch(other.then_branch ? std::make_unique<Stmt>(*other.then_branch) : nullptr)
        , else_branch(other.else_branch ? std::make_unique<Stmt>(*other.else_branch) : nullptr)
    {}

    IfStmt& operator=(const IfStmt& other) {
        if (this == &other) return *this;
        condition = other.condition ? std::make_unique<Expr>(*other.condition ) : nullptr;
        then_branch = other.then_branch ? std::make_unique<Stmt>(*other.then_branch) : nullptr;
        else_branch = other.else_branch ? std::make_unique<Stmt>(*other.else_branch) : nullptr;
        return *this;
    }

    IfStmt(IfStmt&&) noexcept = default;
    IfStmt& operator=(IfStmt&&) noexcept = default;

    IfStmt( std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch)
    :
    condition( std::move(condition)),
    then_branch( std::move(then_branch)),
    else_branch( std::move(else_branch))
    {}

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::unique_ptr<Stmt> else_branch; // may be nullptr
};

struct WhileStmt
{
    WhileStmt(const WhileStmt& other)
        : condition( other.condition ? std::make_unique<Expr>(*other.condition ) : nullptr )
        , body(other.body ? std::make_unique<Stmt>(*other.body) : nullptr)
    {}

    WhileStmt& operator=(const WhileStmt& other) {
        if (this == &other) return *this;
        condition = other.condition ? std::make_unique<Expr>(*other.condition ) : nullptr;
        body = other.body ? std::make_unique<Stmt>(*other.body) : nullptr;
        return *this;
    }

    WhileStmt(WhileStmt&&) noexcept = default;
    WhileStmt& operator=(WhileStmt&&) noexcept = default;

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

struct ReturnStmt
{
    ReturnStmt(const ReturnStmt& other)
        : value( other.value ? std::make_unique<Expr>(*other.value ) : nullptr )
    {}

    ReturnStmt& operator=(const ReturnStmt& other) {
        if (this == &other) return *this;
        value = other.value ? std::make_unique<Expr>(*other.value ) : nullptr;
        return *this;
    }

    ReturnStmt(ReturnStmt&&) noexcept = default;
    ReturnStmt& operator=(ReturnStmt&&) noexcept = default;

    std::unique_ptr<Expr> value; // may be nullptr
};

struct DrawStmt
{
    DrawStmt(const DrawStmt& other)
        : sprite( other.sprite ),
          x( other.x ? std::make_unique<Expr>(*other.x ) : nullptr ),
          y( other.y ? std::make_unique<Expr>(*other.y ) : nullptr ),
          height( other.height ? std::make_unique<Expr>(*other.height ) : nullptr )
    {}

    DrawStmt& operator=(const DrawStmt& other) {
        if (this == &other) return *this;
        sprite = other.sprite;
        x = other.x ? std::make_unique<Expr>(*other.x ) : nullptr;
        y = other.y ? std::make_unique<Expr>(*other.y ) : nullptr;
        height = other.height ? std::make_unique<Expr>(*other.height ) : nullptr;
        return *this;
    }

    DrawStmt(DrawStmt&&) noexcept = default;
    DrawStmt& operator=(DrawStmt&&) noexcept = default;

    Token sprite;
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
    std::unique_ptr<Expr> height; // optional
};

struct GetKeyStmt
{
    Token variable;
};

struct Block
{
    Block() = default;

    Block(const Block& other)
    {
        statements.reserve(other.statements.size());
        for (const auto& stmt : other.statements)
            statements.push_back(std::make_unique<Stmt>(*stmt));
    }

    Block& operator=(const Block& other) {
        if (this == &other) return *this;

        statements.clear();
        statements.reserve(other.statements.size());
        for (const auto& stmt : other.statements)
            statements.push_back(std::make_unique<Stmt>(*stmt));

        return *this;
    }

    Block(Block&&) noexcept = default;
    Block& operator=(Block&&) noexcept = default;

    std::vector<std::unique_ptr<Stmt>> statements;
};

using StmtVariant = std::variant<
    DummyStmt,
    ExprStmt,
    IfStmt,
    WhileStmt,
    ReturnStmt,
    DrawStmt,
    GetKeyStmt,
    Block
>;

struct Stmt
{
    StmtVariant value;

    Stmt() = default;
    Stmt(const Stmt&) = default;
    Stmt(Stmt&&) noexcept = default;
    Stmt& operator=(const Stmt&) = default;
    Stmt& operator=(Stmt&&) noexcept = default;
    ~Stmt() = default;

    template<typename T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, Stmt>, int> = 0>
    Stmt(T&& v) : value(std::forward<T>(v)) {}

    auto operator<=>(const Stmt&) const = default;
};

struct Type
{
    enum class eTypeTypes { NIBBLE, BYTE, SNACK, NUMBER, BOOL, KEY, SPRITE };

    eTypeTypes type;
    int sprite_size;

    auto operator<=>(const Type&) const = default;
};

// Variable declaration: `var <id> : <type> ;`
struct VarDecl
{
    std::string identifier;
    Type type;

    auto operator<=>(const VarDecl&) const = default;
};

struct TypedIdentifier
{
    std::string identifier;
    Type type;

    auto operator<=>(const TypedIdentifier&) const = default;
};

using ParamList = std::vector<TypedIdentifier>;
using Vardecls = std::vector<VarDecl>;
using Stmts = std::vector<std::unique_ptr<Stmt>>;

// Function declaration: `func <id> ( <params>? ) { <stmt>* }`
struct FuncDecl
{
    FuncDecl(const FuncDecl& other) : identifier( other.identifier ), params( other.params), vars(other.vars)
    {
        body.reserve(other.body.size());
        for (const auto& stmt : other.body)
            body.push_back(std::make_unique<Stmt>(*stmt));
    }

    FuncDecl& operator=(const FuncDecl& other) {
        if (this == &other) return *this;

        identifier = other.identifier;
        params = other.params;
        vars = other.vars;

        body.clear();
        body.reserve(other.body.size());
        for (const auto& stmt : other.body)
            body.push_back(std::make_unique<Stmt>(*stmt));

        return *this;
    }

    FuncDecl(FuncDecl&&) noexcept = default;
    FuncDecl& operator=(FuncDecl&&) noexcept = default;

    FuncDecl(std::string identifier,
             ParamList params,
             Vardecls vars,
             Stmts body)
        : identifier(std::move(identifier))
        , params(std::move(params))
        , vars(std::move(vars))
        , body(std::move(body))
    {}

    std::string identifier;
    ParamList params;
    Vardecls vars;
    Stmts body;

    auto operator<=>(const FuncDecl&) const = default;
};

using Decl = std::variant<FuncDecl, VarDecl>;

struct Program
{
    std::vector<Decl> declarations;

    auto operator<=>(const Program&) const = default;
};

std::ostream& operator<<( std::ostream& os, const Program& program );

class Parser {
public:
    Parser( const Tokens& tokens ) : tokens(tokens ) {}

    Program AST();

protected:
    const Tokens& tokens;
    Tokens::const_iterator cursor;

    void forward_cursor();
    const Token& peek() { return (cursor != tokens.end()) ? *cursor : tokens.back(); }
    bool match(Token::Type type);
    Token consume(Token::Type type, const std::string& message);
    Token consume(Token::Type type, const std::string& lexeme, const std::string& message);

    Decl parse_decl();
    FuncDecl parse_func_decl();
    ParamList parse_param_list();
    TypedIdentifier parse_typed_identifier();
    VarDecl parse_var_decl();
    Type parse_type();

    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<Stmt> parse_if_stmt();
    std::unique_ptr<Stmt> parse_while_stmt();
    std::unique_ptr<Stmt> parse_draw_stmt();
    std::unique_ptr<Stmt> parse_return_stmt();
    std::unique_ptr<Stmt> parse_getkey_stmt();
    std::unique_ptr<Stmt> parse_block();

    std::unique_ptr<Expr> parse_expr();

};