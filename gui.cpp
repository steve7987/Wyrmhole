#include "gui.h"

Gui::Gui(){
	m_graphics = 0;
	//windowList = 0;
	windowArray = 0;
}

Gui::~Gui(){

}

bool Gui::Initialize(Graphics * graphics, char * guiFilename, int sWidth, int sHeight, ID3D11Device* device){
	m_graphics = graphics;
	screenWidth = sWidth;
	screenHeight = sHeight;
	//load info from file
	//textDump("loading from file");
	if (!loadFile(guiFilename, device)){
		return false;
	}
	return true;
}

int Gui::Frame(int mouseX, int mouseY){
	//go through window list and check where the press was
	return windowArray[0]->Frame(mouseX, mouseY, windowArray);
	
}

void Gui::setVisible(int windowID, bool visible){
	//windowList[windowID].isVisible = visible;
	//also set bitmap to invisible
	//m_graphics->SetVisibleBitmap(windowList[windowID].imageIndex, visible);
	windowArray[windowID]->setVisible(visible);
}

void Gui::setProgressBar(int windowID, float amount){
	windowArray[windowID]->setProgress(amount);
}

void Gui::Shutdown(){
	//delete window list
	if (windowArray){
		for (int i = 0; i < winArrayLength; i++){
			if (windowArray[i]){
				windowArray[i]->Shutdown();
				delete windowArray[i];
				windowArray[i] = 0;
			}
		}
		delete [] windowArray;
		windowArray = 0;
	}
}

bool Gui::loadFile(char * filename, ID3D11Device* device){
	ifstream fin;
	char input;
	float input2;
	char texfile[128];

	//open file
	fin.open(filename);
	if (fin.fail()){
		return false;
	}
	//read up to num items
	for (fin.get(input); input != ':'; fin.get(input));
	//load total number of windows
	fin >> numWindows;
	numWindows++;  //for root
	winArrayLength = numWindows + 10;  //allows for 10 additional windows to be made
	windowArray = new Window*[winArrayLength];
	for (int i = 0; i < winArrayLength; i++){
		windowArray[i] = 0;
	}
	//add root window
	windowArray[0] = new Window();
	//textDump("making root window");
	if (!windowArray[0]->Initialize(device, screenWidth, screenWidth, NULL, screenWidth, screenHeight, 0, 0, 0, 0, 0, true, 0, 0, false)){
		return false;
	}
	//textDump("done with root making other windows");
	//now load rest of windows
	for (int i = 1; i < numWindows; i++){
		//read up to first value after :
		for (fin.get(input); input != ':'; fin.get(input));
		//compute variables for window init
		int top, bot, left, right, parent;
		bool border;
		fin >> parent;
		fin >> input2;
		left = (int) (input2 * screenWidth);
		fin >> input2;
		right = (int) (input2 * screenWidth);
		fin >> input2;
		top = (int) (input2 * screenHeight);
		fin >> input2;
		bot = (int) (input2 * screenHeight);
		fin >> border;
		//border = input2;
		//get texture name
		//read up to "
		for (fin.get(input); input != '"'; fin.get(input));
		int j = 0;
		//read string into texfile
		for (fin.get(input); input != '"'; fin.get(input)){
			texfile[j] = input;
			j++;
		}
		texfile[j] = '\0';
		//now create the texture and the image
		if (j == 0){ //no background image
			windowArray[i] = new Window();
			if (!windowArray[i]->Initialize(device, screenWidth, screenHeight, NULL, right - left, bot - top, 0, 0, 1, 1, i, 
											false, left, top, border))
			{
				return false;
			}
		}
		else {
			WCHAR wtexfile[128];
			MultiByteToWideChar(CP_ACP, 0, texfile, -1,  wtexfile, 128);
			//create window
			windowArray[i] = new Window();
			if (!windowArray[i]->Initialize(device, screenWidth, screenHeight, wtexfile, right - left, bot - top, 0, 0, 1, 1, i, 
											true, left, top, border))
			{
				return false;
			}
		}
		//finally need to add kids to parent window
		windowArray[parent]->addChild(i);
	}
	fin.close();

	return true;
}

bool Gui::Render(ID3D11DeviceContext* deviceContext, TextureShader * textureShader, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
				 D3DXMATRIX orthoMatrix){
	//just call render on root window
	return windowArray[0]->Render(deviceContext, textureShader, worldMatrix, viewMatrix, orthoMatrix, windowArray);
}

int Gui::AddWindow(WCHAR* textureFilename, int parent, int bitmapWidth, int bitmapHeight, float tx, float ty, float bx, float by, bool vis, 
					int xpos, int ypos, bool hasBorder)
{
	int index;
	for (index = 0; index < winArrayLength; index++){
		if (windowArray[index] == 0){
			break;
		}
	}
	if (index == winArrayLength){
		textDump("Gui window array out of room");
		return 0;
	}
	windowArray[index] = new Window();
	windowArray[index]->Initialize(m_graphics->GetDevice(), screenWidth, screenHeight, textureFilename, bitmapWidth, bitmapHeight, tx, ty, bx, by, index, true, xpos, ypos, hasBorder);
	windowArray[parent]->addChild(index);
	return index;
}

void Gui::RemoveWindow(int id, int parentid){
	if (windowArray[id]){
		windowArray[parentid]->removeChild(id);
		windowArray[id]->Shutdown();
		delete windowArray[id];
		windowArray[id] = 0;
	}
}