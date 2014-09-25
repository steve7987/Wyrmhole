#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "classlist.h"
#include "input.h"
#include "level.h"

class GameState{
public:
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0; 
	
	virtual bool update(float t, Input * input) = 0;  //returns false if game should exit
	virtual void render(float t) = 0;
	virtual void onExit() = 0;
	virtual void onEnter(Level * l) = 0;

};

#endif