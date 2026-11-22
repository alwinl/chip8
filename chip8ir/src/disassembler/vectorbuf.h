/*
 * vectorbuf.h Copyright 2025 Alwin Leerling dna.leerling@gmail.com
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

#include <streambuf>
#include <vector>

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_vectorbuf : public std::basic_streambuf<CharT, Traits>
{
public:
    //@{
	/** Standard types */
	using char_type = std::basic_streambuf<CharT, Traits>::char_type;
	// using traits_type = std::basic_streambuf<char_type, traits>::traits_type;
	using int_type = Traits::int_type;
	using pos_type = Traits::pos_type;
	using off_type = Traits::off_type;
	//@}

	//@{
	/**
	 * Constructor(s). Copying or moving of stream buffers is not allowed
	 */
    basic_vectorbuf( std::vector<uint8_t>& vec ) : data(vec)
    {
        sync_pointers();
    }

	basic_vectorbuf( const basic_vectorbuf &rhs ) = delete;
	basic_vectorbuf( basic_vectorbuf &rhs ) = delete;

	~basic_vectorbuf() { };
	//@}

	//@{
	/**
	 * Needs to be public as it is used in basic_vectorstream
	 */
    void sync_pointers()
    {
        auto* base = reinterpret_cast<CharT*>(data.data());
        auto size  = static_cast<std::streamsize>(data.size());

        this->setg(base, base, base + size);
        this->setp(base + size, base + size); // start "at the end" for writing
    }
	//@}

protected:
	// Locales
	// virtual void imbue( const locale& loc )

	// Buffer management and positioning
	// virtual basic_streambuf<char_type,_Traits>* setbuf( char_type*, streamsize )
	// virtual pos_type seekoff( off_type off, ios_base::seekdir way, ios_base::openmode mode = ios_base::in | ios_base::out)
	// virtual pos_type seekpos( pos_type, ios_base::openmode mode = ios_base::in | ios_base::out) // not specialised
	// virtual int sync()

	// Input functions (get)
	// std::streamsize showmanyc() override;
	// std::streamsize xsgetn( char_type *s, std::streamsize n ) override;

	int_type underflow() override
    {
        return ( this->gptr() == this->egptr() ) ? Traits::eof() : Traits::to_int_type(*this->gptr());
    };

	// int_type uflow() override;
	// int_type pbackfail( int_type c ) override;

	// Output functions(put)
	std::streamsize xsputn( const char_type *s, std::streamsize n ) override
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(s);

        data.insert(data.end(), bytes, bytes + n);
        sync_pointers();
        this->pbump(static_cast<int>(data.size()));

        return n;
    };

	int_type overflow( int_type ch ) override
    {
        if (Traits::eq_int_type(ch, Traits::eof()))
            return Traits::not_eof(ch);

        CharT c = Traits::to_char_type(ch);
        auto offset = this->pptr() - reinterpret_cast<CharT*>(data.data());
        data.push_back(static_cast<uint8_t>(c));
        sync_pointers();
        this->pbump(static_cast<int>(offset + 1));
        return ch;
    };

private:
    std::vector<uint8_t>& data;

};