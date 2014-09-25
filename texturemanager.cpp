#include "texturemanager.h"

TextureManager::TextureManager(){
	textureList = 0;
}

TextureManager::~TextureManager(){

}

	
bool TextureManager::Initialize(){
	textureList = new TexType[MAXTEXTURE];
	if (!textureList){
		return false;
	}
	for (int i = 0; i < MAXTEXTURE; i++){
		textureList[i].filename = 0;
		textureList[i].tex = 0;
	}
	highestTexture = 0;
	return true;
}


void TextureManager::Shutdown(){
	if (textureList){
		for (int i = 0; i < MAXTEXTURE; i++){
			if (textureList[i].tex){
				delete textureList[i].filename;
				textureList[i].filename = 0;
				textureList[i].tex->Shutdown();
				delete textureList[i].tex;
				textureList[i].tex = 0;
			}
		}
	}
	delete [] textureList;
	textureList = 0;
}


Texture * TextureManager::GetTexture(ID3D11Device* device, WCHAR* filename, bool useMipMaps){
	int index;
	//look for texture name
	for (index = 0; index < highestTexture; index++){
		if (wcscmp(filename, textureList[index].filename) == 0){
			//found texture
			return textureList[index].tex;
		}
	}
	highestTexture++;
	//need to make it
	textureList[index].tex = new Texture();
	textureList[index].tex->Initialize(device, filename, useMipMaps);
	//textureList[index].filename = filename;
	textureList[index].filename = new WCHAR[128];
	wcscpy_s(textureList[index].filename, 128, filename);
	return textureList[index].tex;
}
