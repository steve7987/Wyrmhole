#ifndef _RING_H_
#define _RING_H_


#include "graphics.h"
#include "collidable.h"
#include "vector.h"
#include "quaternion.h"
#include "helpers.h"
#include "model.h"
#include "level.h"

extern Graphics * g_graphics;
extern Level * g_level;

class Ring : public Collidable{
public:
	Ring();
	~Ring();

	bool Initialize(char* modelFilename, WCHAR* textureFilename, Vector position, 
					Quaternion rotation, float radius, float thickness);
	void Shutdown();
	
	
	bool Update(float t);
	bool Render(float t);

	bool CollideWithPoint(Vector point, Shot * shot); 
	bool CollideWithEllipsoid(Vector point, Vector scale, Quaternion rotation, Shot * shot);  //same as point for now
	bool CollideWithLineSegment(Vector start, Vector end, Vector& collisionPoint, Shot * shot);  //just checks the ends
	bool CollideWithRay(Vector origin, Vector direction, Vector& collisionPoint, Shot * shot); //not implemented

private:
	Model * model;
	Vector position;
	Quaternion rotation;  //rotation from track
	float radius;
	float thickness;

	float spinAngle;  //after ring collided with increases to spin ring

};


#endif