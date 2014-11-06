#ifndef _TUBESHADER_H_
#define _TUBESHADER_H_


#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "helpers.h"

using namespace std;

#define TUBESHADERFILEVS L"./tube.vs"
#define TUBESHADERFILEPS L"./tube.ps"

class TubeShader{
private:
	//definition of cbuffer, must be the same as the one in vertex shader
	struct MatrixBufferType {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct LightBufferType{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;  //needed to make struct multiple of 16 for createbuffer function requirements
	};
	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float textureOffset;
	};

	struct TubeLayoutBufferType
	{
		D3DXVECTOR3 p1;  //p1, p2, p3, p4 are positions of the four points of the tube
		float s1;  //s1, s2, s3 are distances of each of the sections
		D3DXVECTOR3 p2;
		float s2;
		D3DXVECTOR3 p3;
		float s3;
		D3DXVECTOR3 p4;
		float maxAngle; 
		D3DXVECTOR3 Center;  //center of the turning circle
		float radius;  //padding to make each segment mult of 4
		D3DXVECTOR3 v1;  //unit vectors for parameterizing the circle
		float pad1;
		D3DXVECTOR3 v2;
		float pad2;
	};

public:
	TubeShader();
	~TubeShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
			    D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
				D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor, D3DXVECTOR3 cameraPosition, float textureOffset, 
				D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4,
				D3DXVECTOR3 Center, D3DXVECTOR3 v1, D3DXVECTOR3 v2, float s1, float s2, float s3, float maxAngle, float radius);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
					   D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, 
					   D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor, D3DXVECTOR3 cameraPosition, float textureOffset,
					   D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4,
					   D3DXVECTOR3 Center, D3DXVECTOR3 v1, D3DXVECTOR3 v2, float s1, float s2, float s3, float maxAngle, float radius);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);


private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	


	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_tubeLayoutBuffer;

	//new from color shader:  sample state to interface with texture shader
	ID3D11SamplerState* m_sampleState;
	//new from texture shader:  buffer to hold global light variables for the shader
	ID3D11Buffer* m_lightBuffer;

};

#endif