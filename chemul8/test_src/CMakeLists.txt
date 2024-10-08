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

find_package( cppunit REQUIRED )

add_library( chemul8_test_setup INTERFACE )
target_compile_definitions( chemul8_test_setup INTERFACE ${CPPUNIT_CFLAGS_OTHER} )
target_include_directories( chemul8_test_setup INTERFACE ${CPPUNIT_INCLUDE_DIRS} )
target_link_directories( chemul8_test_setup INTERFACE ${CPPUNIT_LIBRARY_DIRS} )
target_link_libraries( chemul8_test_setup INTERFACE ${CPPUNIT_LIBRARIES} )

add_executable(
	chemul8_tests

	test_runner.cc
)

target_link_libraries( chemul8_tests chemul8_test_setup )
target_include_directories( chemul8_tests PUBLIC ../src )


# add_test( FilenameExtractorTest::ValidCreation chemul8_tests FilenameExtractorTest::ValidCreation )
