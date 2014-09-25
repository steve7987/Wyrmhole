#include "debris.h"

Debris::Debris(){
	model = 0;
}
	
Debris::~Debris(){

}


	
bool Debris::Initialize(char* modelFilename, WCHAR* textureFilename, Vector position, Quaternion rotation, Vector scale){
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	model = new Model();
	if (!model){
		textDump("Error creating debris model");
		return false;
	}
	if (!model->Initialize(g_graphics->GetDevice(), modelFilename, textureFilename, false)){
		textDump("Error initializing debris model");
		return false;
	}
	model->SetPosition(position);
	model->SetRotation(rotation);
	model->SetScale(scale);
	hit = false;
	return true;
}
	
void Debris::Shutdown(){
	if (model){
		model->Shutdown();
		delete model;
		model = 0;
	}
}
	
	
	
bool Debris::Update(float t){
	return !hit;
}
	
bool Debris::Render(float t){
	g_graphics->RenderObject(model, SHADER_TEXTURE);
	return true;
}

	
bool Debris::CollideWithPoint(Vector point, Shot * shot){
	if (!hit && (point - position)*(point - position) < 5 * 5){
		hit = true;
		if (shot == NULL){
			//hit the ship
			g_level->GetShip()->DamageShield(0.5f, position);
		}
		return true;
	}
	return false;
} 
	
bool Debris::CollideWithEllipsoid(Vector point, Vector scale, Quaternion rotation, Shot * shot){
	return CollideWithPoint(point, shot);
}
	
bool Debris::CollideWithLineSegment(Vector start, Vector end, Shot * shot){
	return false;
}  