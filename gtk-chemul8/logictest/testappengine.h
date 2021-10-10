/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#ifndef TESTAPPENGINE_H
#define TESTAPPENGINE_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

/** \brief
 */
class TestAppEngine : public CppUnit::TestFixture
{
public:
    TestAppEngine() {};
    virtual ~TestAppEngine() {};

	CPPUNIT_TEST_SUITE( TestAppEngine );

	CPPUNIT_TEST( a_test );

	CPPUNIT_TEST_SUITE_END();

private:
	void a_test();
};

#endif // TESTAPPENGINE_H
