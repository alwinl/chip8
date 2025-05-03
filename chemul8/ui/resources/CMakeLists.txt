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

find_program(BLUEPRINT_COMPILER blueprint-compiler REQUIRED)

set(BLP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_BASE}.blp)
set(UI_FILE ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.ui)

add_custom_command(
    OUTPUT ${UI_FILE}
    COMMAND ${BLUEPRINT_COMPILER} compile --output ${UI_FILE} ${BLP_FILE}
    DEPENDS ${BLP_FILE}
    COMMENT "Compiling Blueprint UI"
)


find_program( GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED )

set( GRESOURCE_XML ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_BASE}.gresource.xml )
set( GRESOURCE_C ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_BASE}.gresource.c )

set( GRESOURCE_DEPEND
	${UI_FILE}
)

set( GRESOURCE_ARGS
	--target=${GRESOURCE_C}
	--generate-source
	--sourcedir=${CMAKE_CURRENT_SOURCE_DIR}
	--sourcedir=${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
	OUTPUT ${GRESOURCE_C}
	COMMAND ${GLIB_COMPILE_RESOURCES} ${GRESOURCE_ARGS} ${GRESOURCE_XML}
	DEPENDS ${GRESOURCE_XML} ${GRESOURCE_DEPEND}
    COMMENT "Generating GResources bundle"
)

set_source_files_properties( ${GRESOURCE_C} PROPERTIES GENERATED TRUE )
add_custom_target( ${RESOURCE_BASE}-resource DEPENDS ${GRESOURCE_C} )

set( GRESOURCE_C "${GRESOURCE_C}" PARENT_SCOPE )
