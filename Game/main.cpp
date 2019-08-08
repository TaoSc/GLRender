#include <string>

#include <SimpleIni.h>

#include "Renderer.h"


int main(int argc, char* argv[])
{
	const std::string directory = "C:/Users/taosc/Documents/Repositories/Game/";
	const std::string window_title = "Game";

	CSimpleIniA ini_file;
	ini_file.SetUnicode();
	ini_file.LoadFile((directory + "config.ini").c_str());

	Renderer mainRender{ window_title, directory, ini_file };

	if (!mainRender.init())
		return EXIT_FAILURE;

	mainRender.mainLoop();

	return EXIT_SUCCESS;
}
