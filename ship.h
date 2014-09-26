#ifndef _SHIP_H_
#define _SHIP_H_

#include <list>

#include "graphics.h"
#include "vector.h"
#include "quaternion.h"
#include "helpers.h"
#include "model.h"
#include "trackmanager.h"
#include "billboard.h"
#include "shield.h"
#include "collidable.h"

extern Graphics * g_graphics;
#define TOLERANCE 0.000001f 

#define STAT_FORWARDV 0
#define STAT_BOOSTV 1
#define STAT_BOOSTA 2
#define STAT_HORIZONTALV 3
#define STAT_HORIZONTALA 4
#define STAT_HORIZONTALROLL 5
#define STAT_SHIELDX 6
#define STAT_SHIELDY 7
#define STAT_SHIELDZ 8



class Ship{
public:
	Ship();
	~Ship();

	bool Initialize(char * datafile);
	void Shutdown();
	
	void SetTrack(TrackManager * track);

	void Update(float t);  //updates the ship when in the track state
	void Render(float t);

	void DisplayUpdate(float t);  //updates the ship when in a display state (e.g. selection state)

	void Boost();
	void HorizontalAcceleration(Vector accel);

	void SetDistance(float dist);
	Quaternion GetRotation();  //gives final rotation of ship i.e. trackRotation * turnRotation
	Vector GetPosition();  //returns position with offset
	float GetDistance();   //give distance down track

	void DamageShield(float strength, Vector position);  //position should be in world space
	
	
	//only used for modeling ship (in selection state), use update during actual game (trackstate)
	void SetPosition(Vector v);  
	void SetRotation(Quaternion q);

private:
	bool loadInfo(char * datafile);
	void UpdateEngine(float t);
	void CheckCollisions(float t);
	
	
	float enginet;
	

	float boostVelocity;  //current velocity bonus from using boost/brake
	bool boostOn;  //true if boost is active, false if its off

	Vector horizontalVelocity;
	Vector horizontalAccel;
	bool horizontalOn;

	Vector prevOffset; //for debug
	Vector horizontalOffset;  //offset due to movment perpendicular to the track
	Vector position;  //position on the track
	Vector heading;
	Quaternion trackRotation;  //rotation to made nose point down track
	Quaternion turnRotation;  //roll, pitch for ship when moving horizontally
	float currentDistance;
	

	
	float * stats;
	int numStats;
	Model * model;
	Billboard * engineBB;
	TrackManager * track;
	Shield * shield;
};

#endif