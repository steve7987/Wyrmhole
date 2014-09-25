#include "gapp.h"
#include "vld.h"

//global vars ALL set in gapp.cpp except g_level
TextureManager * g_textureManager;
Graphics * g_graphics;
Gui * g_gui;
GameStateManager * g_gameStateManager;
int g_screenWidth, g_screenHeight;
ModelManager * g_modelManager;
Level * g_level;  //set in gamestatemanager.cpp

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Gapp * app;

	//make the app
	app = new Gapp();
	if (!app){
		return 0;
	}
	//initialize and run
	if (!app->Initialize()){
		return 0;
	}

	app->Run();
	//shutdown and release
	app->Shutdown();
	delete app;
	app = 0;
	
	return 0;
}

