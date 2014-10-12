#include "tracktube.h"

TrackTube::TrackTube(){
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	m_texture = 0;
	vinfo = 0;
}

TrackTube::~TrackTube(){

}
	
bool TrackTube::Initialize(ID3D11Device * device, TrackSegment * segment, WCHAR * textureFilename, float radius, 
						   float textureRepeat, float startdist, int tubesides, int tubesegments, float randomness) 
{
	numTubeSides = tubesides;
	numTubeSegments = tubesegments;

	GetVertexInfo(segment, radius, randomness);
	if (!InitializeBuffers(device, segment, textureRepeat, startdist)){
		return false;
	}
	m_texture = g_textureManager->GetTexture(device, textureFilename);
	return true;
}

void TrackTube::Shutdown(){
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

bool TrackTube::Render(ID3D11DeviceContext * deviceContext, Vector camlook){
	//render buffer
	RenderBuffers(deviceContext);
	return true;
}

int TrackTube::GetIndexCount(){
	return m_indexCount;
}

D3DXMATRIX TrackTube::GetWorldMatrix(){
	D3DXMATRIX id;
	D3DXMatrixIdentity(&id);
	return id;
}

ID3D11ShaderResourceView* TrackTube::GetTexture(){
	return m_texture->GetTexture();
}

bool TrackTube::HasAlpha(){
	return false;
}

float TrackTube::getDepthSq(Vector campos, Vector camlook){
	return 10; //doesnt matter, tube will never be transparent
}

bool TrackTube::InitializeBuffers(ID3D11Device * device, TrackSegment * segment, float repeat, float startdist){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	//make index and vertex arrays
	m_vertexCount = 6*TUBESIDES*NUMTUBESEGMENTS;  //2 tris per side
	m_indexCount = m_vertexCount;
	vertices = new VertexType[m_vertexCount];
	if (!vertices){
		return false;
	}
	indices = new unsigned long[m_indexCount];
	if (!indices){
		return false;
	}
	//setup vertex info
	double seglength = segment->GetLength() / NUMTUBESEGMENTS;
	double curlength = startdist;
	for (int i = 0; i < NUMTUBESEGMENTS; i++){
		for (int k = 0; k < TUBESIDES - 1; k++){
			//bottom left
			vertices[6*(TUBESIDES*i + k) + 0].position = vinfo[i*TUBESIDES + k].position.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 0].normal = vinfo[i*TUBESIDES + k].normal.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 0].texture = D3DXVECTOR2((float)(curlength/repeat), (1.0f*k)/TUBESIDES);

			//bottom right
			vertices[6*(TUBESIDES*i + k) + 1].position = vinfo[(i+1)*TUBESIDES + k].position.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 1].normal = vinfo[(i+1)*TUBESIDES + k].normal.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 1].texture = D3DXVECTOR2((float)((curlength+seglength)/repeat), (1.0f*k)/TUBESIDES);

			//top left
			vertices[6*(TUBESIDES*i + k) + 2].position = vinfo[i*TUBESIDES + k + 1].position.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 2].normal = vinfo[i*TUBESIDES + k + 1].normal.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 2].texture = D3DXVECTOR2((float)(curlength/repeat), (1.0f*(k+1))/TUBESIDES);

			//second tri 
			//top left
			vertices[6*(TUBESIDES*i + k) + 3].position = vinfo[i*TUBESIDES + k + 1].position.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 3].normal = vinfo[i*TUBESIDES + k + 1].normal.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 3].texture = D3DXVECTOR2((float)(curlength/repeat), (1.0f*(k+1))/TUBESIDES);

			//bottom right
			vertices[6*(TUBESIDES*i + k) + 4].position = vinfo[(i+1)*TUBESIDES + k].position.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 4].normal = vinfo[(i+1)*TUBESIDES + k].normal.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 4].texture = D3DXVECTOR2((float)((curlength+seglength)/repeat), (1.0f*k)/TUBESIDES);

			//top right
			vertices[6*(TUBESIDES*i + k) + 5].position = vinfo[(i+1)*TUBESIDES + k + 1].position.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 5].normal = vinfo[(i+1)*TUBESIDES + k + 1].normal.d3dvector();
			vertices[6*(TUBESIDES*i + k) + 5].texture = D3DXVECTOR2((float)((curlength+seglength)/repeat), (1.0f*(k+1))/TUBESIDES);
		}
		
		//handle kth side differently since it wraps around and texture coords and vertex position dont match formula
		//bottom left
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 0].position = vinfo[i*TUBESIDES + TUBESIDES - 1].position.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 0].normal = vinfo[i*TUBESIDES + TUBESIDES - 1].normal.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 0].texture = D3DXVECTOR2((float)(curlength/repeat), (1.0f*(TUBESIDES - 1))/TUBESIDES);

		//bottom right
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 1].position = vinfo[(i+1)*TUBESIDES + TUBESIDES - 1].position.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 1].normal = vinfo[(i+1)*TUBESIDES + TUBESIDES - 1].normal.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 1].texture = D3DXVECTOR2((float)((curlength+seglength)/repeat), (1.0f*(TUBESIDES - 1))/TUBESIDES);

		//top left
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 2].position = vinfo[i*TUBESIDES + 0].position.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 2].normal = vinfo[i*TUBESIDES + 0].normal.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 2].texture = D3DXVECTOR2((float)(curlength/repeat), 1.0f);

		//second tri 
		//top left
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 3].position = vinfo[i*TUBESIDES + 0].position.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 3].normal = vinfo[i*TUBESIDES + 0].normal.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 3].texture = D3DXVECTOR2((float)(curlength/repeat), 1.0f);

		//bottom right
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 4].position = vinfo[(i+1)*TUBESIDES + TUBESIDES - 1].position.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 4].normal = vinfo[(i+1)*TUBESIDES + TUBESIDES - 1].normal.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 4].texture = D3DXVECTOR2((float)((curlength+seglength)/repeat), (1.0f*(TUBESIDES - 1))/TUBESIDES);

		//top right
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 5].position = vinfo[(i+1)*TUBESIDES + 0].position.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 5].normal = vinfo[(i+1)*TUBESIDES + 0].normal.d3dvector();
		vertices[6*(TUBESIDES*i + TUBESIDES - 1) + 5].texture = D3DXVECTOR2((float)((curlength+seglength)/repeat), 1.0f);
		curlength += seglength;

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


	//now that buffers have been created, we can release the arrays
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	delete [] vinfo;
	vinfo = 0;

	return true;

}

