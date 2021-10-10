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

#include "guicontroller.h"
#include "guiview.h"
#include "emulatorscreen.h"

#include "appbase.h"

//-----------------------------------------------------------------------------
//	Create an instance of the application
//-----------------------------------------------------------------------------
Glib::RefPtr<GUIController> GUIController::create( AppEngine* engine )
	{ return Glib::RefPtr<GUIController>( new GUIController( engine ) ); }

//-----------------------------------------------------------------------------
//	Application class constructor
//-----------------------------------------------------------------------------
GUIController::GUIController( AppEngine* engine ) : Gtk::Application( "net.dnatechnologies.application" )
{
	Glib::set_application_name("Application");

	this->engine = engine;
}

//-----------------------------------------------------------------------------
//	Application class destructor
//-----------------------------------------------------------------------------
GUIController::~GUIController( )
{
}

void GUIController::on_startup()
{
	Gtk::Application::on_startup();

    add_action("new",     sigc::mem_fun( *this, &GUIController::on_action_new ) );
    add_action("open",    sigc::mem_fun( *this, &GUIController::on_action_open ) );
    add_action("save",    sigc::mem_fun( *this, &GUIController::on_action_save ) );
    add_action("saveas",  sigc::mem_fun( *this, &GUIController::on_action_save_as ) );
    add_action("quit",    sigc::mem_fun( *this, &GUIController::on_action_quit ) );
	add_action("about",   sigc::mem_fun( *this, &GUIController::on_action_about ) );

    Glib::RefPtr<Gtk::Builder> ui_model = Gtk::Builder::create_from_resource( "/net/dnatechnologies/application/gui.glade" );

	ui_model->get_widget_derived("appView", view );
	ui_model->get_widget_derived("drwMain", emul_screen );

	register_window = Glib::RefPtr<Gtk::TextBuffer>::cast_dynamic( ui_model->get_object( "wndRegister" ) );

	//frame->signal_button_press_event().connect( sigc::mem_fun( *this, &GUIController::on_board_button_pressed) );
	//frame->signal_button_release_event().connect( sigc::mem_fun( *this, &GUIController::on_board_button_released) );
}

//-----------------------------------------------------------------------------
// The application has been started. Time to create and show a window
// NB we could build multiple views here and activate (show) views
// as needed
//-----------------------------------------------------------------------------
void GUIController::on_activate()
{
	add_window( *view );
	view->show();



	register_window->set_text( "V0 : 0x00\tV1 : 0x01\tV2 : 0x00\tV3 : 0x01\n"
							"V4 : 0x00\tV5 : 0x01\tV6 : 0x00\tV7 : 0x01\n"
							"V8 : 0x00\tV9 : 0x01\tVA : 0x00\tVB : 0x01\n"
							"VC : 0x00\tVD : 0x01\tVE : 0x00\tVF : 0x01\n");

	on_action_new();
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void GUIController::on_action_new()
{
	Gtk::MessageDialog("on_action_new not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void GUIController::on_action_open()
{
	Gtk::MessageDialog("on_action_open not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void GUIController::on_action_save()
{
	Gtk::MessageDialog("on_action_save not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void GUIController::on_action_save_as()
{
	Gtk::MessageDialog("on_action_save_as not implemented yet", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true).run();
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
void GUIController::on_action_quit()
{
	quit();
}

/*
//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
bool GUIController::on_board_button_pressed( GdkEventButton* button_event )
{
	return true;
}

//-----------------------------------------------------------------------------
// Action implementation
//-----------------------------------------------------------------------------
bool GUIController::on_board_button_released( GdkEventButton* button_event )
{
	return true;
}
*/

/**-----------------------------------------------------------------------------
 * \brief Show some application information
 *
 * Show our Help About... box
 */
void GUIController::on_action_about()
{
    Gtk::AboutDialog dlg;

    dlg.set_transient_for( *view ) ;
    dlg.set_name("application") ;
    //dlg.set_logo( Gdk::Pixbuf::create_from_resource("/net/dnatechnologies/application/application.png") ) ;
    dlg.set_version( "0.01" ) ;
    dlg.set_comments("A GtkMM application") ;
    dlg.set_copyright( "Copyright © 2020 Alwin Leerling" ) ;
    dlg.set_website( "http://github" ) ;

    dlg.run();
}

