#ifndef _PROGRESSBAR_H_
#define _PROGRESSBAR_H_


#include <D3D11.h>
#include <D3DX10math.h>
#include "classlist.h"
#include "texture.h"
#include "texturemanager.h"

extern TextureManager * g_textureManager;

class ProgressBar{
private:
	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	ProgressBar();
	~ProgressBar();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, 
					float bitmapWidth, float bitmapHeight, float posx, float posy);
	void Shutdown();
	void setProgress(float amount);  //input should be from 0 to 1

	bool Render(ID3D11DeviceContext* deviceContext); 
	int GetIndexCount(); //should be six
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext);  //change length of progress bar
	
	bool LoadTexture(ID3D11Device* device, WCHAR* filename);  //adjust for reading multiple images from same texture without reloading
	void ReleaseTexture();

	float m_top, m_left, m_height, m_length;
	float currentProgress;
	float previousProgress;
	int m_screenWidth, m_screenHeight;

	Texture * m_texture;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;


};

#endif