void TrackTube::RenderBuffers(ID3D11DeviceContext * deviceContext){
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

void TrackTube::GetVertexInfo(TrackSegment * segment, float radius, float randomness){
	vinfo = new VertexInfo[TUBESIDES*(NUMTUBESEGMENTS+1)];
	//setup verticies
	double length = segment->GetLength();
	for (int i = 0; i < NUMTUBESEGMENTS + 1; i++){
		Vector center = segment->GetPoint(i*length/NUMTUBESEGMENTS);
		Quaternion rotation = segment->GetRotation(i*length/NUMTUBESEGMENTS);
		Vector v1 = rotation*Vector(0,1,0);
		Vector v2 = rotation*Vector(0,0,1);
		for (int k = 0; k < TUBESIDES; k++){
			Vector randomOffset = Vector(0, 0, 0);
			if (i != 0 && i != NUMTUBESEGMENTS){
				//if its not the front or end of the segment add some randomness to look more wormholey
				randomOffset = Vector(randb(-randomness * radius, randomness * radius), 
									  randb(-randomness * radius, randomness * radius), 
									  randb(-randomness * radius, randomness * radius));
			}
			vinfo[i*TUBESIDES + k].position = center + radius*(v1*((float)cos(k*2*PI/TUBESIDES)) + v2*((float)sin(k*2*PI/TUBESIDES))) + randomOffset;
		}
	}
	//calculate normals	
	for (int i = 0; i < NUMTUBESEGMENTS + 1; i++){
		for (int k = 0; k < TUBESIDES; k++){
			if (i == 0){  //border case
				vinfo[i*TUBESIDES + k].normal = Vector(-1, -1, -1);
				
				Vector p0 = vinfo[i*TUBESIDES + k].position;
				Vector pup = vinfo[i*TUBESIDES + (k+1)%TUBESIDES].position - p0;
				Vector pdown = vinfo[i*TUBESIDES + (k-1+TUBESIDES)%TUBESIDES].position - p0;
				Vector pright = vinfo[(i+1)*TUBESIDES + k].position - p0;
				//Vector pleft = vinfo[(i-1)*TUBESIDES + k].position - p0;
				Vector norm = pright.cross(pup) + pdown.cross(pright);
				vinfo[i*TUBESIDES + k].normal = norm/norm.length();
				
			}
			else if (i == NUMTUBESEGMENTS){  //other end
				Vector p0 = vinfo[i*TUBESIDES + k].position;
				Vector pup = vinfo[i*TUBESIDES + (k+1)%TUBESIDES].position - p0;
				Vector pdown = vinfo[i*TUBESIDES + (k-1+TUBESIDES)%TUBESIDES].position - p0;
				//Vector pright = vinfo[(i+1)*TUBESIDES + k].position - p0;
				Vector pleft = vinfo[(i-1)*TUBESIDES + k].position - p0;
				Vector norm = pup.cross(pleft) + pleft.cross(pdown);
				vinfo[i*TUBESIDES + k].normal = norm/norm.length();
			}
			else {
				Vector p0 = vinfo[i*TUBESIDES + k].position;
				Vector pup = vinfo[i*TUBESIDES + (k+1)%TUBESIDES].position - p0;
				Vector pdown = vinfo[i*TUBESIDES + (k-1+TUBESIDES)%TUBESIDES].position - p0;
				Vector pright = vinfo[(i+1)*TUBESIDES + k].position - p0;
				Vector pleft = vinfo[(i-1)*TUBESIDES + k].position - p0;
				Vector norm = pright.cross(pup) + pup.cross(pleft) + pleft.cross(pdown) + pdown.cross(pright);
				vinfo[i*TUBESIDES + k].normal = norm/norm.length();
			}
		}
	}
	
}