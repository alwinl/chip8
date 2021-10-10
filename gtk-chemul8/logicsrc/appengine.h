/*
 * Copyright 2014 Alwin Leerling <alwin@jambo>
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

#ifndef APPENGINE_H
#define APPENGINE_H

//-----------------------------------------------------------------------------
// The AppModel class is the M in MVC. You can also regard
// this class as the Abstract Data Type of the application.
//
// Just like the Gtk::ComboBox or Gtk::TreeView classes have
// a Gtk::TreeModel class representing a datstructure containing
// data to be used by the GUI element, so does the
// application use this class as a source (or repository)
// for the application data.
//-----------------------------------------------------------------------------
class AppEngine
{
public:
	AppEngine( );
	virtual ~AppEngine();
};

#endif // APPENGINE_H
