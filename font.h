#ifndef _FONT_H_
#define _FONT_H_

//class that holds resources for a specific font

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

#include "classlist.h"
#include "texture.h"
#include "helpers.h"
#include "texturemanager.h"

#define TAB_LENGTH 20

extern TextureManager * g_textureManager;

class Font{
private:
	struct FontType{
		float left, right;  //uv coords of char
		int size;  //width of char
	};
	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	Font();
	~Font();

	bool Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	int GetLength(const char* sentence);

	void BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY);
	//make array for a given sentence at given position

private:
	bool LoadFontData(char* filename);  //load format from textfile
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void ReleaseTexture();

	FontType * m_Font;
	Texture * m_Texture;

};


#endif