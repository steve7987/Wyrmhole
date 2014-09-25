#ifndef _SPRINGCAMERA_H_
#define _SPRINGCAMERA_H_

#include "classlist.h"
#include "trackmanager.h"
#include <D3DX10math.h>
#include "vector.h"
#include "quaternion.h"
#include "camera.h"
#include "helpers.h"


class SpringCamera : public BaseCamera{
public:
	SpringCamera();

	bool Initialize(float positionWeight, float springStep, TrackManager * track);
	bool Initialize(char * datafile, TrackManager * track);

	Vector GetLookVector();
	Vector GetPosition();
	void Update(Vector shippos, float distance);
	void Reset();

	void Render(float t);
	void GetViewMatrix(D3DXMATRIX& viewMatrix);
	void GetGUIWorldMatrix(D3DXMATRIX& worldMatrix);

private:
	Vector position;
	Vector velocity;
	
	Vector look;

	Vector shipPosition;
	float distance;

	Quaternion rotation;


	//weights for spring
	float vweight;
	float dweight;
	float step;


	TrackManager * track;

	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_worldMatrix;
};

#endif