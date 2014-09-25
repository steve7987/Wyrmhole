#ifndef _DEBRIS_H_
#define _DEBRIS_H_


#include "graphics.h"
#include "collidable.h"
#include "vector.h"
#include "quaternion.h"
#include "helpers.h"
#include "model.h"
#include "level.h"
#include "ship.h"

extern Graphics * g_graphics;
extern Level * g_level;

class Debris : public Collidable{
public:
	Debris();
	~Debris();


	bool Initialize(char* modelFilename, WCHAR* textureFilename, Vector position, Quaternion rotation, Vector scale);
	void Shutdown();
	
	
	bool Update(float t);
	bool Render(float t);

	bool CollideWithPoint(Vector point, Shot * shot); 
	bool CollideWithEllipsoid(Vector point, Vector scale, Quaternion rotation, Shot * shot);
	bool CollideWithLineSegment(Vector start, Vector end, Shot * shot);  

private:
	Model * model;
	Vector position;
	Quaternion rotation;
	Vector scale;

	bool hit;  //true if this debris has been already hit by something
};


#endif