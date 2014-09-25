#ifndef _LIGHT_H_
#define _LIGHT_H_


#include <d3dx10math.h>
#include "vector.h"

class Light {
public:
	Light();
	~Light();

	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDirection(Vector v);
	void SetDirection(float x, float y, float z);
	void SetAmbientColor(float r, float g, float b, float a);


	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();


private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_ambientColor;
};

#endif