#ifndef _TRACKSTATE_H_
#define _TRACKSTATE_H_

#include <time.h>

#include "classlist.h"
#include "helpers.h"
#include "gamestate.h"
#include "gamestatemanager.h"
#include "input.h"
#include "gui.h"
#include "ship.h"
#include "camera.h"
#include "vector.h"
#include "trackmanager.h"
#include "springcamera.h"
#include "light.h"
#include "level.h"

extern Graphics * g_graphics;
extern Gui * g_gui;
extern GameStateManager * g_gameStateManager;
extern int g_screenWidth, g_screenHeight;

class TrackState : public GameState{
public:
	TrackState();
	~TrackState();

	bool Initialize();
	void Shutdown(); 
	
	bool update(float t, Input * input);
	void render(float t);
	void onExit();
	void onEnter(Level * l);

private:
	void UpdateCamera(float t, Input * input);
	void HandleInput(Input * input);

	void SetLightParameters(char * tubedatafile);

	int tSentence, distSentence, scoreSentence;  //sentences for game state, current distance, and score
	float curdist;  //distance of ship down the track
	int prevdist;  //for displaying distance sentence

	int curscore;

	Ship * heroShip;

	TrackManager * track;

	BaseCamera * activecam;

	SpringCamera * springcam;
	Camera * camera;
	Light * tunnelLight;
	Level * level;  //holds data about the current level
};

#endif