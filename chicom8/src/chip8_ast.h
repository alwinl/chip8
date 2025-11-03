/*
 * chip8_ast.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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
#include <memory>
#include <unordered_map>


template <typename T>
std::unique_ptr<T> clone_unique( const std::unique_ptr<T>& src )
{
    return src ? std::unique_ptr<T>(static_cast<T*>(src->clone().release())) : nullptr;
}

template <typename T>
std::vector<std::unique_ptr<T>> clone_vector(const std::vector<std::unique_ptr<T>>& src)
{
    std::vector<std::unique_ptr<T>> result;
    result.reserve(src.size());

    for (const auto& item : src)
        result.push_back(clone_unique(item));

    return result;
}

// Generic streaming for std::unique_ptr<T>
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::unique_ptr<T>& ptr)
{
    if (ptr)  // check for nullptr
        os << *ptr; // delegate to T's operator<<
    else
        os << "nullptr"; // optional: show null pointers explicitly
    return os;
}

template <typename T>
std::ostream& stream_unique_ptr_vector(std::ostream& os, const T& container, const std::string& sep = ", ")
{
    bool first = true;

    for( const auto& elem : container ) {

        if( !first)
            os << sep;
        else
            first = false;

        if( elem )
            os << *elem; // uses operator<< for T
        else
            os << "nullptr";
    }

    return os;
}

struct Expr
{
    virtual ~Expr() noexcept = default;
    virtual std::unique_ptr<Expr> clone() const = 0;

    virtual void print( std::ostream& os ) const = 0;
};

inline std::ostream& operator<<( std::ostream& os, const Expr& expr )
{
    expr.print( os );
    return os;
}

struct Number : Expr
{
    explicit Number( int value ) : value( value ) {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<Number>( value ); }

    void print( std::ostream& os ) const override { os << "Number(" << value << ")"; };

    int value;
};

inline std::ostream& operator<<( std::ostream& os, const Number& number )
{
    os << number.value;
    return os;
}

struct Bool : Expr
{
    explicit Bool( bool value ) : value( value ) {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<Bool>( value ); }

    void print( std::ostream& os ) const override { os << "Bool(" << value << ")"; };

    bool value;
};

inline std::ostream& operator<<( std::ostream& os, const Bool& boolean )
{
    os << (boolean.value ? "true" : "false");
    return os;
}

struct Identifier : Expr
{
    explicit Identifier( std::string value ) : value( value ) {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<Identifier>( value ); }

    void print( std::ostream& os ) const override { os << "Identifier(" << value << ")"; };

    bool operator==( Identifier& other )
        { return value == other.value; }

    std::string value;
};

inline std::ostream& operator<<( std::ostream& os, const Identifier& name )
{
    os << name.value;
    return os;
}

struct BcdExpr  : Expr
{
    BcdExpr( std::unique_ptr<Identifier> identifier ) : identifier( std::move(identifier) ) {}

    BcdExpr( const BcdExpr& other )
        : identifier( clone_unique(other.identifier) )
    {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<BcdExpr>( clone_unique(identifier) ); }

    void print( std::ostream& os ) const override { os << "bcd " << identifier->value; };

    std::unique_ptr<Identifier> identifier;
};

struct RndExpr : Expr
{
    RndExpr( std::unique_ptr<Expr> number_type ) : number_type( std::move(number_type) ) {}

    RndExpr(const RndExpr& other)
        : number_type( clone_unique(other.number_type) )
    {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<RndExpr>( clone_unique(number_type) ); }

    void print( std::ostream& os ) const override { os << "rnd " << number_type; };

    std::unique_ptr<Expr> number_type;
};

struct KeyDownExpr : Expr
{
    KeyDownExpr( std::unique_ptr<Identifier> identifier ) : identifier( std::move(identifier) ) {}

    KeyDownExpr(const KeyDownExpr& other)
        : identifier( clone_unique(other.identifier) )
    {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<KeyDownExpr>( clone_unique(identifier) ); }

    void print( std::ostream& os ) const override { os << "keydown " << identifier->value; };

    std::unique_ptr<Identifier> identifier;
};

struct GetKeyExpr : Expr
{
    GetKeyExpr( ) {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<GetKeyExpr>(); }

    void print( std::ostream& os ) const override { os << "getkey"; };
};

struct FuncCallExpr : Expr
{
    FuncCallExpr( std::unique_ptr<Identifier> name, std::vector<std::unique_ptr<Expr>> args ) : name(std::move(name)), args( std::move(args) ) {}

    FuncCallExpr( const FuncCallExpr& other )
        : name( clone_unique(other.name) ), args( clone_vector( other.args) )
    {}

    FuncCallExpr& operator=( const FuncCallExpr& other )
    {
        if( this == &other )
            return *this;

        name = clone_unique( other.name );
        args = clone_vector( other.args );

        return *this;
    }

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<FuncCallExpr>(clone_unique(name), clone_vector(args)); }

    void print( std::ostream& os ) const override
    {
        os << "func " << name << "(";
        stream_unique_ptr_vector( os, args );
        os << ")";
    };

    std::unique_ptr<Identifier> name;
    std::vector<std::unique_ptr<Expr>> args;
};

struct BracedExpr : Expr
{
    BracedExpr( std::unique_ptr<Expr> expr ) : expr( std::move(expr) ) {}

    BracedExpr( const BracedExpr& other )
        : expr( clone_unique(other.expr) )
    {}

    BracedExpr& operator=( const BracedExpr& other )
    {
        if( this == &other )
            return *this;

        expr = clone_unique( other.expr );
        return *this;
    }

    BracedExpr(BracedExpr&&) noexcept = default;
    BracedExpr& operator=(BracedExpr&&) noexcept = default;

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<BracedExpr>( clone_unique(expr) ); }

    void print( std::ostream& os ) const override { os << "( " << expr << " )"; };

    std::unique_ptr<Expr> expr;
};

struct UnaryExpr : Expr
{
    enum class Operator { NOT, NEGATIVE };

    UnaryExpr( std::unique_ptr<Expr> expr, UnaryExpr::Operator op ) : expr(std::move(expr)), op(op) {}

    UnaryExpr( const UnaryExpr& other )
        : expr( clone_unique(other.expr) ), op( other.op )
    {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<UnaryExpr>( clone_unique(expr), op ); }

    void print( std::ostream& os ) const override
    {
        os << "UnaryExpr(";

        switch( op ) {
        case UnaryExpr::Operator::NEGATIVE : os << '-'; break;
        case UnaryExpr::Operator::NOT : os << "!"; break;
        }

        os << expr << ")";
    }

    std::unique_ptr<Expr> expr;
    Operator op;
};

struct BinaryExpr : Expr
{
    enum class Operator { MULTIPLY, DIVIDE, ADD, SUBTRACT, GREATER_THAN, GREATER_EQ, LESS_THAN, LESS_EQ, EQUALS, NOT_EQ, LOGIC_AND, LOGIC_OR, ASSIGN };

    BinaryExpr( std::unique_ptr<Expr> lhs, BinaryExpr::Operator op, std::unique_ptr<Expr> rhs) : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}

    BinaryExpr( const BinaryExpr& other )
        : lhs( clone_unique(other.lhs) ), op( other.op ), rhs( clone_unique(other.rhs) )
    {}

    std::unique_ptr<Expr> clone() const override
        { return std::make_unique<BinaryExpr>( clone_unique(lhs), op, clone_unique(rhs) ); }

    void print( std::ostream& os ) const override
    {
        os << "BinaryExpr(" << lhs;

        switch( op ) {
        case BinaryExpr::Operator::MULTIPLY : os << " * "; break;
        case BinaryExpr::Operator::DIVIDE : os << " / "; break;
        case BinaryExpr::Operator::ADD : os << " + "; break;
        case BinaryExpr::Operator::SUBTRACT : os << " - "; break;
        case BinaryExpr::Operator::GREATER_THAN : os << " > "; break;
        case BinaryExpr::Operator::GREATER_EQ : os << " >= "; break;
        case BinaryExpr::Operator::LESS_THAN : os << " < "; break;
        case BinaryExpr::Operator::LESS_EQ : os << " <= "; break;
        case BinaryExpr::Operator::EQUALS : os << " == "; break;
        case BinaryExpr::Operator::NOT_EQ : os << " != "; break;
        case BinaryExpr::Operator::LOGIC_AND : os << " && "; break;
        case BinaryExpr::Operator::LOGIC_OR : os << " || "; break;
        case BinaryExpr::Operator::ASSIGN : os << " = "; break;
        }

        os << rhs << ")";
    }

    std::unique_ptr<Expr> lhs;
    Operator op;
    std::unique_ptr<Expr> rhs;
};




struct Stmt
{
    virtual ~Stmt() noexcept = default;
    virtual std::unique_ptr<Stmt> clone() const = 0;
    virtual void print( std::ostream& os ) const = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Stmt& stmt)
{
    stmt.print(os);
    return os;
}

struct ExprStmt : Stmt
{
    ExprStmt( std::unique_ptr<Expr> expr ) : expr( std::move(expr) ) {}

    ExprStmt( const ExprStmt& other ) : expr( clone_unique(other.expr) ) {}

    ExprStmt& operator=( const ExprStmt& other )
    {
        if( this == &other )
            return *this;

        expr = clone_unique( other.expr );

        return *this;
    }

    ExprStmt(ExprStmt&&) noexcept = default;
    ExprStmt& operator=(ExprStmt&&) noexcept = default;

    std::unique_ptr<Stmt> clone() const override
        { return std::make_unique<ExprStmt>(clone_unique(expr) ); }

    void print( std::ostream& os ) const override { os << expr << ";\n"; };

    std::unique_ptr<Expr> expr;
};

struct IfStmt : Stmt
{
    IfStmt( std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch )
        : condition( std::move(condition)), then_branch( std::move(then_branch)), else_branch( std::move(else_branch))
    {}

    IfStmt( const IfStmt& other )
        : condition( clone_unique(other.condition) )
        , then_branch( clone_unique(other.then_branch) )
        , else_branch( clone_unique(other.else_branch) )
    {}

    IfStmt& operator=(const IfStmt& other)
    {
        if( this == &other )
            return *this;

        condition = clone_unique(other.condition);
        then_branch = clone_unique(other.then_branch);
        else_branch = clone_unique(other.else_branch);

        return *this;
    }

    IfStmt(IfStmt&&) noexcept = default;
    IfStmt& operator=(IfStmt&&) noexcept = default;

    std::unique_ptr<Stmt> clone() const override
        { return std::make_unique<IfStmt>( clone_unique(condition), clone_unique(then_branch), clone_unique(else_branch) ); }

    void print( std::ostream& os ) const override
    {
        os << "if( " << condition << " )\n\t" << then_branch << "\n";

        if( else_branch )
            os << "else\n\t" << *else_branch << "\n";
    };
    
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::unique_ptr<Stmt> else_branch; // may be nullptr
};

struct WhileStmt : Stmt
{
    WhileStmt( std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body )
        : condition( std::move(condition)), body( std::move(body) )
    {}

    WhileStmt( const WhileStmt& other )
        : condition( clone_unique(other.condition) ), body( clone_unique(other.body) )
    {}

    WhileStmt& operator=( const WhileStmt& other )
    {
        if( this == &other )
            return *this;

        condition = clone_unique(other.condition);
        body = clone_unique(other.body);

        return *this;
    }

    WhileStmt(WhileStmt&&) noexcept = default;
    WhileStmt& operator=(WhileStmt&&) noexcept = default;

    std::unique_ptr<Stmt> clone() const override
        { return std::make_unique<WhileStmt>( clone_unique(condition), clone_unique(body) ); }

    void print( std::ostream& os ) const override
    {
        os << "while( " << condition << ")\n\t" << body << "\n";
    };
    
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

struct ReturnStmt : Stmt
{
    ReturnStmt( std::unique_ptr<Expr> value ) : value( std::move(value)) {}

    ReturnStmt( const ReturnStmt& other )
        : value( clone_unique(other.value) )
    {}

    ReturnStmt& operator=( const ReturnStmt& other )
    {
        if( this == &other )
            return *this;

        value = clone_unique(other.value);

        return *this;
    }

    ReturnStmt(ReturnStmt&&) noexcept = default;
    ReturnStmt& operator=(ReturnStmt&&) noexcept = default;

    std::unique_ptr<Stmt> clone() const override
        { return std::make_unique<ReturnStmt>( clone_unique(value) ); }

    void print( std::ostream& os ) const override
    {
        os << "return";
        if( value )
            os << " " << value;
        os << ";\n";
    };
    
    std::unique_ptr<Expr> value; // may be nullptr
};

struct DrawStmt : Stmt
{
    DrawStmt( std::unique_ptr<Identifier> sprite_id, std::unique_ptr<Expr> x, std::unique_ptr<Expr> y, std::unique_ptr<Expr> height )
        : sprite_id( std::move(sprite_id) ), x( std::move(x)), y( std::move(y)), height( std::move(height))
    {}

    DrawStmt( const DrawStmt& other )
        : sprite_id( clone_unique(other.sprite_id) ),
          x( clone_unique(other.x) ),
          y( clone_unique(other.y) ),
          height( clone_unique(other.height) )
    {}

    DrawStmt& operator=( const DrawStmt& other )
    {
        if( this == &other )
            return *this;

        sprite_id = clone_unique(other.sprite_id);
        x = clone_unique(other.x);
        y = clone_unique(other.y);
        height = clone_unique(other.height);

        return *this;
    }

    DrawStmt(DrawStmt&&) noexcept = default;
    DrawStmt& operator=(DrawStmt&&) noexcept = default;

    std::unique_ptr<Stmt> clone() const override
        { return std::make_unique<DrawStmt>( clone_unique(sprite_id), clone_unique(x), clone_unique(y), clone_unique(height) ); }

    void print( std::ostream& os ) const override
    {
        os << "draw " << x << ", " << y;
        if( height )
            os << ", " << height;
        os << ";\n";
    };

    std::unique_ptr<Identifier> sprite_id;
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
    std::unique_ptr<Expr> height; // optional
};

struct BlockStmt : Stmt
{
    BlockStmt() = default;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements))
    {}

    BlockStmt(const BlockStmt& other) : statements( clone_vector( other.statements) )
    { }

    BlockStmt& operator=(const BlockStmt& other)
    {
        if (this == &other)
            return *this;

        statements = clone_vector( other.statements );

        return *this;
    }

    BlockStmt(BlockStmt&&) noexcept = default;
    BlockStmt& operator=(BlockStmt&&) noexcept = default;

    std::unique_ptr<Stmt> clone() const override
        { return std::make_unique<BlockStmt>( clone_vector(statements) ); }

    void print( std::ostream& os ) const override
    {
        os << "{\n";
        stream_unique_ptr_vector( os, statements, "\t" );
        // for( auto const& statement : statements )
        //     os << "\t" << statement;
        os << "}\n";
    };
    
    std::vector<std::unique_ptr<Stmt>> statements;
};





struct Decl
{
    virtual ~Decl() noexcept = default;
    virtual std::unique_ptr<Decl> clone() const = 0;
    virtual void print( std::ostream& os ) const = 0;
};

inline std::ostream& operator<<( std::ostream& os, const Decl& decl )
{
    decl.print( os );
    return os;
}

struct VarDecl : Decl
{
    enum class eTypes { NIBBLE, BYTE, SNACK, NUMBER, BOOL, KEY, SPRITE };

    inline static std::unordered_map<VarDecl::eTypes, std::string> type_to_string = {
        { VarDecl::eTypes::NIBBLE, "nibble" },
        { VarDecl::eTypes::BYTE, "byte" },
        { VarDecl::eTypes::SNACK, "snack" },
        { VarDecl::eTypes::NUMBER, "number" },
        { VarDecl::eTypes::BOOL, "bool" },
        { VarDecl::eTypes::KEY, "key" },
        { VarDecl::eTypes::SPRITE, "sprite" },
    };


    VarDecl( std::unique_ptr<Identifier> identifier, eTypes type, std::unique_ptr<Expr> byte_size = nullptr )
        : identifier( std::move(identifier)), type(type), byte_size( std::move(byte_size) ) {}

    VarDecl(const VarDecl& other) : identifier( clone_unique( other.identifier ) ), 
        type( other.type ), byte_size( clone_unique(other.byte_size) )
    {}

    VarDecl& operator=(const VarDecl& other)
    {
        if (this == &other)
            return *this;

        identifier = clone_unique( other.identifier );
        type = other.type;
        byte_size = clone_unique( other.byte_size );

        return *this;
    }

    std::unique_ptr<Decl> clone() const override
        { return std::make_unique<VarDecl>( clone_unique(identifier), type, clone_unique( byte_size) ); };

    void print( std::ostream& os ) const override
    {
        os << "var " << identifier << ": " << type_to_string.at( type);

        if( type == eTypes::SPRITE && byte_size )
            os << " " << byte_size;
    }

    std::unique_ptr<Identifier> identifier;
    eTypes type;
    std::unique_ptr<Expr> byte_size;
};

inline std::ostream& operator<<( std::ostream& os, const VarDecl& decl )
{
    os << *decl.identifier << ": " << VarDecl::type_to_string.at( decl.type);

    if( decl.type == VarDecl::eTypes::SPRITE && decl.byte_size )
        os << " " << decl.byte_size;

    return os;
}

struct FuncDecl : Decl
{
    FuncDecl( std::unique_ptr<Identifier> identifier, std::vector<std::unique_ptr<VarDecl>> params, std::vector<std::unique_ptr<VarDecl>> vars, std::vector<std::unique_ptr<Stmt>> body )
        : identifier(std::move(identifier)), params(std::move(params)), vars(std::move(vars)), body(std::move(body))
    {}

    FuncDecl(const FuncDecl& other) : identifier( clone_unique( other.identifier ) ), 
        params( clone_vector(other.params) ), vars( clone_vector(other.vars) ), body( clone_vector(other.body) )
    {}

    FuncDecl& operator=(const FuncDecl& other)
    {
        if (this == &other)
            return *this;

        identifier = clone_unique( other.identifier );
        params = clone_vector(other.params);
        vars = clone_vector(other.vars);
        body = clone_vector(other.body);

        return *this;
    }

    FuncDecl(FuncDecl&&) noexcept = default;
    FuncDecl& operator=(FuncDecl&&) noexcept = default;

    std::unique_ptr<Decl> clone() const override
        { return std::make_unique<FuncDecl>( clone_unique(identifier), clone_vector(params), clone_vector(vars), clone_vector(body) ); };

    void print( std::ostream& os ) const override
    {
        os << "func " << identifier << "(";
        stream_unique_ptr_vector( os, params, ", " );
        os << ")\n";
        
        os << "\t\t{\n";
        os << "\t\t\t";
        stream_unique_ptr_vector( os, vars, ", " );
        os << "\n";

        stream_unique_ptr_vector( os, body, ", " );
        os << "\n\t\t}\n\t)";
    }

    std::unique_ptr<Identifier> identifier;
    std::vector<std::unique_ptr<VarDecl>> params;
    std::vector<std::unique_ptr<VarDecl>> vars;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct Program
{
    std::vector<std::unique_ptr<Decl>> declarations;
};

std::ostream& operator<<( std::ostream& os, const Program& program );

bool operator==( const Program& a, const Program& b );
