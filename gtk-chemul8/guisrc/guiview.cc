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

#include "guiview.h"

//-----------------------------------------------------------------------------
// The GUIView class
//
// Director of all screen real estate supplied to the application
//-----------------------------------------------------------------------------
GUIView::GUIView( BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ui_model ) : Gtk::ApplicationWindow(cobject)
{
	Glib::RefPtr<Gio::SimpleActionGroup> action_group = Gio::SimpleActionGroup::create();

	action_group->add_action( "cut", sigc::mem_fun( *this, &GUIView::on_action_cut ) );
	action_group->add_action( "copy", sigc::mem_fun( *this, &GUIView::on_action_copy ) );
	action_group->add_action( "paste", sigc::mem_fun( *this, &GUIView::on_action_paste ) );

	insert_action_group( "win", action_group );

	set_default_size(640,480);

	show_all_children();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GUIView::~GUIView()
{
}

//-----------------------------------------------------------------------------
// action implementation
//-----------------------------------------------------------------------------
void GUIView::on_action_cut()
{
	Gtk::MessageDialog("on_action_cut not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

//-----------------------------------------------------------------------------
// action implementation
//-----------------------------------------------------------------------------
void GUIView::on_action_copy()
{
	Gtk::MessageDialog("on_action_copy not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

//-----------------------------------------------------------------------------
// action implementation
//-----------------------------------------------------------------------------
void GUIView::on_action_paste()
{
	Gtk::MessageDialog("on_action_paste not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}
