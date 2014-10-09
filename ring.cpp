#include "ring.h"


Ring::Ring(){
	model = 0;
}
	
Ring::~Ring(){

}

	
bool Ring::Initialize(char* modelFilename, WCHAR* textureFilename, Vector position, 
					  Quaternion rotation, float radius, float thickness)
{
	this->position = position;
	this->rotation = rotation;
	this->radius = radius;
	this->thickness = thickness;
	model = new Model();
	if (!model){
		textDump("Error creating ring model");
		return false;
	}
	if (!model->Initialize(g_graphics->GetDevice(), modelFilename, textureFilename, false)){
		textDump("Error making ring model");
		return false;
	}
	model->SetPosition(position);
	model->SetRotation(rotation);
	model->SetScale(radius, radius, radius);
	spinAngle = 0;
	return true;
}

void Ring::Shutdown(){
	if (model){
		model->Shutdown();
		delete model;
		model = 0;
	}
}
	
	

bool Ring::Update(float t){
	if (spinAngle > 0){
		spinAngle += t/1000;
		model->SetRotation(rotation*Quaternion(Vector(1,0,0), spinAngle));
	}
	return spinAngle < 1.0f;
}

bool Ring::Render(float t){
	if (spinAngle > 0){
		g_graphics->RenderObject(model, SHADER_TEXTURE);
	}
	else {
		g_graphics->RenderObject(model, SHADER_LIGHT);
	}
	return true;
}


bool Ring::CollideWithPoint(Vector point, Shot * shot){
	if (spinAngle == 0 && (point-position)*(point-position) < radius*radius){
		spinAngle = 0.001f;
		g_level->SetScore(g_level->GetScore() + 1);
		return true;
	}
	return false;
} 

bool Ring::CollideWithEllipsoid(Vector point, Vector scale, Quaternion rotation, Shot * shot){
	return CollideWithPoint(point, shot);
}
	
bool Ring::CollideWithLineSegment(Vector start, Vector end, Vector& collisionPoint, Shot * shot){
	return CollideWithPoint(start, shot) || CollideWithPoint(end, shot);
}

bool Ring::CollideWithRay(Vector origin, Vector direction, Vector& collisionPoint, Shot * shot){
	return false;
}