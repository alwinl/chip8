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

set( RESOURCE_BASE gchemul8 )

find_program( GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED )

set( GRESOURCE_C ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.gresource.c )
set( GRESOURCE_XML ${RESOURCE_BASE}.gresource.xml )
set( GRESOURCE_TARGET ${RESOURCE_BASE}-resource )

set( GRESOURCE_DEPEND
	gchemul8.ui
)

add_custom_command(
	OUTPUT ${GRESOURCE_C}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${GLIB_COMPILE_RESOURCES}
	ARGS
		--target=${GRESOURCE_C}
		--generate-source
		${GRESOURCE_XML}
	VERBATIM
	MAIN_DEPENDENCY ${GRESOURCE_XML}
	DEPENDS ${GRESOURCE_DEPEND}
)

set_source_files_properties( ${GRESOURCE_C} PROPERTIES GENERATED TRUE )
add_custom_target( ${GRESOURCE_TARGET} DEPENDS ${GRESOURCE_C} )

set( GRESOURCE_C "${GRESOURCE_C}" PARENT_SCOPE )
