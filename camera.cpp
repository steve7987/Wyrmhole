#include "camera.h"


Camera::Camera(){
	position = Vector(0,0,0);
	rotation = Quaternion(0, 0, 0, 1);
	target = Vector(0, 0, -1);
}

Camera::~Camera(){

}

void Camera::SetPosition(Vector v){
	position = v;
}



void Camera::SetPosition(float x, float y, float z){
	position.x = x;
	position.y = y;
	position.z = z;
}

void Camera::SetRotation(Quaternion rot){
	rotation = rot;
}

void Camera::SetTarget(Vector v){
	target = v;;
}

void Camera::SetTarget(float x, float y, float z){
	target.x = x;
	target.y = y;
	target.z = z;
}

Vector Camera::GetPosition(){
	return position;
}

Quaternion Camera::GetRotation(){
	return rotation;
}

Vector Camera::GetLookVector(){
	return (target - position) / (target - position).length();
}

//build view matrix first rotate then translate
void Camera::Render(float t){
	// Setup the vector that points upwards.
	Vector up = rotation*Vector(0,1,0);
	up = up / up.length();

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position.d3dvector(), &target.d3dvector(), &up.d3dvector());
	//create world matrix to adjust gui
	D3DXMatrixTranslation(&m_worldMatrix, (float)position.x, (float)position.y, (float)position.z);
}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix){
	viewMatrix = m_viewMatrix;
}

void Camera::GetGUIWorldMatrix(D3DXMATRIX& worldMatrix){
	worldMatrix = m_worldMatrix;
}