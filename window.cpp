#include "window.h"

Window::Window(){
	Image = 0;
	for (int i = 0; i < 8; i ++){
		WindowBorder[i] = 0;
	}
	Bar = 0;
}
	
Window::~Window(){

}

bool Window::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, 
					int bitmapWidth, int bitmapHeight, float tx, float ty, float bx, float by, int idin, bool vis, 
					int xpos, int ypos, bool hasBorder){
	//copy vars
	id = idin;
	top = ypos;
	bottom = ypos + bitmapHeight;
	left = xpos;
	right = xpos + bitmapWidth;
	isVisible = vis;
	
	if (id == 0){  //root window has no graphics
		return true;
	}
	if (textureFilename == NULL){  //window doesnt have image
		Image = 0;
		//textDump("no image");
		//return true; 
	}
	else {
		//create image
		Image = new Bitmap();
		//textDump("making window image");
		if (!Image->Initialize(device, screenWidth, screenHeight, textureFilename, (float) bitmapWidth, (float) bitmapHeight, (float) left, 
							   (float) top, tx, ty, bx, by)){
			return false;
		}
	}
	if (hasBorder){
		//make window border
		WindowBorder[0] = new Bitmap();
		WindowBorder[0]->Initialize(device, screenWidth, screenHeight, BORDERFILE, BORDERPADDING, BORDERPADDING, (float) left, (float) top, 
									0.0f, 0.0f, BORDERPADDING/100.0f, BORDERPADDING/100.0f, false);
		WindowBorder[1] = new Bitmap();
		WindowBorder[1]->Initialize(device, screenWidth, screenHeight, BORDERFILE, bitmapWidth - 2*BORDERPADDING, BORDERPADDING, (float) left + BORDERPADDING, (float) top,
									BORDERPADDING/100.0f, 0.0f, 1.0f - BORDERPADDING/100.0f, BORDERPADDING/100.0f, false);
		WindowBorder[2] = new Bitmap();
		WindowBorder[2]->Initialize(device, screenWidth, screenHeight, BORDERFILE, BORDERPADDING, BORDERPADDING, (float) right - BORDERPADDING, (float) top, 
									1 - BORDERPADDING/100, 0.0, 1.0, BORDERPADDING / 100, false);
		WindowBorder[3] = new Bitmap();
		WindowBorder[3]->Initialize(device, screenWidth, screenHeight, BORDERFILE, BORDERPADDING, bitmapHeight - 2*BORDERPADDING, (float) right - BORDERPADDING, (float) top + BORDERPADDING,
									1 - BORDERPADDING/100, BORDERPADDING/100, 1.0f,  1 - BORDERPADDING/100, false);
		WindowBorder[4] = new Bitmap();
		WindowBorder[4]->Initialize(device, screenWidth, screenHeight, BORDERFILE, BORDERPADDING, BORDERPADDING, (float) right - BORDERPADDING, (float) bottom - BORDERPADDING, 
									1 - BORDERPADDING/100, 1 - BORDERPADDING/100, 1.0f, 1.0f, false);
		WindowBorder[5] = new Bitmap();
		WindowBorder[5]->Initialize(device, screenWidth, screenHeight, BORDERFILE, bitmapWidth - 2*BORDERPADDING, BORDERPADDING, (float) left + BORDERPADDING, (float) bottom - BORDERPADDING,
									BORDERPADDING/100, 1 - BORDERPADDING/100, 1 - BORDERPADDING/100, 1.0f, false);
		WindowBorder[6] = new Bitmap();
		WindowBorder[6]->Initialize(device, screenWidth, screenHeight, BORDERFILE, BORDERPADDING, BORDERPADDING, (float) left, (float) bottom - BORDERPADDING, 
									0.0f, 1 - BORDERPADDING/100, BORDERPADDING/100, 1.0f, false);
		WindowBorder[7] = new Bitmap();
		WindowBorder[7]->Initialize(device, screenWidth, screenHeight, BORDERFILE, BORDERPADDING, bitmapHeight - 2 * BORDERPADDING, (float) left, (float) top + BORDERPADDING, 
									0.0f, BORDERPADDING/100, BORDERPADDING/100, 1 - BORDERPADDING/100, false);

	}
	//make progress bar
	Bar = new ProgressBar();
	Bar->Initialize(device, screenWidth, screenHeight, L"./pbar.dds", (float) bitmapWidth, (float) bitmapHeight, (float) xpos, (float) ypos);
	setProgress(0.0f);  //initially progress bar is empty aka invisible

	return true;
}

void Window::Shutdown(){
	//delete [] children;

	if (Bar != 0){
		Bar->Shutdown();
		delete Bar;
		Bar = 0;
	}

	if (Image != 0){
		Image->Shutdown();
		delete Image;
		Image = 0;
	}

	for (int i = 0; i < 8; i++){
		if (WindowBorder[i]){
			WindowBorder[i]->Shutdown();
			delete WindowBorder[i];
			WindowBorder[i] = 0;
		}
	}
}

void Window::setVisible(bool visible){
	isVisible = visible;
}

void Window::setProgress(float amount){
	Bar->setProgress(amount);
}

int Window::Frame(int mouseX, int mouseY, Window ** windowList){
	if (!isVisible){
		return -1;
	}
	
	//if mouse click out of this window return -1
	if (mouseX > right || mouseX < left || mouseY < top || mouseY > bottom){
		return -1;
	}
	//check children
	int retval = id;
	for (std::vector<int>::iterator it = kids.begin(); it != kids.end(); it++){
		if (windowList[*it]){
			int tmp = windowList[*it]->Frame(mouseX, mouseY, windowList);
			if (tmp != -1){
				retval = tmp;
			}
		}
	}
	return retval;
}

bool Window::Render(ID3D11DeviceContext* deviceContext, TextureShader * textureShader, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
					D3DXMATRIX orthoMatrix, Window** windowList)
{
	if (!isVisible){  //if window is hidden dont do anything
		return true;
	}

	//render window image
	if (Image != 0){
		if (!Image->Render(deviceContext)){
			return false;
		}
		if (!textureShader->Render(deviceContext, Image->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Image->GetTexture())){
			return false;
		}
	}

	//render progress bar
	if (Bar != 0){
		if (!Bar->Render(deviceContext)){
			return false;
		}
		if (!textureShader->Render(deviceContext, Bar->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, Bar->GetTexture())){
			return false;
		}
	}


	//render borders
	for (int i = 0; i < 8; i++){
		if (WindowBorder[i]){
			if (!WindowBorder[i]->Render(deviceContext)){
				return false;
			}
			if (!textureShader->Render(deviceContext, WindowBorder[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, WindowBorder[i]->GetTexture())){
				return false;
			}
		}
	}
	bool retval = true;
	for (std::vector<int>::iterator it = kids.begin(); it != kids.end(); it++){
		if (windowList[*it]){
			retval = retval && (windowList[*it])->Render(deviceContext, textureShader, worldMatrix, 
																viewMatrix, orthoMatrix, windowList);
		}
	}
	return retval;
}

void Window::addChild(int childId){
	kids.push_back(childId);
}

void Window::removeChild(int childId){
	kids.erase(std::remove(kids.begin(), kids.end(), childId), kids.end());
}