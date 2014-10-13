#ifndef _TRACKTUBE_H_
#define _TRACKTUBE_H_


#include <D3D11.h>
#include <D3DX10math.h>

#include "classlist.h"
#include "helpers.h"
#include "renderable.h"
#include "tracksegment.h"
#include "vector.h"
#include "texture.h"
#include "texturemanager.h"

#define NUMTUBESEGMENTS numTubeSegments //number of cylinders
#define TUBESIDES numTubeSides  //sides that make up the circle

extern TextureManager * g_textureManager;

class TrackTube : public Renderable{
private:
	struct VertexInfo{
		Vector position;
		Vector normal;
	};

	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	TrackTube();
	~TrackTube();
	
	bool Initialize(ID3D11Device * device, TrackSegment * segment, WCHAR * textureFilename, float radius, 
					float textureRepeat, float startdist, int tubesides, int tubesegments, float randomness, int smoothingPasses);
	void Shutdown();

	bool Render(ID3D11DeviceContext * deviceContext, Vector camlook);
	int GetIndexCount();
	D3DXMATRIX GetWorldMatrix();
	ID3D11ShaderResourceView* GetTexture();
	bool HasAlpha();
	float getDepthSq(Vector campos, Vector camlook);

private:
	bool InitializeBuffers(ID3D11Device * device, TrackSegment * segment, float repeat, float startdist);
	void RenderBuffers(ID3D11DeviceContext * deviceContext);

	void GetVertexInfo(TrackSegment * segment, float radius, float randomness, int smoothingPasses);

	ID3D11Buffer * m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	VertexInfo * vinfo;

	int numTubeSides;
	int numTubeSegments;

	Texture * m_texture;
};



#endif