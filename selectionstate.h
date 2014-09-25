#ifndef _SELECTIONSTATE_H_
#define _SELECTIONSTATE_H_

#include "classlist.h"
#include "helpers.h"
#include "gamestate.h"
#include "gamestatemanager.h"
#include "input.h"
#include "gui.h"
#include "ship.h"
#include "level.h"
#include "camera.h"
#include <deque>

extern Graphics * g_graphics;
extern Gui * g_gui;
extern GameStateManager * g_gameStateManager;

class SelectionState : public GameState {
public:
	SelectionState();
	~SelectionState();
	
	bool Initialize();
	void Shutdown(); 
	
	bool update(float t, Input * input);  //returns false if game should exit
	void render(float t);
	void onExit();
	void onEnter(Level * l);

private:
	bool LoadShips();
	
	int mmSentence;  //debug sentence says what state this is
	
	Ship * currentShip;
	Level * currentLevel;
	Camera * camera;

	std::deque<Ship*> shipList;
	std::deque<Ship*>::iterator curshipIt;
	float rotation;
};

#endif