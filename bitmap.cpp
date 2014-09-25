#include "bitmap.h"

Bitmap::Bitmap(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_texture = 0;
}

Bitmap::~Bitmap(){
}

bool Bitmap::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, 
						float bitmapWidth, float bitmapHeight, float posx, float posy, float tx, float ty, float bx, float by, bool useMipMaps)
{
	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	angle = 0.0f;

	//store position
	xPosition = posx;
	yPosition = posy;

	//store texture coords for the bitmap
	uvTX = tx;
	uvTY = ty;
	uvBX = bx;
	uvBY = by;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	
	m_previousPosX = -1;
	m_previousPosY = -1;
	m_previousAngle = 0.0f;

	//make buffers
	if (!InitializeBuffers(device)){
		return false;
	}
	//textDump("image buffers initialized");
	//load texture
	if (!LoadTexture(device, textureFilename, useMipMaps)) {
		return false;
	}

	//textDump("image texture initialized");
	return true;
}

void Bitmap::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();
}

void Bitmap::setPosition(float x, float y){
	xPosition = x;
	yPosition = y;
}

void Bitmap::Rotate(float anglerad){
	angle = anglerad;
}

bool Bitmap::Render(ID3D11DeviceContext* deviceContext){
	if (!UpdateBuffers(deviceContext, xPosition, yPosition)){
		return false;
	}

	RenderBuffers(deviceContext);

	return true;
}

int Bitmap::GetIndexCount() {
	return m_indexCount;
}

ID3D11ShaderResourceView* Bitmap::GetTexture(){
	return m_texture->GetTexture();
}

bool Bitmap::InitializeBuffers(ID3D11Device* device){
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

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

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

void Bitmap::ShutdownBuffers(){
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

bool Bitmap::UpdateBuffers(ID3D11DeviceContext* deviceContext, float positionX, float positionY){
	//reposition the image if necessary
	float left, right, top, bottom;
	//float centerX, centerY;
	D3DXMATRIX rotation;
	D3DXVECTOR3 center;
	D3DXVECTOR4 temp;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if((positionX == m_previousPosX) && (positionY == m_previousPosY)  && (angle == m_previousAngle))
	{
		return true;
	}
	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;
	m_previousAngle = angle;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;
	//calculate rotation info
	center = D3DXVECTOR3((left + right) / 2, (top + bottom) / 2, 0.0f);
	D3DXMatrixRotationZ(&rotation, angle);


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	//adjust .texture to grab only part of the image
	// First triangle.
	D3DXVec3Transform(&temp, &(D3DXVECTOR3(left, top, 0.0f) - center), &rotation);
	vertices[0].position.x = temp.x + center.x;  // Top left.
	vertices[0].position.y = temp.y + center.y;
	vertices[0].position.z = temp.z + center.z;
	vertices[0].texture = D3DXVECTOR2(uvTX, uvTY);

	//vertices[1].position = rotation * (D3DXVECTOR3(right, bottom, 0.0f) - center) + center;  // Bottom right.
	D3DXVec3Transform(&temp, &(D3DXVECTOR3(right, bottom, 0.0f) - center), &rotation);
	vertices[1].position.x = temp.x + center.x;  // Top left.
	vertices[1].position.y = temp.y + center.y;
	vertices[1].position.z = temp.z + center.z;
	vertices[1].texture = D3DXVECTOR2(uvBX, uvBY);

	//vertices[2].position = rotation * (D3DXVECTOR3(left, bottom, 0.0f) - center) + center;  // Bottom left.
	D3DXVec3Transform(&temp, &(D3DXVECTOR3(left, bottom, 0.0f) - center), &rotation);
	vertices[2].position.x = temp.x + center.x;  // Top left.
	vertices[2].position.y = temp.y + center.y;
	vertices[2].position.z = temp.z + center.z;
	vertices[2].texture = D3DXVECTOR2(uvTX, uvBY);

	// Second triangle.
	//vertices[3].position = rotation * (D3DXVECTOR3(left, top, 0.0f) - center) + center;  // Top left.
	D3DXVec3Transform(&temp, &(D3DXVECTOR3(left, top, 0.0f) - center), &rotation);
	vertices[3].position.x = temp.x + center.x;  // Top left.
	vertices[3].position.y = temp.y + center.y;
	vertices[3].position.z = temp.z + center.z;
	vertices[3].texture = D3DXVECTOR2(uvTX, uvTY);

	//vertices[4].position = rotation * (D3DXVECTOR3(right, top, 0.0f) - center) + center;  // Top right.
	D3DXVec3Transform(&temp, &(D3DXVECTOR3(right, top, 0.0f) - center), &rotation);
	vertices[4].position.x = temp.x + center.x;  // Top left.
	vertices[4].position.y = temp.y + center.y;
	vertices[4].position.z = temp.z + center.z;
	vertices[4].texture = D3DXVECTOR2(uvBX, uvTY);

	//vertices[5].position = rotation * (D3DXVECTOR3(right, bottom, 0.0f) - center) + center;  // Bottom right.
	D3DXVec3Transform(&temp, &(D3DXVECTOR3(right, bottom, 0.0f) - center), &rotation);
	vertices[5].position.x = temp.x + center.x;  // Top left.
	vertices[5].position.y = temp.y + center.y;
	vertices[5].position.z = temp.z + center.z;
	vertices[5].texture = D3DXVECTOR2(uvBX, uvBY);

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

//setup vertex and index buffers on the gpu
void Bitmap::RenderBuffers(ID3D11DeviceContext* deviceContext){
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

bool Bitmap::LoadTexture(ID3D11Device* device, WCHAR* filename, bool useMipMaps){
	m_texture = g_textureManager->GetTexture(device, filename, useMipMaps);
	
	/*
	m_texture = new Texture;
	if(!m_texture)
	{
		return false;
	}
	//textDump("trying to load texture");
	if (!m_texture->Initialize(device, filename, useMipMaps)) {
		return false;
	}
	//textDump("loaded");
	*/
	return true;
}

void Bitmap::ReleaseTexture(){
	// Release the texture object.
	/*
	if(m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
	*/
}
