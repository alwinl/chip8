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

find_package( PkgConfig REQUIRED )

pkg_check_modules( GTKmm REQUIRED gtkmm-4.0 )

add_executable(
	gchemul8

	gchemul8.cc
)

target_include_directories( gchemul8 PUBLIC ${GTKmm_INCLUDE_DIRS} )
target_link_libraries( gchemul8 PUBLIC ${GTKmm_LIBRARIES})

add_custom_command(
	TARGET gchemul8 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gchemul8.ui $<TARGET_FILE_DIR:gchemul8>
)