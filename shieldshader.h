#ifndef _SHIELDSHADER_H_
#define _SHIELDSHADER_H_


#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "helpers.h"

using namespace std;

#define SHIELDSHADERFILEVS L"./shield.vs"
#define SHIELDSHADERFILEPS L"./shield.ps"

class ShieldShader{
private:
	//definition of cbuffer, must be the same as the one in vertex shader
	struct MatrixBufferType {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct ShieldBufferType{
		D3DXVECTOR3 direction;
		float strength;  //determines how much strength/alpha
	};

public:
	ShieldShader();
	~ShieldShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
			    D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
				D3DXVECTOR3 direction, float strength);
	//direction should be unit vector from center of shield to point of contact
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
					   D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
					   D3DXVECTOR3 direction, float strength);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	


	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	//new from color shader:  sample state to interface with texture shader
	ID3D11SamplerState* m_sampleState;
	//new from texture shader:  buffer to hold global vars for the shader
	ID3D11Buffer* m_shieldBuffer;
};

#endif