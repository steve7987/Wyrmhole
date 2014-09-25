#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <D3D11.h>
#include <D3DX10math.h>
#include <fstream>

#include "classlist.h"
#include "texture.h"
#include "helpers.h"

#include <string.h>
#include <map>

class ModelManager{
private:
	struct ModelBuffers{
		ID3D11Buffer * vertexBuffer;
		ID3D11Buffer * indexBuffer;
		int indexCount;
	};
	//vertex type, needs to match same type in shader class
	struct VertexType{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
	//struct for vertex info loaded from file
	struct ModelType{
		float x, y, z;		//position
		float tu, tv;		//texture coords 
		float nx, ny, nz;	//normal
	};

public:
	ModelManager();
	~ModelManager();

	bool Initialize();
	void Shutdown();
	
	bool AddModel(ID3D11Device * device, std::string modelFilename);
	bool RenderBuffers(ID3D11DeviceContext * deviceContext, std::string modelFilename);
	int GetIndexCount(std::string modelFilename);

private:
	bool InitializeBuffers(ID3D11Device * device);
	bool LoadModel(std::string modelFilename);

	std::map<std::string, ModelBuffers> modelMap;
};

#endif