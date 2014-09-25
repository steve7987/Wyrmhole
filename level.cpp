#include "level.h"

Level::Level(){
	currentShip = 0;
}

Level::~Level(){

}

bool Level::Initialize(){
	score = 0;
	return true;
}

void Level::Shutdown(){

}

Ship * Level::GetShip(){
	return currentShip;
}

void Level::SetShip(Ship * s){
	currentShip = s;
}

void Level::SetScore(int s){
	score = s;
}

int Level::GetScore(){
	return score;
}