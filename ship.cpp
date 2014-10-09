#include "ship.h"

Ship::Ship(){
	model = 0;
	stats = 0;
	engineBB = 0;
	track = 0;
	shield = 0;
}

Ship::~Ship(){

}

bool Ship::Initialize(char * datafile){
	if (!loadInfo(datafile)){
		return false;
	}

	boostVelocity = 0;
	boostOn = false;

	horizontalVelocity = Vector(0,0,0);
	horizontalAccel = Vector(0,0,0);
	horizontalOffset = Vector(0,0,0);
	horizontalOn = false;
	
	prevOffset = Vector (0,0,0);
	enginet = 0;

	position = Vector(0,0,0);
	heading = Vector(1, 0, 0);
	trackRotation = Quaternion(0, 0, 0, 1);
	turnRotation = Quaternion(0, 0, 0, 1);
	
	currentDistance = 0;
	return true;
}

void Ship::SetTrack(TrackManager * track){
	this->track = track;
}

void Ship::Shutdown(){
	if (model){
		model->Shutdown();
		delete model;
		model = 0;
	}
	if (shield){
		shield->Shutdown();
		delete shield;
		shield = 0;
	}
	if (engineBB){
		engineBB->Shutdown();
		delete engineBB;
		engineBB = 0;
	}
	if (stats){
		delete [] stats;
		stats = 0;
	}
}

bool Ship::Update(float t){
	shield->Update(t);
	currentDistance += stats[STAT_FORWARDV] * t / 1000.0f;
	//compute boost info
	if (boostOn){
		boostVelocity +=  stats[STAT_BOOSTA] * t / 1000.0f;
		if (boostVelocity > stats[STAT_BOOSTV]) boostVelocity = stats[STAT_BOOSTV];
	}
	else {
		boostVelocity -= stats[STAT_BOOSTA] * t / 1000.0f;
		if (boostVelocity < 0) boostVelocity = 0;
	}
	currentDistance += boostVelocity * t / 1000.0f;
	//compute horizontal offset info
	if (horizontalOn){
		horizontalVelocity = horizontalVelocity + horizontalAccel * t / 1000.0f;
		if (horizontalVelocity.length() > stats[STAT_HORIZONTALV]) {
			horizontalVelocity = horizontalVelocity / horizontalVelocity.length() * stats[STAT_HORIZONTALV];
		}
	}
	//decelerate horizontal v
	else if (horizontalVelocity*horizontalVelocity > 1e-6f){
		Vector decel = -1*horizontalVelocity/horizontalVelocity.length()*stats[STAT_HORIZONTALA];
		if (decel*decel*t*t/1000000.0f > horizontalVelocity*horizontalVelocity) {
			horizontalVelocity = Vector(0,0,0);
		}
		else {
			horizontalVelocity = horizontalVelocity + decel * t / 1000.0f;
		}
	}
	else {
		horizontalVelocity = Vector(0,0,0);
	}
	CheckCollisions(t);
	horizontalOffset = horizontalOffset + horizontalVelocity * t / 1000.0f;
	//debug for horizontal offset
	if ((prevOffset - horizontalOffset).length() > 2.0){
		std::stringstream oss;
		oss << "HO: " << horizontalOffset.x << ", " << horizontalOffset.y << ", " << horizontalOffset.z;
		oss << " HV: " << horizontalVelocity.x << ", " << horizontalVelocity.y << ", " << horizontalVelocity.z;
		
		textDump(oss.str());
	}
	prevOffset = horizontalOffset;
	//compute turnRotation based on horizonal velocity
	if (horizontalVelocity*horizontalVelocity > 1e-6f){
		Vector totalvel = Vector(stats[STAT_FORWARDV] + boostVelocity, 0, 0) + horizontalVelocity;
		totalvel = totalvel/totalvel.length();
		turnRotation = Quaternion(Vector(1,0,0), totalvel);
		
		Vector hvmag = horizontalVelocity / stats[STAT_HORIZONTALV];
		Quaternion roll = Quaternion(Vector(1, 0, 0), hvmag.z*stats[STAT_HORIZONTALROLL]);
		turnRotation = turnRotation*roll;
		
	}
	else {
		turnRotation = Quaternion(0, 0, 0, 1);
	}
	trackRotation = track->GetRotation(currentDistance);
	position = track->GetPoint(currentDistance);
	model->SetPosition(position + trackRotation*horizontalOffset);
	model->SetRotation(trackRotation*turnRotation);
		
	boostOn = false;
	horizontalOn = false;
	
	return true;
}

