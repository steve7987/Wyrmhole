#include "testingstate.h"

TestingState::TestingState(){
	currentShip = 0;
	camera = 0;
}
	
TestingState::~TestingState(){

}

bool TestingState::Initialize(){
	mmSentence = g_graphics->RegisterSentence(64);
	g_graphics->ChangeSentence(mmSentence, "Testing State", 12, 38, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(mmSentence, false);
	g_gui->setVisible(GUIWINDOW_TESTING, false);
	//make camera
	camera = new Camera();
	if (!camera){
		textDump("failed to create track state camera");
		return false;
	}
	camera->SetPosition(0.0f, 5.0f, -25.0f);
	//initialize ships
	if (!LoadShips()){
		return false;
	}
	curshipIt = shipList.begin();
	currentShip = (*curshipIt);
	return true;
}

void TestingState::Shutdown(){
	if (camera){
		delete camera;
		camera = 0;
	}
	
	for (std::deque<Ship*>::iterator it = shipList.begin(); it != shipList.end(); ++it){
		(*it)->Shutdown();
		delete (*it);
		(*it) = 0;
	}
	shipList.clear();
	currentShip = 0;
}

bool TestingState::update(float t, Input * input){
	rotation += t / 1000.0f;
	//currentShip->SetRotation(Quaternion(Vector(0,1,0), rotation));
	currentShip->DisplayUpdate(t);
	//move camera
	if (input->IsKeyDown(0x41)){
		camera->SetPosition(Quaternion(Vector(0,1,0), t / 1000.f)*camera->GetPosition());
	}
	if (input->IsKeyDown(0x44)){
		camera->SetPosition(Quaternion(Vector(0,1,0), -1*t / 1000.f)*camera->GetPosition());
	}
	if (input->IsKeyDown(0x53)){
		camera->SetPosition(Quaternion(Vector(0,1,0).cross(camera->GetPosition()).normalize(), t / 1000.f)*camera->GetPosition());
	}
	if (input->IsKeyDown(0x57)){
		camera->SetPosition(Quaternion(Vector(0,1,0).cross(camera->GetPosition()).normalize(), -1*t / 1000.f)*camera->GetPosition());
	}
	if (input->KeyBeenPushed(VK_SPACE)){
		currentShip->DamageShield(1.0, Vector(1,0,0));
	}




	//handle state changes
	if (input->KeyBeenPushed(VK_ESCAPE)){
		g_gameStateManager->change("main menu");
		return true;
	}
	int mx, my;
	input->GetMouseLocation(mx, my);
	int window = g_gui->Frame(mx, my);
	if (input->MouseBeenPushed(MK_LBUTTON)){
		if (window == GUIWINDOW_TESTINGQUIT){
			g_gameStateManager->change("main menu");
			return true;
		}
	}
	return true;
}

void TestingState::render(float t){
	g_graphics->StartFrame(t, camera);
	currentShip->Render(t);
	g_graphics->EndFrame();
}

void TestingState::onExit(){
	g_graphics->SetVisibleSentence(mmSentence, false);
	g_gui->setVisible(GUIWINDOW_TESTING, false);
	currentLevel->SetShip(currentShip);
	currentLevel->SetScore(0);
}

void TestingState::onEnter(Level * l){
	//display gui info
	g_graphics->SetVisibleSentence(mmSentence, true);
	g_gui->setVisible(GUIWINDOW_TESTING, true);
	//setup current level and current ship
	currentLevel = l;
	currentLevel->SetShip(currentShip);
	//reset the current ship
	rotation = 0;
	currentShip->SetPosition(Vector(0,0,0));
	currentShip->SetRotation(Quaternion(0,0,0,1));
}

bool TestingState::LoadShips(){
	
	ifstream fin;
	char input;
	fin.open("./Assets/shiplist.txt");
	if (fin.fail()){
		textDump("failed to open ship list data file");
		return false;
	}
	for (fin.get(input); input != ':'; fin.get(input));
	int numShips;
	fin >> numShips;
	for (int i = 0; i < numShips; i++){
		for (fin.get(input); input != '"'; fin.get(input));
		//read ship data
		char datafile[128];
		int j = 0;
		for (fin.get(input); input != '"'; fin.get(input)){
			datafile[j] = input;
			j++;
		}
		datafile[j] = '\0';
		Ship * toadd = new Ship();
		if (!toadd){
			textDump("could not create ship");
			return false;
		}
		if (!toadd->Initialize(datafile)){
			textDump("could not initialize ship");
			return false;
		}
		shipList.push_back(toadd);
	}

	fin.close();
	
	return true;
}