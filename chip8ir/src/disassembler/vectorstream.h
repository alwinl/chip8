/*
 * vectorstream.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <istream>
#include <vector>

#include "vectorbuf.h"

template< typename CharT, typename Traits = std::char_traits<CharT> >
class basic_ivectorstream : public std::basic_istream<CharT, Traits>
{
public:
	template<typename T>
	basic_ivectorstream( std::vector<T>& raw_bytes )  :  
		std::basic_istream<CharT, Traits>( &buffer ), buffer( raw_bytes)
	{
        this->unsetf( std::ios::skipws );
    }

private:
	basic_vectorbuf<CharT, Traits> buffer;
};

using  ivectorstream = basic_ivectorstream<char>;
using  wivectorstream = basic_ivectorstream<wchar_t>;

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_ovectorstream : public std::basic_ostream<CharT, Traits>
{
public:
    explicit basic_ovectorstream( std::vector<uint8_t>& target )
        : std::basic_ostream<CharT, Traits>(&buffer),
          buffer(target)
    {}

private:
    basic_vectorbuf<CharT, Traits> buffer;
};

using  ovectorstream = basic_ovectorstream<char>;
using  wovectorstream = basic_ovectorstream<wchar_t>;

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_vectorstream : public std::basic_iostream<CharT, Traits>
{
public:
    using vector_type = std::vector<uint8_t>;

    explicit basic_vectorstream( vector_type& data )
        : std::basic_iostream<CharT, Traits>( &buffer ), buffer(data), ref_data(data)
    {
        this->unsetf(std::ios::skipws);
    }

    basic_vectorstream()
        : std::basic_iostream<CharT, Traits>(&buffer), buffer(owned_data), ref_data(owned_data)
    {
        this->unsetf(std::ios::skipws);
    }

    const vector_type& vector() const { return ref_data; }
    vector_type& vector() { return ref_data; }

    void clear_vector()
    {
        ref_data.clear();
        buffer.sync_pointers();
        this->clear();
    }

private:
    vector_type owned_data;
    basic_vectorbuf<CharT, Traits> buffer;
    vector_type& ref_data;
};

using vectorstream  = basic_vectorstream<char>;
using wvectorstream = basic_vectorstream<wchar_t>;
