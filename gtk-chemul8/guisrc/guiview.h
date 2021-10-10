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

#ifndef GUIVIEW_H
#define GUIVIEW_H

#include <gtkmm.h>

class GUIController;

//-----------------------------------------------------------------------------
// This is our view class of the MVC.
//
// You will typically build a complete widget tree and hang that
// of this "window". In a sense this class represents a frame
// around your client window.
//
// You can draw on the GUIView widget by overriding the on_draw
// function, but you have to take care not to draw over the
// area used for menus
//
//-----------------------------------------------------------------------------
class GUIView : public Gtk::ApplicationWindow
{
public:
	GUIView( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model );
	virtual ~GUIView();

private:

	// action handlers
	void on_action_cut();
	void on_action_copy();
	void on_action_paste();

	void on_action_about();
};

#endif // GUIVIEW_H
