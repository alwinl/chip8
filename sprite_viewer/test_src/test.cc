#include <SDL.h>
#include <glib.h>
#include <gtkmm.h>
#include <gtkmm/socket.h>

#include <iostream>
#include <sstream>

class WindowSDL
{
public:
	WindowSDL( int argc, char **argv );
	~WindowSDL();

	WindowSDL( WindowSDL & ) = delete;
	WindowSDL( WindowSDL && ) = delete;

	WindowSDL &operator=( WindowSDL & ) = delete;
	WindowSDL &operator=( WindowSDL && ) = delete;

	bool loopSDL();
	static void clear();
	void run();

	// SDL_Surface *obtSuperficie() { return surface; }

private:
	Gtk::Main main_;
	Gtk::Socket socket_;
	Gtk::Window window_;
	Gtk::VBox vbox_;

	SDL_Surface *surface = nullptr;
};

WindowSDL::WindowSDL( int argc, char **argv ) : main_( &argc, &argv )
{
	window_.set_title( "Ventana SDL" );
	socket_.set_size_request( 320, 200 );
	window_.set_border_width( 6 );
}

WindowSDL::~WindowSDL()
{
	if( surface != nullptr )
		SDL_FreeSurface( surface );
}

bool WindowSDL::loopSDL()
{
	SDL_Event event;
	// SDL_Surface *display = this->surface;

	while( SDL_PollEvent( &event ) != 0 ) {
		// Handle quit event, not sure if this will ever appear
		if( event.type == SDL_QUIT )
			return FALSE;

		// Handle clear userevent
		if( event.type == SDL_USEREVENT && event.user.code == 0 ) {
			SDL_FillRect( surface, nullptr, 0 );
			// SDL_Flip( surface );
		}

		// Handle draw rect userevent
		if( event.type == SDL_USEREVENT && event.user.code == 1 ) {
			SDL_Rect rect;

			rect.x = rand() % 320;
			rect.y = rand() % 200;
			rect.w = rand() % 100 + 10;
			rect.h = rand() % 100 + 10;

			SDL_FillRect( surface, &rect, SDL_MapRGB( surface->format, rand() % 255, rand() % 255, rand() % 255 ) );
			// SDL_Flip( surface );
		}
	}
	// Forzar a que se dibuje un cuadrado nuevo
	event.type = SDL_USEREVENT;
	event.user.code = 1;
	SDL_PushEvent( &event );

	return true;
}

void WindowSDL::clear()
{
	/* Put clear userevent on queue */
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = 0;
	SDL_PushEvent( &event );
}

void WindowSDL::run()
{
	vbox_.pack_start( socket_, false, false );

	window_.add( vbox_ );
	window_.show_all();

	std::string winid( "SDL_WINDOWID=" );

	winid += std::to_string( socket_.get_id() );

	// SDL_putenv( winid.c_str() );

	if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
		throw std::runtime_error( "Error initialising SDL" );

	// surface = SDL_SetVideoMode( 320, 200, 0, 0 );
	clear();

	Glib::signal_timeout().connect( sigc::mem_fun( *this, &WindowSDL::loopSDL ), 5 );
	// Use this for global (non class) functions:
	// Glib::signal_timeout().connect(sigc::ptr_fun(&loopSDL), 100);

	main_.run( window_ );
}

int main( int argc, char **argv )
{
	WindowSDL app( argc, argv );

	app.run();
}