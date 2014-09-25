#include "light.h"

Light::Light(){
}
	
Light::~Light(){

}

	
void Light::SetDiffuseColor(float r, float g, float b, float a){
	m_diffuseColor = D3DXVECTOR4(r,g,b,a);
}
	
void Light::SetDirection(float x, float y, float z){
	m_direction = D3DXVECTOR3(x, y, z);
}

void Light::SetDirection(Vector v){
	m_direction = v.d3dvector();
}
	
D3DXVECTOR4 Light::GetDiffuseColor(){
	return m_diffuseColor;
}
	
D3DXVECTOR3 Light::GetDirection(){
	return m_direction;
}

void Light::SetAmbientColor(float r, float g, float b, float a){
	m_ambientColor = D3DXVECTOR4(r,g,b,a);
	
}

D3DXVECTOR4 Light::GetAmbientColor(){
	return m_ambientColor;
}