#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

#include <D3D11.h>
#include <D3DX10math.h>
#include "vector.h"



class Renderable{
public:
	virtual bool Render(ID3D11DeviceContext * deviceContext, Vector camlook) = 0;
	virtual int GetIndexCount() = 0;
	virtual D3DXMATRIX GetWorldMatrix() = 0;
	virtual ID3D11ShaderResourceView* GetTexture() = 0;

	virtual bool HasAlpha() = 0;
	virtual float getDepthSq(Vector campos, Vector look) = 0;
};

struct RenderableType{
	Renderable * m;
	int shader;
	float depthSq;
	float * parameters;
};

class CompareRenderableType{
public:
	bool operator() (RenderableType& r1, RenderableType& r2){
		return r1.depthSq < r2.depthSq;
	}
};

#endif