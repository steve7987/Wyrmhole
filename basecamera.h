#ifndef _BASECAMERA_H_
#define _BASECAMERA_H_


#include <D3D11.h>
#include <D3DX10math.h>
#include "vector.h"

class BaseCamera{
public:
	virtual void Render(float t) = 0;
	virtual void GetViewMatrix(D3DXMATRIX& viewMatrix) = 0;
	virtual Vector GetLookVector() = 0;
	virtual Vector GetPosition() = 0;
};

#endif