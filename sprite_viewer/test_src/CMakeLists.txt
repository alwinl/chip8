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

add_library( sprite_viewer_test_setup INTERFACE )
target_compile_definitions( sprite_viewer_test_setup INTERFACE ${CPPUNIT_CFLAGS_OTHER} )
target_include_directories( sprite_viewer_test_setup INTERFACE ${CPPUNIT_INCLUDE_DIRS} )
target_link_directories( sprite_viewer_test_setup INTERFACE ${CPPUNIT_LIBRARY_DIRS} )
target_link_libraries( sprite_viewer_test_setup INTERFACE ${CPPUNIT_LIBRARIES} )

add_executable(
	sprite_viewer_tests

	test_runner.cc
)

target_link_libraries( sprite_viewer_tests sprite_viewer_test_setup )
target_include_directories( sprite_viewer_tests PUBLIC ../src )