void Ship::DisplayUpdate(float t){
	shield->Update(t);
}

void Ship::UpdateEngine(float t){
	engineBB->SetPosition(position + trackRotation*(horizontalOffset) - trackRotation*turnRotation*Vector(2.5, 0, 0));
	enginet += t;
	//if (enginet > 2*PI) enginet -= 2*PI;
	float scale = 1.4f + 0.2f*sin(enginet*2*PI/75.0f);
	engineBB->SetScale(scale, scale);
}

void Ship::CheckCollisions(float t){
	//check if outside tube
	for (int i = 0; i < 8; i++){  //approximate ellipse with octagon
		float t = i*PI/4;
		Vector point = cos(t) * Vector(0, stats[STAT_SHIELDY], 0) + sin(t) * Vector(0, 0, stats[STAT_SHIELDZ]);
		point = turnRotation*point + horizontalOffset;
		if (point * point > track->GetTrackRadius()*track->GetTrackRadius()){
			//damage shields, adjust ship position??
			
		}
	}
	//check if collision with another object/projectile
	//get list of collidables for slightly before and after ship pos
	std::list<Collidable*>* collidables = track->GetCollidables(currentDistance-25);
	std::list<Collidable*>* collidables2 = track->GetCollidables(currentDistance+25);
	for (std::list<Collidable*>::iterator it = collidables->begin(); it != collidables->end(); ++it){
		if ((*it)->CollideWithEllipsoid(position + trackRotation*horizontalOffset, 
									Vector(stats[STAT_SHIELDX], stats[STAT_SHIELDY], stats[STAT_SHIELDZ]), 
									trackRotation, NULL))
		{
			//if shield collides with the object then do stuff
			//most should be done by the collidable
		}

	}
	if (collidables != collidables2){
		for (std::list<Collidable*>::iterator it = collidables2->begin(); it != collidables2->end(); ++it){
			if ((*it)->CollideWithEllipsoid(position + trackRotation*horizontalOffset, 
											Vector(stats[STAT_SHIELDX], stats[STAT_SHIELDY], stats[STAT_SHIELDZ]), 
											trackRotation, NULL))
			{
				//if shield collides with the object then do stuff
			}
		}
	}	
}

bool Ship::Render(float t){
	
	UpdateEngine(t);
	g_graphics->RenderObject(model, SHADER_LIGHT);
	g_graphics->RenderObject(engineBB, SHADER_TEXTURE);
	shield->Render(t, position + trackRotation*horizontalOffset, trackRotation*turnRotation);
	return true;
}

void Ship::SetDistance(float dist){
	currentDistance = dist;
	position = track->GetPoint(dist);
	trackRotation = track->GetRotation(dist);
	horizontalOffset = Vector(0,0,0);
	horizontalVelocity = Vector(0,0,0);
	turnRotation = Quaternion(0, 0, 0, 1);
	model->SetPosition(position);
	model->SetRotation(trackRotation);
}

Quaternion Ship::GetRotation(){
	return trackRotation * turnRotation;
}

float Ship::GetDistance(){
	return currentDistance;
}

Vector Ship::GetPosition(){
	return position + trackRotation*horizontalOffset;
}

void Ship::SetPosition(Vector v){
	position = v;
	horizontalOffset = Vector(0, 0, 0);
	horizontalVelocity = Vector(0,0,0);
	model->SetPosition(v);
	//shield->SetPosition(v);
}

void Ship::SetRotation(Quaternion q){
	trackRotation = q;
	turnRotation = Quaternion(0, 0, 0, 1);
	heading = q*Vector(1,0,0);
	model->SetRotation(q);
	//shield->SetRotation(q);
}

void Ship::Boost(){
	boostOn = true;
}

void Ship::HorizontalAcceleration(Vector accel){
	accel.x = 0;
	accel = accel/accel.length();
	accel = accel * stats[STAT_HORIZONTALA];
	horizontalAccel = accel;
	horizontalOn = true;
}


