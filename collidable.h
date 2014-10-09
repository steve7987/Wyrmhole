#ifndef _COLLIDABLE_H_
#define _COLLIDABLE_H_

#include "vector.h"
#include "quaternion.h"
#include "shot.h"

class Collidable{
public:
	virtual bool Update(float t) = 0;  //returns false if the collidable should be removed and true otherwise
	virtual bool Render(float t) = 0;

	//null shot means it is the ship which is hitting
	virtual bool CollideWithPoint(Vector point, Shot * shot) = 0; 

	//generally used to see if ship hits the collidable
	virtual bool CollideWithEllipsoid(Vector point, Vector scale, Quaternion rotation, Shot * shot) = 0;

	//see if the object collides with a line segment, gives back the location of the collision
	virtual bool CollideWithLineSegment(Vector start, Vector end, Vector& collisionPoint, Shot * shot) = 0;  

	//see if the object collides with an infinite ray, gives back the location of the collision
	virtual bool CollideWithRay(Vector origin, Vector direction, Vector& collisionPoint, Shot * shot) = 0;

	virtual void Shutdown() = 0;
};


#endif