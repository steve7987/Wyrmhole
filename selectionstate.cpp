#include "selectionstate.h"

SelectionState::SelectionState(){
	currentShip = 0;
	camera = 0;
}
	
SelectionState::~SelectionState(){

}

bool SelectionState::Initialize(){
	mmSentence = g_graphics->RegisterSentence(64);
	g_graphics->ChangeSentence(mmSentence, "Selection State", 12, 38, 1.0f, 1.0f, 1.0f);
	g_graphics->SetVisibleSentence(mmSentence, false);
	g_gui->setVisible(GUIWINDOW_SELECTION, false);
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

void SelectionState::Shutdown(){
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

bool SelectionState::update(float t, Input * input){
	rotation += t / 1000.0f;
	currentShip->SetRotation(Quaternion(Vector(0,1,0), rotation));
	//change current ship
	if (input->KeyBeenPushed(0x41)){
		if (curshipIt == shipList.begin()){
			curshipIt = shipList.end();
		}
		--curshipIt;
		currentShip = *curshipIt;
	}
	if (input->KeyBeenPushed(0x44)){
		++curshipIt;
		if (curshipIt == shipList.end()){
			curshipIt = shipList.begin();
		}
		currentShip = *curshipIt;
	}
	if (input->KeyBeenPushed(VK_SPACE)){
		currentShip->Boost();
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
		if (window == GUIWINDOW_SELECTIONSTART){
			g_gameStateManager->change("track");
		}
		else if (window == GUIWINDOW_SELECTIONQUIT){
			g_gameStateManager->change("main menu");
			return true;
		}
	}
	return true;
}

void SelectionState::render(float t){
	g_graphics->StartFrame(t, camera);
	currentShip->Render(t);
	g_graphics->EndFrame();
}

void SelectionState::onExit(){
	g_graphics->SetVisibleSentence(mmSentence, false);
	g_gui->setVisible(GUIWINDOW_SELECTION, false);
	currentLevel->SetShip(currentShip);
	currentLevel->SetScore(0);
}

void SelectionState::onEnter(Level * l){
	//display gui info
	g_graphics->SetVisibleSentence(mmSentence, true);
	g_gui->setVisible(GUIWINDOW_SELECTION, true);
	//setup current level and current ship
	currentLevel = l;
	currentLevel->SetShip(currentShip);
	//reset the current ship
	rotation = 0;
	currentShip->SetPosition(Vector(0,0,0));
	currentShip->SetRotation(Quaternion(0,0,0,1));
}

bool SelectionState::LoadShips(){
	
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