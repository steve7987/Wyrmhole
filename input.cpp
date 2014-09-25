#include "input.h"

Input::Input(){
	
}

Input::~Input(){

}

bool Input::Initialize(HWND hwnd, int screenWidth, int screenHeight){
	POINT p;
	//store screen dimensions
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = screenWidth / 2;
	m_mouseY = screenHeight / 2;

	p.x = m_mouseX;
	p.y = m_mouseY;

	ClientToScreen(hwnd, &p);

	SetCursorPos(p.x, p.y);

	for (int i = 0; i < 256; i++){
		m_keys[i] = false;
		m_keyPushed[i] = false;
	}

	for (int i = 0; i < 64; i++){
		m_mouse[i] = false;
		m_mousePushed[i] = false;
		m_mouseReleased[i] = false;
	}
	
	return true;
}

void Input::KeyDown(unsigned int key){
	m_keys[key] = true;
	m_keyPushed[key] = true;
}

void Input::KeyUp(unsigned int key){
	m_keys[key] = false;

}

	
void Input::Shutdown(){
	
}

void Input::MouseMoved(HWND hwnd, int xpos, int ypos){
	POINT p;
	p.x = xpos;
	p.y = ypos;
	if (GetCursorPos(&p)){
		ScreenToClient(hwnd, &p);		
		m_mouseX = p.x;
		m_mouseY = p.y;
	}
}

void Input::GetMouseLocation(int& mouseX, int& mouseY){
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}
	
bool Input::IsKeyDown(unsigned int key){
	return m_keys[key];
}

bool Input::KeyBeenPushed(unsigned int key){
	if (m_keyPushed[key]){
		m_keyPushed[key] = false;
		return true;
	}
	return false;
}

bool Input::IsMouseDown(unsigned int button){
	return m_mouse[button];
}
	
bool Input::MouseBeenPushed(unsigned int button){
	if (m_mousePushed[button]){
		m_mousePushed[button] = false;
		return true;
	}
	return false;
}
	
bool Input::MouseBeenReleased(unsigned int button){
	if (m_mouseReleased[button]){
		m_mouseReleased[button] = false;
		return true;
	}
	return false;
}

void Input::ResetInput(){
	for (int i = 0; i < 64; i++){
		m_mousePushed[i] = false;
		m_mouseReleased[i] = false;
	}
	for (int i = 0; i < 256; i++){
		m_keyPushed[i] = false;
	}
}

void Input::MouseButtonDown(unsigned int button){
	m_mouse[button] = true;
	m_mousePushed[button] = true;
}
	
void Input::MouseButtonUp(unsigned int button){
	m_mouse[button] = false;
	m_mouseReleased[button] = true;
}