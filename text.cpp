#include "text.h"


Text::Text(){
	m_Font = 0;
	m_FontShader = 0;
}

Text::~Text(){
	
	
}

	
bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, 
					D3DXMATRIX baseViewMatrix){
	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	
	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	//create and initialize font
	m_Font = new Font();
	if(!m_Font)
	{
		return false;
	}
	textDump("initializing font");
	if(!m_Font->Initialize(device, "./Assets/plaintext.txt", L"./Assets/plaintext.dds"))
	{
		return false;
	}
	
	// Create the font shader object.
	m_FontShader = new FontShader;
	if(!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	if(!m_FontShader->Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}
	//initialize sentence array
	m_sentenceArray = new SentenceType*[MAXSENTENCE];
	if (!m_sentenceArray){
		return false;
	}
	for (int i = 0; i < MAXSENTENCE; i++){
		m_sentenceArray[i] = 0;
	}
	highestSentence = 0;

	return true;
}

	
void Text::Shutdown(){
	//clear sentence array
	if (m_sentenceArray){
		for (int i = 0; i < MAXSENTENCE; i++){
			if (m_sentenceArray[i] != 0){
				ReleaseSentence(m_sentenceArray[i]);
				delete m_sentenceArray[i];
				m_sentenceArray[i] = 0;
			}
		}
		delete [] m_sentenceArray;
		m_sentenceArray = 0;
	}
	// Release the font shader object.
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

}

	
bool Text::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix){
	//render sentence array
	for (int i = 0; i < highestSentence; i++){
		if (m_sentenceArray[i] != 0 && m_sentenceArray[i]->visible){
			if (!RenderSentence(deviceContext, m_sentenceArray[i], worldMatrix, orthoMatrix)){
				return false;
			}
		}
	}
	
	//renders two test sentences
	/*
	if (!RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix)){
		return false;
	}

	if (!RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix)){
		return false;
	}
	*/
	return true;
}

int Text::RegisterSentence(ID3D11Device* device, int maxLength){
	//find first open spot for sentence
	int index;
	for (index = 0; index < MAXSENTENCE; index++){
		if (m_sentenceArray[index] == 0){
			break;
		}
	}
	if (index == MAXSENTENCE){
		textDump("sentence array full");
		return -1;
	}
	//check if highestsentence should be updated
	if (index >= highestSentence){
		highestSentence = index + 1;
	}
	//found open index, register sentence
	//can make new sentence here and then not pass sentencetype ** (i think)
	m_sentenceArray[index] = new SentenceType;
	if (InitializeSentence(m_sentenceArray[index], maxLength, device)){
		m_sentenceArray[index]->visible = true;
		return index;
	}
	textDump("error initializing sentence");
	return -1;
}
	
bool Text::ChangeSentence(ID3D11DeviceContext* deviceContext, int id, std::string text, int positionX, int positionY, 
						  float red, float green, float blue){
	if (m_sentenceArray[id] == 0){
		textDump("invalid access of sentence");
		return false;
	}
	return UpdateSentence(m_sentenceArray[id], text, positionX, positionY, red, green, blue, deviceContext);
}

void Text::SetVisible(int id, bool visible){
	m_sentenceArray[id]->visible = visible;
}

void Text::ClearSentence(int id){
	ReleaseSentence(m_sentenceArray[id]);
	delete m_sentenceArray[id];
	m_sentenceArray[id] = 0;
}

int Text::GetLength(std::string text){
	return m_Font->GetLength(text.c_str());
}

int Text::GetHeight(){
	return m_Font->GetHeight();
}

bool Text::InitializeSentence(SentenceType * sentence, int maxLength, ID3D11Device* device){
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Create a new sentence object.
	/**sentence = new SentenceType;
	if(!*sentence)
	{
		return false;
	}
	*/
	// Initialize the sentence buffers to null.
	(sentence)->vertexBuffer = 0;
	(sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(sentence)->indexCount = (sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(sentence)->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(sentence)->indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (sentence)->vertexCount));

	// Initialize the index array.
	for(int i = 0; i < (sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	//vertices are dynamic so we can change sentence content easily
	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(sentence)->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//index buffer can be static since it never changes
	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(sentence)->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;

}

	
bool Text::UpdateSentence(SentenceType* sentence, std::string text, int positionX, int positionY, float red, float green, float blue,
						ID3D11DeviceContext* deviceContext){
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	//set color and size of sentence
	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;
	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text.c_str());

	// Check for possible buffer overflow.
	if(numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	//calculate start pos
	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text.c_str(), drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}


void Text::ReleaseSentence(SentenceType * sentence){
	if(sentence)
	{
		// Release the sentence vertex buffer.
		if(sentence->vertexBuffer)
		{
			sentence->vertexBuffer->Release();
			sentence->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if(sentence->indexBuffer)
		{
			sentence->indexBuffer->Release();
			sentence->indexBuffer = 0;
		}
	}
}

	
bool Text::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, 
						D3DXMATRIX orthoMatrix){
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), 
				      pixelColor);
	if(!result)
	{
		false;
	}

	return true;
}
