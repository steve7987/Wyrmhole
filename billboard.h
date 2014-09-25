#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <D3D11.h>
#include <D3DX10math.h>
#include "classlist.h"
#include "texture.h"
#include "renderable.h"
#include "helpers.h"
#include "texturemanager.h"
#include "vector.h"
#include "quaternion.h"

extern TextureManager * g_textureManager;

class Billboard : public Renderable{
private:
	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	Billboard();
	~Billboard();
	
	bool Initialize(ID3D11Device * device, WCHAR* textureFilename);
	void Shutdown();
	
	bool Render(ID3D11DeviceContext * deviceContext, Vector camlook);
	int GetIndexCount();
	D3DXMATRIX GetWorldMatrix();
	ID3D11ShaderResourceView* GetTexture();
	bool HasAlpha();
	float getDepthSq(Vector campos, Vector look);
	
	void SetPosition(Vector v);
	void SetScale(float x, float y);

private:
	bool InitializeBuffers(ID3D11Device * device);
	void RenderBuffers(ID3D11DeviceContext * deviceContext);
	
	void UpdateWorldMatrix();

	Vector position;
	Quaternion rotation;
	float xscale, yscale;
	D3DXMATRIX worldMatrix;

	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture * m_texture;

};


#endif