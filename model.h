#ifndef _MODEL_H_
#define _MODEL_H_


#include <D3D11.h>
#include <D3DX10math.h>
#include "texture.h"
#include <fstream>

#include "classlist.h"
#include "helpers.h"
#include "texturemanager.h"
#include "modelmanager.h"
#include "renderable.h"
#include "vector.h"
#include "quaternion.h"


extern TextureManager * g_textureManager;
extern ModelManager * g_modelManager;

using namespace std;

class Model : public Renderable{
public:
	Model();
	~Model();
	
	bool Initialize(ID3D11Device * device, char* modelFilename, WCHAR* textureFilename, bool hasAlpha);
	void Shutdown();
	bool Render(ID3D11DeviceContext * deviceContext, Vector camlook);

	bool HasAlpha();
	float getDepthSq(Vector campos, Vector look);
	void GetBoundingSphere(float& radius, Vector& center);

	//these will set the transformation and update world matrix = scale*rot*trans
	void SetPosition(Vector v);
	void SetPosition(float x, float y, float z);
	void SetRotation(Quaternion q);
	void SetScale(float x, float y, float z);
	void SetScale(Vector s);
	//set world matrix to input and ignore scale, trans and rot variables (note: trans is still used for depthSQ)
	void SetWorldMatrix(D3DXMATRIX world);  
	void SetDepthSqPos(Vector pos);  //sets a position for the model to use for depthsq sorting, call after update pos

	int GetIndexCount();

	D3DXMATRIX GetWorldMatrix();

	ID3D11ShaderResourceView* GetTexture();

private:
	std::string modelFile;

	void UpdateWorldMatrix();

	float xpos, ypos, zpos;
	//float pitch, yaw, roll;
	Quaternion rotation;
	float xscale, yscale, zscale;
	D3DXMATRIX worldMatrix;

	bool hasAlpha;

	Texture * m_texture;
};

#endif