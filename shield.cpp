#include "shield.h"


Shield::Shield(){
	for (int i = 0; i < 16; i++){
		shieldParts[i] = 0;
	}
}

Shield::~Shield(){

}


bool Shield::Initialize(char* modelFilename, WCHAR* textureFilename, Vector shipScale){
	scale = shipScale;
	for (int i = 0; i < 16; i++){
		shieldParts[i] = new Model();
		if (!shieldParts[i]) {
			textDump("Error creating shield model");
			return false;
		}
		if (!shieldParts[i]->Initialize(g_graphics->GetDevice(), modelFilename, textureFilename, true)){
			textDump("Error initializing shield model");
			return false;
		}
	}
	return true;
}

void Shield::Shutdown(){
	for (int i = 0; i < 16; i++){
		if (shieldParts[i]){
			shieldParts[i]->Shutdown();
			delete shieldParts[i];
			shieldParts[i] = 0;
		}
	}
}


void Shield::AddHit(Vector direction, float duration, float strength){
	ShieldType s;
	s.direction = direction;
	s.direction = s.direction / s.direction.length();
	s.duration = duration;
	s.strength = strength;
	shieldHits.push_front(s);
}

void Shield::Update(float t){
	//update shield timers
	for (std::list<ShieldType>::iterator it = shieldHits.begin(); it != shieldHits.end();){
		it->duration -= t / 1000.0f;
		if (it->duration < 0){
			it = shieldHits.erase(it);
		}
		else {
			++it;
		}
	}
}

void Shield::Render(float t, Vector shipPos, Quaternion shipRot){
	if (shieldHits.empty()){
		return;
	}
	//compute positions of shield parts
	
	for (int i = 0; i < 8; i++){
		Quaternion shieldRot = Quaternion(Vector(0,0,1), i*PI/2);
		if (i >= 4) shieldRot = shieldRot*Quaternion(Vector(1,0,0), PI);
		Vector finalScale = scale;
		
		
		if (i%2 == 1){
			float t = finalScale.x;
			finalScale.x = finalScale.y;
			finalScale.y = t;
		}
		
		shieldParts[i]->SetPosition(shipPos);
		shieldParts[i]->SetRotation(shipRot*shieldRot);
		shieldParts[i]->SetScale(finalScale);

		//set depth squared position
		Vector offset = 6*shipRot*shieldRot*Vector(ROOT2/2, 0.5, 0.5);
		shieldParts[i]->SetDepthSqPos(shipPos + offset);

		//setup outerfaceing shield
		shieldParts[i + 8]->SetPosition(shipPos);
		shieldParts[i + 8]->SetRotation(shipRot*shieldRot);
		shieldParts[i + 8]->SetScale(-1*finalScale);

		//set depth squared position slightly higher than normal piece
		offset = 6.05*shipRot*shieldRot*Vector(ROOT2/2, 0.5, 0.5);
		shieldParts[i + 8]->SetDepthSqPos(shipPos + offset);

	}
	

	//render hits on shield parts
	for (std::list<ShieldType>::iterator it = shieldHits.begin(); it != shieldHits.end(); ++it){
		for (int i = 0; i < 16; i++){
			
			float * params = new float[4];
			Vector dir = shipRot*it->direction;
			params[0] = dir.x;
			params[1] = dir.y;
			params[2] = dir.z;
			params[3] = it->strength*it->duration;
			g_graphics->RenderObject(shieldParts[i], SHADER_SHIELD, params);
		}
	}
}