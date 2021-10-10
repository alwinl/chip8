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

#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <gtkmm.h>

class AppEngine;
class GUIView;
class EmulatorScreen;

//-----------------------------------------------------------------------------
// The GUIController class is the controller of the MVC construct employed here
// This class combines and regulates the existence of windows or views (the V in MVC,
// the existence and status of the application data and logic (the M(odel) in MVC)
//
// The Gtk::Application class already has management functions for windows
// so what we have to add is the model paradigm.
//-----------------------------------------------------------------------------
class GUIController : public Gtk::Application
{
public:
	// Instance creator as constructor is private
	static Glib::RefPtr<GUIController> create( AppEngine* engine );
	virtual ~GUIController();

private:
	// Private construction to ensure only references can be obtained. Private as class is terminal
	GUIController( AppEngine* engine );

	// Overrides
	virtual void on_startup();
	virtual void on_activate();

	// Actions
	void on_action_new();			// initialise the model to the empty condition
	void on_action_open();			// load the model from a file
	void on_action_save();			// save the model to a file
	void on_action_save_as();		// save the model to a new file

	void on_action_quit();			// quit application

	void on_action_about();

	//bool on_board_button_pressed( GdkEventButton* button_event );
	//bool on_board_button_released( GdkEventButton* button_event );

	// Widgets
	GUIView * view;
	EmulatorScreen * emul_screen;
	Glib::RefPtr<Gtk::TextBuffer> register_window;


	// Data
	AppEngine * engine;
};

#endif // GUICONTROLLER_H
