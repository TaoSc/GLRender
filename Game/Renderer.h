#pragma once

#include <memory>
#include <string>

#include <SDL.h>
#include <SimpleIni.h>

#include "Input.h"
#include "SDLDeleters.hpp"


class Renderer
{
public:
	Renderer(std::string const& window_title, std::string const& directory, CSimpleIniA const& ini_file);
	~Renderer();
	bool init();

	void mainLoop();

private:
	const std::string& m_window_title;
	unsigned int m_window_width;
	unsigned int m_window_height;
	const std::string& m_directory;

	std::unique_ptr<SDL_Window, SDLDeleters> m_window;
	SDL_GLContext m_context;

	const CSimpleIniA& m_ini_file;

	Input m_input;
};
