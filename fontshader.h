#ifndef _FONTSHADER_H_
#define _FONTSHADER_H_


#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "classlist.h"
#include "helpers.h"

#define FONTSHADERFILEVS L"./font.vs"
#define FONTSHADERFILEPS L"./font.ps"

using namespace std;

class FontShader{
private:
	//definition of cbuffer, must be the same as the one in vertex shader
	struct MatrixBufferType {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct PixelBufferType{
		D3DXVECTOR4 pixelColor;
	};

public:
	FontShader();
	~FontShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
			    D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR4 pixelColor);
private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
					   D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR4 pixelColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);


	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_constantBuffer;

	//new from color shader:  sample state to interface with texture shader
	ID3D11SamplerState* m_sampleState;

	//constant buffer for pixel color
	ID3D11Buffer* m_pixelBuffer;
};

#endif