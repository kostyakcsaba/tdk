
// GLEW
#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

#pragma comment(lib, "glew32.lib")

#include "SDL.h"
#include <iostream>

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "opengl32.lib")

#include "MyApp.h"

int main( int argc, char* args[] )
{
    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    
	SDL_Surface* screen = SDL_SetVideoMode( 800, 600, 32, SDL_OPENGL | SDL_RESIZABLE );

	if ( !screen )
	{
		// baj van
		return -1;
	}

	SDL_WM_SetCaption( "Hello OGLie World", NULL );

	GLenum error = glewInit();
	if ( error != GLEW_OK )
	{
		MessageBox( 0, TEXT("Nem sikerült a GLEW-et inicializálni!"), TEXT("Hiba"), 0);
		return -1;
	}

	// nézzük meg, hogy milyen OGL verzió fut
	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); 
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); 
	if ( glVersion[0] < 3 )
	{
		MessageBox( 0, TEXT("Nem sikerült OGL3/4-es context-et létrehozni!"), TEXT("Hiba"), 0);
		return -1;
	}
	std::cout << "Running OpenGL " << glVersion[0] << "." << glVersion[1] << std::endl;

	bool quit = false;
	SDL_Event ev;

	CMyApp app;
	if (!app.Init())
	{
		MessageBox( 0, TEXT("Error initialising application!"), TEXT("Hiba"), 0);
		std::cout << "Error initialising application!" << std::endl;
		return -1;
	}

	int tickStart = 0;
	static char buffer[30];

	while ( !quit )
	{
		// amíg van esemény, kezeljük õket
		while( SDL_PollEvent( &ev ) )
		{
			switch ( ev.type )
			{
			case SDL_KEYDOWN:
				if ( ev.key.keysym.sym == SDLK_ESCAPE )
					quit = true;
				app.KeyboardDown( ev.key );
				break;
			case SDL_KEYUP:
				app.KeyboardUp(ev.key);
				break;
			case SDL_MOUSEMOTION:
				app.Mouse(ev.motion);
				break;
			case SDL_VIDEORESIZE:
				app.Resize(ev.resize);
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}
				
		}

		tickStart = SDL_GetTicks();

		app.Update();
		app.Render();

		sprintf_s(buffer, 30, "Frame rendered in %d ms", (SDL_GetTicks() - tickStart) );

		SDL_WM_SetCaption( buffer, 0);
	}

	app.Clean();

    //Quit SDL
    SDL_Quit();
    
    return 0;    
}