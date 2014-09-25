#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "classlist.h"
#include "basecamera.h"
#include <D3DX10math.h>
#include "vector.h"
#include "quaternion.h"



class Camera : public BaseCamera{
public:
	Camera();
	~Camera();

	//these change actual position
	virtual void SetPosition(Vector v);
	void SetPosition(float x, float y, float z);
	
	//void SetDesiredPosition(Vector v);  //for spring based cam movement

	void SetRotation(Quaternion rot);
	void SetTarget(Vector v);
	void SetTarget(float x, float y, float z);

	Vector GetPosition();
	Quaternion GetRotation();
	Vector GetLookVector();


	virtual void Render(float t);
	void GetViewMatrix(D3DXMATRIX& viewMatrix);
	void GetGUIWorldMatrix(D3DXMATRIX& worldMatrix);  //gives matrix to translate gui based on camera position

private:
	Vector position;
	Quaternion rotation;
	Vector target;

	
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_worldMatrix;
};

#endif