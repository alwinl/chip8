/*
 * Copyright 2022 Alwin Leerling <dna.leerling@gmail.com>
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

#ifndef PROGRAMPARSERTEST_H
#define PROGRAMPARSERTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


class ProgramParserTest : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( ProgramParserTest );
    CPPUNIT_TEST( TestRemoveSlashR );
    CPPUNIT_TEST( TestRemoveComments );
    CPPUNIT_TEST( TestSplit );
    CPPUNIT_TEST( TestIdentify );
    CPPUNIT_TEST( TestJP );
    CPPUNIT_TEST( TestClearScreen );
    CPPUNIT_TEST_SUITE_END();

private:
	void TestRemoveSlashR();
    void TestRemoveComments();
    void TestSplit();
    void TestIdentify();
    void TestClearScreen();
	void TestJP();
};

#endif // PROGRAMPARSERTEST_H
