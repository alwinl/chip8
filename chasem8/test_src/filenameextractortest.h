/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
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
 *
 *
 */

#ifndef FILENAMEEXTRACTORTEST_H
#define FILENAMEEXTRACTORTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


class FilenameExtractorTest : public CppUnit::TestFixture
{
public:
    FilenameExtractorTest() {};
    virtual ~FilenameExtractorTest() {};

    CPPUNIT_TEST_SUITE( FilenameExtractorTest );
    CPPUNIT_TEST( ValidCreation );
    CPPUNIT_TEST( InvalidCreation );
    CPPUNIT_TEST( OldStyleArgsCreation );
    CPPUNIT_TEST( GetSourceTest );
    CPPUNIT_TEST( GetBinaryFromArgTest );
    CPPUNIT_TEST( GetBinaryFromSourceTest );
    CPPUNIT_TEST( GetListingFromArgTest );
    CPPUNIT_TEST( GetListingFromSourceTest );
    CPPUNIT_TEST( GetAllFromOneArgument );
    CPPUNIT_TEST( GetAllFromTwoArguments );
    CPPUNIT_TEST( GetAllFromThreeArguments );
    CPPUNIT_TEST_SUITE_END();

private:
    void ValidCreation();
    void InvalidCreation();
    void OldStyleArgsCreation();

    void GetSourceTest();
    void GetBinaryFromArgTest();
    void GetBinaryFromSourceTest();
    void GetListingFromArgTest();
    void GetListingFromSourceTest();

    void GetAllFromOneArgument();
    void GetAllFromTwoArguments();
    void GetAllFromThreeArguments();
};

#endif // FILENAMEEXTRACTORTEST_H
