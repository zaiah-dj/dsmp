#include <SDL2/SDL.h>

int main (int argc, char *argv[]) {
	SDL_bool done;
	SDL_Window *window;
	SDL_Event event;
	char *dropped_filedir;

	SDL_Init( SDL_INIT_VIDEO );

	window = SDL_CreateWindow(
		"SDL_DropEvent usage, please drop the file on window."
		,SDL_WINDOWPOS_CENTERED
		,SDL_WINDOWPOS_CENTERED
		,640
		,480
		,SDL_WINDOW_OPENGL
	);

	if ( window == NULL ) {
		SDL_Log( "Could not create window: %s", SDL_GetError() );
		SDL_Quit();
		return 1;
	}

	SDL_EventState( SDL_DROPFILE, SDL_ENABLE );

	done = SDL_FALSE;

	while ( !done ) {
		while ( !done && SDL_PollEvent( &event )) {
			switch (event.type) {
				case SDL_QUIT:
					done = SDL_TRUE;
					break;

				case SDL_DROPFILE:
					dropped_filedir = event.drop.file;	
					SDL_ShowSimpleMessageBox(
						SDL_MESSAGEBOX_INFORMATION
					 ,"File dropped on window"
					 ,dropped_filedir
					 ,window
					);
					SDL_free( dropped_filedir );
					break;
			}
		}	
		SDL_Delay( 0 );
	}

	SDL_DestroyWindow( window );
	SDL_Quit();
	return 0;
}
