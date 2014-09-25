#ifndef _TRACKLINE_H_
#define _TRACKLINE_H_


#include <D3D11.h>
#include <D3DX10math.h>

#include "classlist.h"
#include "helpers.h"
#include "renderable.h"
#include "tracksegment.h"
#include "vector.h"

#define NUMLINESEGMENTS 100

class TrackLine : public Renderable{
private:
	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	TrackLine();
	~TrackLine();
	
	bool Initialize(ID3D11Device * device, TrackSegment * segment);
	void Shutdown();

	bool Render(ID3D11DeviceContext * deviceContext, Vector camlook);
	int GetIndexCount();
	D3DXMATRIX GetWorldMatrix();
	ID3D11ShaderResourceView* GetTexture();

	bool HasAlpha();
	float getDepthSq(Vector campos, Vector camlook);

private:
	bool InitializeBuffers(ID3D11Device * device, TrackSegment * segment);
	void RenderBuffers(ID3D11DeviceContext * deviceContext);

	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};



#endif