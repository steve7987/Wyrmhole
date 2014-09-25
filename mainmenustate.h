#ifndef _MAINMENUSTATE_H_
#define _MAINMENUSTATE_H_

#include "classlist.h"
#include "helpers.h"
#include "gamestate.h"
#include "gamestatemanager.h"
#include "input.h"
#include "gui.h"
#include "trackstate.h"
#include "level.h"

extern Graphics * g_graphics;
extern Gui * g_gui;
extern GameStateManager * g_gameStateManager;

class MainMenuState : public GameState{
public:
	MainMenuState();
	~MainMenuState();

	bool Initialize();
	void Shutdown(); 
	
	bool update(float t, Input * input);
	void render(float t);
	void onExit();
	void onEnter(Level * l);

private:
	int mmSentence;

};

#endif