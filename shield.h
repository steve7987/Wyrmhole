#ifndef _SHIELD_H_
#define _SHIELD_H_

#include <list>
#include <D3D11.h>
#include <D3DX10math.h>

#include "graphics.h"
#include "basecamera.h"
#include "vector.h"
#include "quaternion.h"
#include "helpers.h"
#include "model.h"

struct ShieldType{  //stores info about shield hits for rendering
	Vector direction;  //gives direction from ship center to impact point
	float duration;  //remaining duration in seconds
	float strength;  //strength of hit
};

extern Graphics * g_graphics;

class Shield{
public:
	Shield();
	~Shield();

	bool Initialize(char* modelFilename, WCHAR* textureFilename, Vector shipScale);
	void Shutdown();

	void AddHit(Vector direction, float duration, float strength);
	void Update(float t);
	void Render(float t, Vector shipPos, Quaternion shipRot);

private:
	Vector scale;
	
	Model * shieldParts[8];
	//Model * farHalf;
	//Model * nearHalf;
	std::list<ShieldType> shieldHits;

};


#endif