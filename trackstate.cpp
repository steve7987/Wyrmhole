#include "trackstate.h"

TrackState::TrackState(){
	camera = 0;
	springcam = 0;
	heroShip = 0;
	track = 0;
	tunnelLight = 0;
	level = 0;
}

TrackState::~TrackState(){

}


bool TrackState::Initialize(){
	//setup some debug info
	tSentence = g_graphics->RegisterSentence(64);
	g_graphics->ChangeSentence(tSentence, "Track Game State", 12, 38, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(tSentence, false);
	distSentence = g_graphics->RegisterSentence(128);
	g_graphics->ChangeSentence(distSentence, "Current distance: 0", g_screenWidth - 12 - g_graphics->GetSentenceLength("Current distance: 0"), 12, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(distSentence, false);
	
	//set score up
	curscore = 0;
	scoreSentence = g_graphics->RegisterSentence(64);
	g_graphics->ChangeSentence(scoreSentence, "Score: 0", 12, 40, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(scoreSentence, false);

	//setup light
	tunnelLight = new Light();
	if (!tunnelLight){
		textDump("failed to create tunnel light");
		return false;
	}
	tunnelLight->SetDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
	tunnelLight->SetDirection(0.0f, 0.1f, 0.0f);
	tunnelLight->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);

	

	//setup cameras
	camera = new Camera();
	if (!camera){
		textDump("failed to create track state camera");
		return false;
	}
	camera->SetPosition(0.0f, 0.0f, -25.0f);
	
	
	return true;
}

void TrackState::Shutdown(){
	if(track){
		track->Shutdown();
		delete track;
		track = 0;
	}
	if (camera){
		delete camera;
		camera = 0;
	}

	if (springcam){
		delete springcam;
		springcam = 0;
	}
	
	if (tunnelLight){
		delete tunnelLight;
		tunnelLight = 0;
	}
}

	
bool TrackState::update(float t, Input * input){
	HandleInput(input);

	heroShip->Update(t);
	curdist = heroShip->GetDistance();
	UpdateCamera(t, input);
	track->Update(curdist, t);
	tunnelLight->SetDirection(-1*track->GetTangent(curdist));
	//change display info if needed
	if ((int) curdist != prevdist){
		prevdist = (int) curdist;
		std::ostringstream oss;
		oss << "Current Distance: " << (int) curdist;
		g_graphics->ChangeSentence(distSentence, oss.str(), g_screenWidth - 12 - g_graphics->GetSentenceLength(oss.str()), 12, 1.0f, 1.0f, 1.0f);
	}
	if (curscore != level->GetScore()){
		curscore = level->GetScore();
		std::ostringstream oss;
		oss << "Score: " << curscore;
		g_graphics->ChangeSentence(scoreSentence, oss.str(), g_screenWidth - 12 - g_graphics->GetSentenceLength(oss.str()), 25, 1.0f, 1.0f, 1.0f);
	}

	return true;
}

void TrackState::HandleInput(Input * input){
	if (input->KeyBeenPushed(VK_ESCAPE)){
		g_gameStateManager->change("selection");
	}
	if (input->IsKeyDown(VK_SPACE)){  //boost
		heroShip->Boost();
	}
	if (input->IsKeyDown(0x57) || input->IsKeyDown(0x41) || input->IsKeyDown(0x53) || input->IsKeyDown(0x44)){  //wasd for horizontal movement
		Vector vel = Vector(0,0,0);
		if (input->IsKeyDown(0x57)) vel = vel + Vector(0,1,0);
		if (input->IsKeyDown(0x41)) vel = vel + Vector(0,0,1);
		if (input->IsKeyDown(0x53)) vel = vel + Vector(0,-1,0);
		if (input->IsKeyDown(0x44)) vel = vel + Vector(0,0,-1);
		if (vel != Vector(0,0,0)){
			heroShip->HorizontalAcceleration(vel);
		}
	}
}

void TrackState::render(float t){
	g_graphics->StartFrame(t, activecam, tunnelLight);
	heroShip->Render(t);
	track->Render(curdist);

	g_graphics->EndFrame();
}

void TrackState::onExit(){
	g_graphics->SetVisibleSentence(tSentence, false);
	g_graphics->SetVisibleSentence(distSentence, false);
	g_graphics->SetVisibleSentence(scoreSentence, false);
	if(track){
		track->Shutdown();
		delete track;
		track = 0;
	}
	if (springcam){
		delete springcam;
		springcam = 0;
	}
}

void TrackState::onEnter(Level * l){
	level = l;	
	//setup track
	track = new TrackManager();
	if (!track){
		textDump("error creating track");
		return;
	} 
	if (!track->Initialize((int) time(NULL), "./Assets/tubedata.txt")){
		textDump("error initializing track");
		return;
	}
	//setup camera
	springcam = new SpringCamera();
	if (!springcam){
		textDump("failed to create track state spring camera");
		return;
	}
	if (!springcam->Initialize("./Assets/cameradata.txt", track)){
		textDump("failed to initialize spring camera");
		return;
	}

	//setup ship
	heroShip = level->GetShip();
	heroShip->SetTrack(track);

	curdist = 0;
	prevdist = 0;
	heroShip->SetDistance(0);

	activecam = springcam;
	camera->SetPosition(track->GetPoint(curdist - 50.0f));
	camera->SetTarget(heroShip->GetPosition());
	springcam->Reset();
	
	tunnelLight->SetDirection(track->GetTangent(200));

	g_graphics->ChangeSentence(distSentence, "Current distance: 0", g_screenWidth - 12 - g_graphics->GetSentenceLength("Current distance: 0"), 12, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(distSentence, true);
	g_graphics->SetVisibleSentence(tSentence, true);
	//set score
	curscore = 0;
	g_graphics->ChangeSentence(scoreSentence, "Score: 0", g_screenWidth - 12 - g_graphics->GetSentenceLength("Score: 0"), 25, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(scoreSentence, true);
	
}


void TrackState::UpdateCamera(float t, Input * input){
	if (input->KeyBeenPushed(0x31)){
		activecam = springcam;
	}
	if (input->KeyBeenPushed(0x32)){
		activecam = camera;
	}
	camera->SetPosition(track->GetPoint(curdist - 50.0f));
	camera->SetTarget(heroShip->GetPosition());
	//camera->SetPosition(track->GetPoint(curdist) + 500 *Vector(0,1,0).cross(track->GetTangent(curdist)) / Vector(0,1,0).cross(track->GetTangent(curdist)).length());
	//camera->SetTarget(track->GetPoint(curdist));

	springcam->Update(heroShip->GetPosition(), curdist);
}