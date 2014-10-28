#ifndef _TRACKSEGMENT_H_
#define _TRACKSEGMENT_H_

#include <list>
#include <math.h>

#include "graphics.h"
#include "vector.h"
#include "quaternion.h"
#include "trackline.h"
#include "tracktube.h"
#include "collidable.h"
#include "ring.h"
#include "debris.h"

extern Graphics * g_graphics;

class TrackSegment{
public:
	TrackSegment();
	~TrackSegment();

	bool Initialize(Vector p1, Vector p2, Vector p3, Vector p4, Quaternion beginrot, double startdist, double rad, 
					double texturerepeat, int tubesides, int tubesegments, WCHAR * texturefile, float randomness, int smoothingPasses);
	void Shutdown();

	Vector GetPoint(double distance);  //gives the point at the given distance down the segment
	Vector GetTangent(double distance);  //gives unit vector tangent to the curve at this point 
	Quaternion GetRotation(double distance);  //give quat representing tracks rotation from <1,0,0>
	double GetLength();  //total length of the segment

	float GetSegmentRadius(double distance);  //gives the radius of the circle at this dist (negative means in third part)

	void Update(float t);
	void Render();  //renders the graphics associated withe the track segment
	std::list<Collidable*> * GetCollidables();
	
	int GetSegment(float distance);  //returns type of segment corresponding to this distance.  1 = enter line, 2 = circle, 3 = exit line
	Vector GetCenter();  //returns center of turning circle
	double GetSegmentStart(int segment);  //gives the start point of the corresponding segment

private:
	Vector p1, p2, p3, p4;
	double length;
	TrackLine * tline;
	TrackTube * ttube;

	double startdist;

	Vector center;  //center of turning circle
	Vector v1, v2;  //perpendicular unit vectors that allow for paramaterization of the circle
	double radius, maxAngle;  //radius of circle and angle for exit point

	double s1, s2, s3;  //lengths of 1st line seg, circle seg and second line segment

	Quaternion beginrot;  //rotation at the beginning of this segment

	std::list<Collidable*> collidables;

};

#endif