bool Ship::loadInfo(char * datafile){
	ifstream fin;
	char input;
	fin.open(datafile);
	if (fin.fail()){
		textDump("failed to open ship data file");
		return false;
	}
	for (fin.get(input); input != '"'; fin.get(input));
	//read ship model file
	char modfile[128];
	int j = 0;
	for (fin.get(input); input != '"'; fin.get(input)){
		modfile[j] = input;
		j++;
	}
	modfile[j] = '\0';
	//read ship texture file
	for (fin.get(input); input != '"'; fin.get(input));
	char texfile[128];
	j = 0;
	for (fin.get(input); input != '"'; fin.get(input)){
		texfile[j] = input;
		j++;
	}
	texfile[j] = '\0';
	WCHAR wtex[128];
	MultiByteToWideChar(CP_ACP, 0, texfile, -1,  wtex, 128);
	//read ship engine bb file
	for (fin.get(input); input != '"'; fin.get(input));
	j = 0;
	for (fin.get(input); input != '"'; fin.get(input)){
		texfile[j] = input;
		j++;
	}
	texfile[j] = '\0';
	WCHAR weng[128];
	MultiByteToWideChar(CP_ACP, 0, texfile, -1,  weng, 128);
	//read shield model data
	for (fin.get(input); input != '"'; fin.get(input));
	char shieldmodfile[128];
	j = 0;
	for (fin.get(input); input != '"'; fin.get(input)){
		shieldmodfile[j] = input;
		j++;
	}
	shieldmodfile[j] = '\0';
	//read shield texture file
	for (fin.get(input); input != '"'; fin.get(input));
	j = 0;
	for (fin.get(input); input != '"'; fin.get(input)){
		texfile[j] = input;
		j++;
	}
	texfile[j] = '\0';
	WCHAR wshield[128];
	MultiByteToWideChar(CP_ACP, 0, texfile, -1,  wshield, 128);
	//load other data now
	for (fin.get(input); input != ':'; fin.get(input));
	fin >> numStats;
	stats = new float[numStats];
	for (int i = 0; i < numStats; i++){
		for (fin.get(input); input != ':'; fin.get(input));
		fin >> stats[i];
	}
	fin.close();
	
	model = new Model();
	if (!model){
		textDump("could not create model");
		return false;
	}
	if (!model->Initialize(g_graphics->GetDevice(), modfile, wtex, false)){
		textDump("could not initialize model");
		return false;
	}
	
	engineBB = new Billboard();
	if (!engineBB) {
		textDump("could not create engine billboard");
		return false;
	}
	if (!engineBB->Initialize(g_graphics->GetDevice(), weng)){
		textDump("could not initialize engine billboard");
		return false;
	}
	//create shield
	shield = new Shield();
	if (!shield){
		textDump("could not create shield model");
		return false;
	}
	if (!shield->Initialize(shieldmodfile, wshield, Vector(stats[STAT_SHIELDX], stats[STAT_SHIELDY], stats[STAT_SHIELDZ]))){
		return false;
	}


	return true;
}

void Ship::DamageShield(float strength, Vector position){
	Vector offset = position - (Ship::position + trackRotation*horizontalOffset);
	offset = (trackRotation * turnRotation).inverse()*offset;
	shield->AddHit(offset, 0.25f, strength);
}

bool Ship::CollideWithPoint(Vector point, Shot * shot){
	return false;
}
	
bool Ship::CollideWithEllipsoid(Vector point, Vector scale, Quaternion rotation, Shot * shot){
	return false;
}
bool Ship::CollideWithLineSegment(Vector start, Vector end, Vector& collisionPoint, Shot * shot){
	return false;
}

bool Ship::CollideWithRay(Vector origin, Vector direction, Vector& collisionPoint, Shot * shot){
	//translate to center and scale to sphere of radius 1
	Vector scale = Vector(stats[STAT_SHIELDX], stats[STAT_SHIELDY], stats[STAT_SHIELDZ]);
	
	//Vector scale = Vector(1,1,1);
	origin = origin - position;
	origin.x = origin.x / scale.x;
	origin.y = origin.y / scale.y;
	origin.z = origin.z / scale.z;
	direction.x = direction.x / scale.x;
	direction.y = direction.y / scale.y;
	direction.z = direction.z / scale.z;
	direction = direction / direction.length();
	//compute discriminent for intersection test
	float discrim = (direction * origin) * (direction * origin) - (origin * origin - 1)*(direction * direction);

	if (discrim < 0){
		return false;
	}
	float t = (-1 * direction * origin - sqrt(discrim)) / (direction*direction);
	if (t < 0){
		return false;
	}
	collisionPoint = origin + t*direction;

	//translate point back
	collisionPoint.x = collisionPoint.x * scale.x;
	collisionPoint.y = collisionPoint.y * scale.y;
	collisionPoint.z = collisionPoint.z * scale.z;
	collisionPoint = collisionPoint + position;

	return true;
}