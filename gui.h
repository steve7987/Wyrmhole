#ifndef _GUI_H_
#define _GUI_H_

#include "classlist.h"
#include "graphics.h"
#include "window.h"

class Graphics;
class Window;

#define GUIWINDOW_MAINMENU 1
#define GUIWINDOW_MAINMENUTESTING 2
#define GUIWINDOW_MAINMENUSTART 3
#define GUIWINDOW_MAINMENUQUIT 4
#define GUIWINDOW_SELECTION 5
#define GUIWINDOW_SELECTIONSTART 6
#define GUIWINDOW_SELECTIONQUIT 7
#define GUIWINDOW_TESTING 8
#define GUIWINDOW_TESTINGQUIT 9


class Gui{
private:
	struct window{
		int sentenceIndex;
		int imageIndex;
		int top, bottom, left, right;
	};

public:
	Gui();
	~Gui();
	//reads gui file and creates windows based on that file
	bool Initialize(Graphics * graphics, char * guiFilename, int sWidth, int sHeight, ID3D11Device* device);  
	

	int Frame(int mouseX, int mouseY);  //takes mouse x,y and returns window id that mouse is over or -1
	void setVisible(int windowID, bool visible);  //sets window and its children visible
	void setProgressBar(int windowID, float amount);  //amount should be between 0 and 1
	bool Render(ID3D11DeviceContext* deviceContext, TextureShader * textureShader, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
				D3DXMATRIX orthoMatrix);  //renders all the GUI windows
	int AddWindow(WCHAR* textureFilename, int parent, int bitmapWidth, int bitmapHeight, float tx, float ty, float bx, float by, bool vis, 
					int xpos, int ypos, bool hasBorder);  //returns id of new window
	void RemoveWindow(int id, int parentid);  //removes window but not children (although children will never be rendered if parent removed)

	void Shutdown();

private:
	bool loadFile(char * filename, ID3D11Device* device);
	
	Graphics * m_graphics;
	int numWindows;
	int screenWidth;
	int screenHeight;
	Window ** windowArray;
	int winArrayLength;

};


#endif