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

add_executable ( chasm8 chasm8.cc )
add_executable ( chemul8 chemul8.cc
    menu.cc )
add_executable ( chicom8 chicom8.cc )
add_executable ( chidasm8 chidasm8.cc )

find_package(raylib REQUIRED)

target_link_libraries( chasm8 PRIVATE chip8::ir )
target_link_libraries( chemul8 PRIVATE chip8::ir imgui)
target_link_libraries( chicom8 PRIVATE chip8::ir )
target_link_libraries( chidasm8 PRIVATE chip8::ir )
