#include "billboard.h"

Billboard::Billboard(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_texture = 0;
}
	
Billboard::~Billboard(){

}
		
bool Billboard::Initialize(ID3D11Device * device, WCHAR* textureFilename){
	//initialize vertex and index buffers for the model
	if (!InitializeBuffers(device)){
		return false;
	}
	//load the texture
	m_texture = g_textureManager->GetTexture(device, textureFilename);
	position = Vector(0, 0, 0);
	rotation = Quaternion(0,0,0,1);
	xscale = 1;
	yscale = 1;
	UpdateWorldMatrix();

	return true;
}

void Billboard::Shutdown(){
	//release index buffer
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//release vertex buffer
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

bool Billboard::Render(ID3D11DeviceContext * deviceContext, Vector camlook){
	//calculate rotation based on cam look
	rotation = Quaternion(Vector(0,0,-1), -1*camlook);
	UpdateWorldMatrix();
	//render buffer
	RenderBuffers(deviceContext);
	return true;
}

void Billboard::SetPosition(Vector v){
	position = v;
}

void Billboard::SetScale(float x, float y){
	xscale = x;
	yscale = y;
}

int Billboard::GetIndexCount(){
	return m_indexCount;
}

D3DXMATRIX Billboard::GetWorldMatrix(){
	return worldMatrix;
}

ID3D11ShaderResourceView* Billboard::GetTexture(){
	return m_texture->GetTexture();
}

bool Billboard::HasAlpha(){
	return true;
}

float Billboard::getDepthSq(Vector campos, Vector look){
	Vector d = campos - position;
	return ((d*look)*(d*look) / (look*look));
}

bool Billboard::InitializeBuffers(ID3D11Device * device){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 6;
	m_indexCount = m_vertexCount;

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

	//setup vertex array
	//tri 1
	vertices[0].position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
	vertices[1].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[2].position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	//tri 2
	vertices[3].position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
	vertices[4].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[5].position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
	vertices[5].texture = D3DXVECTOR2(0.0f, 1.0f);
	//index array
	for (int i = 0; i < m_indexCount; i++){
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
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
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
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//now that buffers have been created, we can release the arrys
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}
	
void Billboard::RenderBuffers(ID3D11DeviceContext * deviceContext){
	//set vertex and index buffers as active, and define the render type

	unsigned int stride;
	unsigned int offset;


	//set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	//make vertex buffer active in input assembler
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//make index buffer active in input assembler
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//define render type
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
	
void Billboard::UpdateWorldMatrix(){
	D3DXQUATERNION qat;
	qat.x = rotation.x;
	qat.y = rotation.y;
	qat.z = rotation.z;
	qat.w = rotation.w;
	D3DXMATRIX trans, scale, rot;
	D3DXMatrixScaling(&scale, xscale, yscale, 1.0f);
	D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
	D3DXMatrixRotationQuaternion(&rot, &qat);
	worldMatrix = scale*rot*trans;
}