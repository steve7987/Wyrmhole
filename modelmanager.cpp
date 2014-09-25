#include "modelmanager.h"

ModelManager::ModelManager(){

}
	
ModelManager::~ModelManager(){

}


bool ModelManager::Initialize(){
	return true;
}

void ModelManager::Shutdown(){
	while (!modelMap.empty()){
		modelMap.begin()->second.indexBuffer->Release();
		modelMap.begin()->second.vertexBuffer->Release();
		modelMap.erase(modelMap.begin());
	}
}
	

bool ModelManager::AddModel(ID3D11Device * device, std::string modelFilename){
	if (modelMap.count(modelFilename) > 0){
		return true;
	}
	//load model file
	std::fstream fin;
	char input;

	//open file
	fin.open(modelFilename);
	if (fin.fail()){
		return false;
	}
	
	//read up to vertex count 
	for (fin.get(input); input != ':'; fin.get(input));
	int m_vertexCount, m_indexCount;
	//load number of vertices
	fin >> m_vertexCount;
	//set indices to same value
	m_indexCount = m_vertexCount;
	modelMap[modelFilename].indexCount = m_indexCount;
	//make model data array
	ModelType * m_model;
	m_model = new ModelType[m_vertexCount];
	if (!m_model){
		return false;
	}

	//read up to model data
	for (fin.get(input); input != ':'; fin.get(input));
	fin.get(input); //two returns after data:
	fin.get(input);

	//read vertex data
	for(int i=0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	//close the file
	fin.close();

	//load the buffers
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//make vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}
	//make index array
	indices = new unsigned long[m_indexCount];
	if (!indices){
		return false;
	}

	for (int i = 0; i < m_vertexCount; i++){
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	//setup description of static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//give it a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &modelMap[modelFilename].vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//setup description of static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//give it a pointer to the index buffer
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &modelMap[modelFilename].indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//now that buffers have been created, we can release the arrys
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	delete [] m_model;
	m_model = 0;

	return true;
}

bool ModelManager::RenderBuffers(ID3D11DeviceContext * deviceContext, std::string modelFilename){
	if (modelMap.count(modelFilename) > 0){
		//set vertex and index buffers as active, and define the render type
		unsigned int stride;
		unsigned int offset;
		//set vertex buffer stride and offset.
		stride = sizeof(VertexType); 
		offset = 0;

		//make vertex buffer active in input assembler
		deviceContext->IASetVertexBuffers(0, 1, &modelMap[modelFilename].vertexBuffer, &stride, &offset);

		//make index buffer active in input assembler
		deviceContext->IASetIndexBuffer(modelMap[modelFilename].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//define render type
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return true;
	}
	textDump("render buffer error");
	return false;
}

int ModelManager::GetIndexCount(std::string modelFilename){
	if (modelMap.count(modelFilename) > 0){
		return modelMap[modelFilename].indexCount;
	}
	textDump("index count error");
	return 0;
}