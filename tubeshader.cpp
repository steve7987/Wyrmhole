#include "tubeshader.h"
//enabled backwards compat and using 4.0 shader

TubeShader::TubeShader(){
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_lightBuffer = 0;
	m_cameraBuffer = 0;
	m_tubeLayoutBuffer = 0;
}

TubeShader::~TubeShader(){

}


bool TubeShader::Initialize(ID3D11Device* device, HWND hwnd){
	
	if (!InitializeShader(device, hwnd, TUBESHADERFILEVS, TUBESHADERFILEPS)){
		return false;
	}

	return true;
}

void TubeShader::Shutdown(){
	ShutdownShader();
}

bool TubeShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
						 D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
						 D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor, D3DXVECTOR3 cameraPosition, float textureOffset, 
						 D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4,
					     D3DXVECTOR3 Center, D3DXVECTOR3 v1, D3DXVECTOR3 v2, float s1, float s2, float s3, float maxAngle, float radius)
{
	//set shader parameters
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
							 texture, lightDirection, diffuseColor, ambientColor, cameraPosition, 
							 textureOffset, p1, p2, p3, p4, Center, v1, v2, s1, s2, s3, maxAngle, radius))
	{
		return false;
	}
	//then render
	RenderShader(deviceContext, indexCount);

	return true;
}


bool TubeShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename){
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];  //changed to 4 to accomodate position, uv, normal, distance
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;  //new from color shader
	D3D11_BUFFER_DESC lightBufferDesc;  //new from texture shader
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC tubeLayoutBufferDesc;

	//initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//compile vertex shader program
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "TubeVertexShader", "vs_4_0", 
								   D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, NULL, &vertexShaderBuffer,
								   &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	//compile pixel shader program
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "TubePixelShader", "ps_4_0",
								   D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	//create vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}
	//create pixel shader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
									   pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	//create data layout for vertex shader, needs to match vertexType struct in model class and in the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//adding normals to polygon layout
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	//adding distance measure to polygon layout
	
	polygonLayout[3].SemanticName = "POSITION";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;
	
	//count number of items in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
					   vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	//release vertex and pixel shaders data
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//setup buffer for shaders to hold matrix info
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create buffer pointer so it can be accessed here
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//create description for texture sampler
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}
	//setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//setup tube layout buffer
	tubeLayoutBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tubeLayoutBufferDesc.ByteWidth = sizeof(TubeLayoutBufferType);
	tubeLayoutBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tubeLayoutBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tubeLayoutBufferDesc.MiscFlags = 0;
	tubeLayoutBufferDesc.StructureByteStride = 0;

	// Create the tube layout constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&tubeLayoutBufferDesc, NULL, &m_tubeLayoutBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//setup light buffer for pixel shader.  note byteWidth needs to be multiple of 16
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void TubeShader::ShutdownShader(){
	if (m_tubeLayoutBuffer){
		m_tubeLayoutBuffer->Release();
		m_tubeLayoutBuffer = 0;
	}
	
	if (m_cameraBuffer){
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	//release the light constant buffer.
	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	//release sampler state
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}
	
	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void TubeShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename){
	//writes errors when shader fails to compile
	char* compileErrors;
	unsigned long bufferSize, i;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	std::ostringstream oss;

	for (i = 0; i < bufferSize; i++){
		oss << compileErrors[i];
	}

	textDump(oss.str());

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check log.txt for message.", shaderFilename, MB_OK);
}

//set global variables for shaders
bool TubeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
					   D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
					   D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor, D3DXVECTOR3 cameraPosition, float textureOffset,
					   D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4,
					   D3DXVECTOR3 Center, D3DXVECTOR3 v1, D3DXVECTOR3 v2, float s1, float s2, float s3, float maxAngle, float radius)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;
	TubeLayoutBufferType * dataPtr4;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader. required for directx11
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);
	
	//set update matrix buffer into vertex shader
	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//set shader texture in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//setup the light buffer
	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->textureOffset = textureOffset;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(m_cameraBuffer, 0);
	// Set the position of the camera constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	
	// Lock the tube layout constant buffer so it can be written to.
	result = deviceContext->Map(m_tubeLayoutBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr4 = (TubeLayoutBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataPtr4->p1 = p1;
	dataPtr4->p2 = p2;
	dataPtr4->p3 = p3;
	dataPtr4->p4 = p4;
	dataPtr4->Center = Center;
	dataPtr4->v1 = v1;
	dataPtr4->v2 = v2;
	dataPtr4->s1 = s1;
	dataPtr4->s2 = s2;
	dataPtr4->s3 = s3;
	dataPtr4->maxAngle = maxAngle;
	dataPtr4->radius = radius;
	dataPtr4->pad1 = 0.0f;
	dataPtr4->pad2 = 0.0f;

	// Unlock the tube layout constant buffer.
	deviceContext->Unmap(m_tubeLayoutBuffer, 0);
	// Set the position of the tube layout constant buffer in the vertex shader.
	bufferNumber = 2;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_tubeLayoutBuffer);
	
	return true;
}

//set our data layout to active and then set the shaders to be active
void TubeShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//set the sampler
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render
	deviceContext->DrawIndexed(indexCount, 0, 0);
}