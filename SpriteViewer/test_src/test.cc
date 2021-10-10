#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <glib.h>
#include <SDL.h>

#include <sstream>
#include <iostream>

using namespace std;

class WindowSDL
{
private:
    Gtk::Main main_;
    Gtk::Socket socket_;
    Gtk::Window window_;
    Gtk::VBox vbox_;
    SDL_Surface * surface;

public:

    WindowSDL( int argc, char ** argv ) : main_( &argc, &argv ), surface( 0 )
    {
        window_.set_title( "Ventana SDL" );
        socket_.set_size_request( 320,200 );
        window_.set_border_width( 6 );
    }

    bool loopSDL()
    {
        SDL_Event event;
        SDL_Surface *display = this->surface;
        while( SDL_PollEvent( &event ) ) {
// Handle quit event, not sure if this will ever appear
            if( event.type == SDL_QUIT ) return FALSE;
// Handle clear userevent
            if( event.type == SDL_USEREVENT && event.user.code == 0 ) {
                SDL_FillRect( display, NULL, 0 );
                SDL_Flip( display );
            }

// Handle draw rect userevent
            if( event.type == SDL_USEREVENT && event.user.code == 1 ) {
                SDL_Rect rect;

                rect.x = rand() % 320;
                rect.y = rand() % 200;
                rect.w = rand() % 100 + 10;
                rect.h = rand() % 100 + 10;

                SDL_FillRect( display, &rect, SDL_MapRGB( display->format,
                              rand()%255, rand()%255, rand()%255 ) );
                SDL_Flip( display );
            }
        }
// Forzar a que se dibuje un cuadrado nuevo
        event.type = SDL_USEREVENT;
        event.user.code = 1;
        SDL_PushEvent( &event );

        return true;
    }

    void clear( void )
    {
        /* Put clear userevent on queue */
        SDL_Event event;
        event.type = SDL_USEREVENT;
        event.user.code = 0;
        SDL_PushEvent( &event );
    }

    void run()
    {
        vbox_.pack_start( socket_, false, false );

        window_.add( vbox_ );
        window_.show_all();

        std::stringstream sdlhack;
        sdlhack << "SDL_WINDOWID=" << socket_.get_id() << std::ends;

        char* winhack=new char[32];
        sprintf( winhack,sdlhack.str().c_str() );
        SDL_putenv( winhack );

        if( SDL_Init( SDL_INIT_VIDEO ) )
            throw std::runtime_error( "Error al inicializar la SDL" );
        else {
            surface = SDL_SetVideoMode( 320, 200, 0, 0 );
            clear();
        }


        Glib::signal_timeout().connect( sigc::mem_fun( *this,&WindowSDL::loopSDL ),
                                        5 );
// Use this for global (non class) functions:
// Glib::signal_timeout().connect(sigc::ptr_fun(&loopSDL), 100);

        main_.run( window_ );
    }


    ~WindowSDL()
    {
        if( surface )
			SDL_FreeSurface( surface );
    }

    SDL_Surface* obtSuperficie()
    {
        return surface;
    }

};



int main( int argc, char ** argv )
{
    WindowSDL app( argc, argv );

    app.run();

}
