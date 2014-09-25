#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "classlist.h"
#include "helpers.h"

#include "ship.h"

class Level{
public:
	Level();
	~Level();

	bool Initialize();
	void Shutdown();

	Ship * GetShip();
	void SetShip(Ship * s);

	void SetScore(int s);
	int GetScore();

private:
	Ship * currentShip;
	int score;
};

#endif