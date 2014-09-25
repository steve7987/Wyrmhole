#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <vector>
#include <algorithm> 

#include "classlist.h"
#include "graphics.h"
#include "progressbar.h"

#define STARTCHILDSIZE 8
#define BORDERPADDING 6.0f
#define BORDERFILE L"./Assets/border.dds"

extern Graphics * g_graphics;

class Window{
public:
	Window();
	~Window();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, 
					int bitmapWidth, int bitmapHeight, float tx, float ty, float bx, float by, int idin, bool vis, 
					int xpos, int ypos, bool hasBorder, std::string text = "");  //tx... are texture coords for bitmap
	void Shutdown();
	void setVisible(bool visible);
	int Frame(int mouseX, int mouseY, Window ** windowList);  //returns -1 if nothing hit, or id of child if child hit, or this windows id
	bool Render(ID3D11DeviceContext* deviceContext, TextureShader * textureShader, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
				D3DXMATRIX orthoMatrix, Window** windowList);  //tells this window to render itself, then renders any children
	void addChild(int childId);
	void setProgress(float amount);  //should be between 0 and 1
	void removeChild(int childId);

private:
	int id;  //0 for root
	Bitmap * Image;
	int top, bottom, left, right;
	bool isVisible;
	
	std::vector<int> kids;	
	
	Bitmap * WindowBorder[8];  //clockwise from top left corner
	ProgressBar * Bar;

	int textId;
};

#endif