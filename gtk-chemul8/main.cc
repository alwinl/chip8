/*
 * [program] Copyright 2014 Alwin Leerling <alwin@jambo>
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

#include "appengine.h"

#ifdef TESTING
#include "logictest/testapplication.h"
#define AppClass TestApplication
#else
#include "guisrc/guiapplication.h"
#define AppClass GUIApplication
#endif

int main( int argc, char *argv[] )
{
    AppEngine engine;

    return AppClass( &engine ).run( argc, argv );
}
