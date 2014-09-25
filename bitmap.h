#ifndef _BITMAP_H_
#define _BITMAP_H_


#include <D3D11.h>
#include <D3DX10math.h>

#include "classlist.h"
#include "texture.h"
#include "helpers.h"
#include "texturemanager.h"

extern TextureManager * g_textureManager;

class Bitmap{
private:
	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	Bitmap();
	~Bitmap();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, 
					float bitmapWidth, float bitmapHeight, float posx, float posy, float tx, float ty, float bx, float by, bool useMipMaps=true);  
					//last 4 floats are uv coords for image use mipmaps should only be false for gui windows
	void Shutdown();
	void setPosition(float x, float y);

	bool Render(ID3D11DeviceContext* deviceContext); 

	void Rotate(float anglerad);

	int GetIndexCount(); //almost always six
	ID3D11ShaderResourceView* GetTexture();
private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, float positionX, float positionY);  //move position of the image
	
	bool LoadTexture(ID3D11Device* device, WCHAR* filename, bool useMipMaps);  //adjust for reading multiple images from same texture without reloading
	void ReleaseTexture();

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture * m_texture;

	int m_screenWidth, m_screenHeight;
	float m_bitmapWidth, m_bitmapHeight;
	float m_previousPosX, m_previousPosY;
	float m_previousAngle;
	float uvTX, uvTY, uvBX, uvBY;
	float xPosition, yPosition;
	float angle;  //angle of rotation in radians
	
};

#endif