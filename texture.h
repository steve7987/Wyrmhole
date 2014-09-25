#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <D3D11.h>
#include <D3DX11tex.h>
#include "classlist.h"
#include "helpers.h"

class Texture {
public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* device, WCHAR* filename, bool useMipMaps=true);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

#endif