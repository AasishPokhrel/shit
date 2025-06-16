#include <SDL.h>

#include "savepng.h" // includes <png.h>, you must link with -lpng

int main( int argc, char* args[] )
{
	SDL_Surface *screen;
	SDL_Surface *shot;
	Uint32 color;

	if (SDL_Init(SDL_INIT_VIDEO) < 0 || (screen = SDL_SetVideoMode(640, 480, 0, 0)) == 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}

	/* Fill screen with random color */
	srand(time(NULL));
	color = SDL_MapRGB(screen->format,
	    rand() % 255,
	    rand() % 255,
	    rand() % 255) | 0xFF000000;
	SDL_FillRect(screen, NULL, color);

/*
	SDL_Surface *bmp = SDL_LoadBMP("image.bmp"); // load a bitmap file
	SDL_SavePNG(bmp, "image.png"); // save it as png

	SDL_Rect dest = { 0, 0, bmp->w, bmp->h }; // blit it to screen, just to make screen more interesting
	SDL_BlitSurface(bmp, NULL, screen, &dest);
*/

	/* Update screen, just so we can see it */
	SDL_Delay(100);
	SDL_Flip(screen);
	SDL_Delay(1000);

	/* Save screen as PNG */
	shot = SDL_PNGFormatAlpha(screen);	/* SDL_PNGFormatAlpha is optional, but might be necessary for SCREEN surfaces */
	SDL_SavePNG(shot, "screen.png");
	SDL_FreeSurface(shot);

	SDL_Quit();
	return 0;
}
