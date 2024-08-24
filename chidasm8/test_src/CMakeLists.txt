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

set(CMAKE_CXX_STANDARD 20)

find_package( cppunit REQUIRED )

add_library( chidasm8_test_setup INTERFACE )
# target_compile_definitions( test_setup INTERFACE "TESTING")
target_compile_definitions( chidasm8_test_setup INTERFACE ${CPPUNIT_CFLAGS_OTHER} )
target_include_directories( chidasm8_test_setup INTERFACE ${CPPUNIT_INCLUDE_DIRS} )
target_link_directories( chidasm8_test_setup INTERFACE ${CPPUNIT_LIBRARY_DIRS} )
target_link_libraries( chidasm8_test_setup INTERFACE ${CPPUNIT_LIBRARIES} )

add_executable(
	chidasm8_tests

	test_runner.cc
)

target_link_libraries( chidasm8_tests chidasm8_test_setup )
target_include_directories( chidasm8_tests PUBLIC ../src )


# add_test( FilenameExtractorTest::ValidCreation chemul8_tests FilenameExtractorTest::ValidCreation )
