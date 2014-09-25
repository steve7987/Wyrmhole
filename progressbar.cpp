#include "progressbar.h"

ProgressBar::ProgressBar(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_texture = 0;
}

ProgressBar::~ProgressBar(){

}


bool ProgressBar::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, 
					float bitmapWidth, float bitmapHeight, float posx, float posy){
	m_top = posy;
	m_left = posx;
	m_height = bitmapHeight;
	m_length = bitmapWidth;
	currentProgress = 1;
	previousProgress = 1;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	if (!InitializeBuffers(device)){
		return false;
	}
	if (!LoadTexture(device, textureFilename)) {
		return false;
	}
	return true;
}

void ProgressBar::Shutdown(){
	ReleaseTexture();
	ShutdownBuffers();
}

void ProgressBar::setProgress(float amount){
	//if change less than 1 pixel return
	if ((amount - currentProgress)*m_length < 1.0f && (amount - currentProgress)*m_length > -1.0f){
		return;
	}
	if (amount < 0){
		amount = 0;
	}
	if (amount > 1){
		amount = 1;
	}

	previousProgress = currentProgress;
	currentProgress = amount;
	
}


bool ProgressBar::Render(ID3D11DeviceContext* deviceContext){
	if (currentProgress != previousProgress){
		UpdateBuffers(deviceContext);
		previousProgress = currentProgress;
	}
	RenderBuffers(deviceContext);
	return true;
}


int ProgressBar::GetIndexCount(){
	return m_indexCount;
}

ID3D11ShaderResourceView* ProgressBar::GetTexture(){
	return m_texture->GetTexture();
}

bool ProgressBar::InitializeBuffers(ID3D11Device* device){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 6; //making a square
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}
	// Calculate the screen coordinates of the left side of the bitmap.
	float left = (float)((m_screenWidth / 2) * -1) + m_left;
	float right = left + m_length;
	float top = (float)(m_screenHeight / 2) - m_top;
	float bottom = top - m_height;


	//set vertex array values
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
	//tri 2
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);
	// Load the index array with data.
	for(int i=0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	// can be static since number of vertices doesnt change only their positions
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void ProgressBar::ShutdownBuffers(){
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

	
void ProgressBar::RenderBuffers(ID3D11DeviceContext* deviceContext){
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ProgressBar::UpdateBuffers(ID3D11DeviceContext* deviceContext){
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;
	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}
	// Calculate the screen coordinates of the left side of the bitmap.
	float left = (float)((m_screenWidth / 2) * -1) + m_left;
	float right = left + m_length*currentProgress;
	float top = (float)(m_screenHeight / 2) - m_top;
	float bottom = top - m_height;


	//set vertex array values
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[1].texture = D3DXVECTOR2(currentProgress, 1.0f);
	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
	//tri 2
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	vertices[4].texture = D3DXVECTOR2(currentProgress, 0.0f);
	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[5].texture = D3DXVECTOR2(currentProgress, 1.0f);
	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	//copy into vertex buffer
	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

bool ProgressBar::LoadTexture(ID3D11Device* device, WCHAR* filename){
	m_texture = g_textureManager->GetTexture(device, filename);
	/*m_texture = new Texture;
	if(!m_texture)
	{
		return false;
	}
	//textDump("trying to load texture");
	if (!m_texture->Initialize(device, filename)) {
		return false;
	}
	//textDump("loaded");
	*/
	return true;
}

void ProgressBar::ReleaseTexture(){
	/*
	// Release the texture object.
	if(m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}*/
}
	