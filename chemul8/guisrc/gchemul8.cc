/*
 * Copyright 2024 Alwin Leerling <dna.leerling@gmail.com>
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
 */

#include <gtkmm.h>

#include <iostream>

void activate( Glib::RefPtr<Gtk::Application> app );
void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

int main( int argc, char **argv )
{
	Glib::set_init_to_users_preferred_locale( false );

	auto app = Gtk::Application::create();

	app->signal_activate().connect( [app]() { activate( app ); } );

	return app->run( argc, argv );
}

void activate( Glib::RefPtr<Gtk::Application> app )
{
	auto builder = Gtk::Builder::create_from_file( "gchemul8.ui" );

	auto window = builder->get_widget<Gtk::ApplicationWindow>( "appwindow" );
	auto canvas = builder->get_widget<Gtk::DrawingArea>( "canvas" );

	canvas->set_draw_func( sigc::fun(on_draw) );

	window->show();

	app->add_action( "click", [](){std::cout << "got clicked\n"; } );

	app->add_window( *window );
}

void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
	cr->set_source_rgb( 0.8, 0.2, 0 );

	cr->move_to( width / 2, height / 2);
	cr->line_to( 0, 0 );

	cr->move_to( width / 2, height / 2);
	cr->line_to( width, height / 2 );

	cr->move_to( width / 2, height / 2);
	cr->line_to( 0, height );
}