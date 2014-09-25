#include "gamestatemanager.h"

GameStateManager::GameStateManager(){
	m_currentState = 0;
	m_nextState = 0;
	currentLevel = 0;
}

GameStateManager::~GameStateManager(){
	
}


bool GameStateManager::Initialize(Input * input){
	pmouseX = 0;
	pmouseY = 0;
	pfps = 0;
	fpsSentence = g_graphics->RegisterSentence(128);	
	mouseSentence = g_graphics->RegisterSentence(128);
	this->input = input;
	currentLevel = new Level();
	g_level = currentLevel;
	if (!currentLevel){
		textDump("unable to create level");
		return false;
	}
	if (!currentLevel->Initialize()){
		textDump("unable to initialize current level");
		return false;
	}
	return true;
}

void GameStateManager::Shutdown(){
	if (currentLevel){
		currentLevel->Shutdown();
		delete currentLevel;
		currentLevel = 0;
	}
	//should delete all game states
	while (!mStates.empty()){
		mStates.begin()->second->Shutdown();
		delete mStates.begin()->second;
		mStates.erase(mStates.begin());
	}
}


bool GameStateManager::update(float t, Fps * fps){
	if (m_nextState != 0){
		if (m_currentState){
			m_currentState->onExit();
		}
		m_currentState = m_nextState;
		m_currentState->onEnter(currentLevel);
		m_nextState = 0;
		input->ResetInput();
		textDump("game state changed");
	}
	if (!m_currentState){
		textDump("no current state... exiting");
		return false;
	}
	//show some debug info (mouse position and fps)
	int mouseX, mouseY;
	input->GetMouseLocation(mouseX, mouseY);
	if (mouseX != pmouseX || mouseY != pmouseY){
		pmouseX = mouseX;
		pmouseY = mouseY;
		//update mouse position sentence
		std::ostringstream oss;
		oss << "Mouse: " << mouseX << ", " << mouseY;
		g_graphics->ChangeSentence(mouseSentence, oss.str(), 12, 12, 1.0f, 1.0f, 1.0f);
	}
	if (pfps != fps->GetFps()){
		pfps = fps->GetFps();
		std::ostringstream oss;
		oss << "Fps: " << fps->GetFps();
		g_graphics->ChangeSentence(fpsSentence, oss.str(), 12, 25, 1.0f, 1.0f, 1.0f);
	}

	return m_currentState->update(t, input);
}

void GameStateManager::render(float t){
	m_currentState->render(t);
}


void GameStateManager::add(std::string name, GameState * toAdd){
	mStates[name] = toAdd;
}

void GameStateManager::change(std::string name){
	if (mStates.find(name) == mStates.end()){
		textDump("Unable to find gamestate named: " + name);
		return;
	}
	textDump("Next state is: " + name);
	m_nextState = mStates[name];
}
