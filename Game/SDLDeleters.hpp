#pragma once

#include <SDL.h>


struct SDLDeleters
{
	void operator()(SDL_Surface* ptr) const { if (ptr) SDL_FreeSurface(ptr); }
	void operator()(SDL_Texture* ptr) const { if (ptr) SDL_DestroyTexture(ptr); }
	void operator()(SDL_Renderer* ptr) const { if (ptr) SDL_DestroyRenderer(ptr); }
	void operator()(SDL_Window* ptr) const { if (ptr) SDL_DestroyWindow(ptr); }
	void operator()(SDL_RWops* ptr) const { if (ptr) SDL_RWclose(ptr); }
};
