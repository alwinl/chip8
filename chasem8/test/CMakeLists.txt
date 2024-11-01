#
# CMakeLists.txt Copyright 2024 Alwin Leerling dna.leerling@gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#

find_package(PkgConfig REQUIRED)
pkg_check_modules( CPPUNIT REQUIRED cppunit )

add_executable(
	chasem8_tests

	test_runner.cc
	filenameextractortest.cc
	programparsertest.cc
	testprogram.cc
)

target_link_libraries( chasem8_tests ${CPPUNIT_LIBRARIES} chasem8_logic )

add_test( FilenameExtractorTest::ValidCreation chasem8_tests FilenameExtractorTest::ValidCreation )
add_test( FilenameExtractorTest::InvalidCreation chasem8_tests FilenameExtractorTest::InvalidCreation )
add_test( FilenameExtractorTest::OldStyleArgsCreation chasem8_tests FilenameExtractorTest::OldStyleArgsCreation )
add_test( FilenameExtractorTest::GetSourceTest chasem8_tests FilenameExtractorTest::GetSourceTest )
add_test( FilenameExtractorTest::GetBinaryFromArgTest chasem8_tests FilenameExtractorTest::GetBinaryFromArgTest )
add_test( FilenameExtractorTest::GetBinaryFromSourceTest chasem8_tests FilenameExtractorTest::GetBinaryFromSourceTest )
add_test( FilenameExtractorTest::GetListingFromArgTest chasem8_tests FilenameExtractorTest::GetListingFromArgTest )
add_test( FilenameExtractorTest::GetListingFromSourceTest chasem8_tests FilenameExtractorTest::GetListingFromSourceTest )
add_test( FilenameExtractorTest::GetAllFromOneArgument chasem8_tests FilenameExtractorTest::GetAllFromOneArgument )
add_test( FilenameExtractorTest::GetAllFromTwoArguments chasem8_tests FilenameExtractorTest::GetAllFromTwoArguments )
add_test( FilenameExtractorTest::GetAllFromThreeArguments chasem8_tests FilenameExtractorTest::GetAllFromThreeArguments )

add_test( ProgramParserTest::TestRemoveSlashR chasem8_tests ProgramParserTest::TestRemoveSlashR )
add_test( ProgramParserTest::TestRemoveComments chasem8_tests ProgramParserTest::TestRemoveComments )
add_test( ProgramParserTest::TestSplit chasem8_tests ProgramParserTest::TestSplit )
add_test( ProgramParserTest::TestIdentify chasem8_tests ProgramParserTest::TestIdentify )
add_test( ProgramParserTest::TestJP chasem8_tests ProgramParserTest::TestJP )
add_test( ProgramParserTest::TestClearScreen chasem8_tests ProgramParserTest::TestClearScreen )

add_test( TestProgram::a_test chasem8_tests TestProgram::a_test )
