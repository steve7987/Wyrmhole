#include "trackline.h"


TrackLine::TrackLine(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
}

TrackLine::~TrackLine(){

}
	
bool TrackLine::Initialize(ID3D11Device * device, TrackSegment * segment){
	if (!InitializeBuffers(device, segment)){
		return false;
	}
	return true;
}

void TrackLine::Shutdown(){
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

bool TrackLine::Render(ID3D11DeviceContext * deviceContext, Vector camlook){
	//render buffer
	RenderBuffers(deviceContext);
	return true;
}

int TrackLine::GetIndexCount(){
	return m_indexCount;
}

D3DXMATRIX TrackLine::GetWorldMatrix(){
	D3DXMATRIX id;
	D3DXMatrixIdentity(&id);

	return id;

}

ID3D11ShaderResourceView* TrackLine::GetTexture(){
	return NULL;
}

bool TrackLine::HasAlpha(){
	return false;
}

float TrackLine::getDepthSq(Vector campos, Vector camlook){
	return 10; //should never be in queue for transparent rendering so this value doesnt matter
}

bool TrackLine::InitializeBuffers(ID3D11Device * device, TrackSegment * segment){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 4*NUMLINESEGMENTS;
	m_indexCount = m_vertexCount;
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}
	//make index array
	indices = new unsigned long[m_indexCount];
	if (!indices){
		return false;
	}
	float length = segment->GetLength();
	length = length / NUMLINESEGMENTS;
	for (int i = 0; i < NUMLINESEGMENTS; i++){
		Vector start = segment->GetPoint(i*length);
		Vector end = segment->GetPoint((i+1)*length);
		vertices[4*i].position = start.d3dvector();
		vertices[4*i].color = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
		vertices[4*i+1].position = end.d3dvector();
		vertices[4*i+1].color = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
		Vector up1 = 2*segment->GetRotation(i*length)*Vector(0,1,0);
		Vector up2 = 2*segment->GetRotation((i+1)*length)*Vector(0,1,0);
		vertices[4*i+2].position = (start + up1).d3dvector();
		vertices[4*i+2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		vertices[4*i+3].position = (end + up2).d3dvector();
		vertices[4*i+3].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	}

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
	
	
	delete [] indices;
	indices = 0;
	delete [] vertices;
	vertices = 0;
	return true;


}

void TrackLine::RenderBuffers(ID3D11DeviceContext * deviceContext){
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}