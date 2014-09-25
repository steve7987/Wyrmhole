#ifndef _INPUT_H_
#define _INPUT_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>


#include "helpers.h"

class Input{
public:
	Input();
	~Input();
	bool Initialize(HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();

	//bool Frame();

	void GetMouseLocation(int& mouseX, int& mouseY);
	bool IsKeyDown(unsigned int key);
	bool KeyBeenPushed(unsigned int key);  //checks if key has been pushed, and sets it back to false if it has been
	bool IsMouseDown(unsigned int button);
	bool MouseBeenPushed(unsigned int button); //checks if mouse button has been pushed, and sets it back to false if it has been
	bool MouseBeenReleased(unsigned int button);  //same as above for release

	void ResetInput();  //sets all mouse/key as though they have not been pressed

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	void MouseMoved(HWND hwnd, int xpos, int ypos);
	void MouseButtonDown(unsigned int button);
	void MouseButtonUp(unsigned int button);
	

private:
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	
	bool m_keyPushed[256];
	bool m_keys[256];
	bool m_mousePushed[64];
	bool m_mouse[64];
	bool m_mouseReleased[64];
};

#endif