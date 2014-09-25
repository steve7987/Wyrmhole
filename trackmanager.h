#ifndef _TRACKMANAGER_H_
#define _TRACKMANAGER_H_

#include <list>
#include <deque>

#include "tracksegment.h"
#include "classlist.h"
#include "vector.h"


class TrackManager{
public:
	TrackManager();
	~TrackManager();

	//lengths are approximate circle sections may be longer, displacement is max change in a single coord for getting p3
	//bool Initialize(int numSegments, int seed, float minLength, float maxLength, float maxDisplacement);
	bool Initialize(int seed, char * tubedatafile);
	void Shutdown();

	Vector GetTangent(float distance);  //gives unit vector tangent at this distance
	Quaternion GetRotation(float distance);
	Vector GetPoint(float distance);  //gives point coord at this distance
	float GetLength();

	float GetSegmentRadius(float distance);  //gives the radius of the circle at this dist
	float GetTrackRadius();  //gives radius of tube

	void Render(float distance);
	void Update(float distance, float t);

	std::list<Collidable*> * GetCollidables(float distance);

private:

	std::deque<TrackSegment*> segmentList;
	float totalLength;
	float trackRadius;  //radius of the tube
};


#endif