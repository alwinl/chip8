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

add_subdirectory(resources)

find_package(PkgConfig REQUIRED)
pkg_check_modules( GTKMM REQUIRED gtkmm-4.0)

SET( LIB_SETUP gchemul8_setup )

# define a setup with all the crud that needs to be added to the gcc call
add_library( ${LIB_SETUP} INTERFACE )
target_include_directories( ${LIB_SETUP} INTERFACE ${GTKMM_INCLUDE_DIRS} )
target_compile_options( ${LIB_SETUP} INTERFACE ${GTKMM_CFLAGS_OTHER} )
target_link_directories( ${LIB_SETUP} INTERFACE ${GTKMM_LIBRARY_DIRS} )
target_link_libraries( ${LIB_SETUP}  INTERFACE ${GTKMM_LIBRARIES} )

set_source_files_properties( ${GRESOURCE_C} PROPERTIES GENERATED TRUE )

add_executable(
	gchemul8

	gchemul8.cc

	${GRESOURCE_C}
)

add_dependencies( gchemul8 gchemul8-resource )

target_link_libraries( gchemul8 ${LIB_SETUP} )
# chessparser_logic )

