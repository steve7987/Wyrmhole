#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "classlist.h"
#include "texture.h"
#include "helpers.h"

#define MAXTEXTURE 1024

struct TexType{
	WCHAR * filename;
	Texture * tex;
};

class TextureManager{
public:
	TextureManager();
	~TextureManager();

	bool Initialize();
	void Shutdown();
	Texture * GetTexture(ID3D11Device* device, WCHAR* filename, bool useMipMaps=true);
private:
	TexType * textureList;
	int highestTexture;
};

#endif