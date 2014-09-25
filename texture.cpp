#include "texture.h"

Texture::Texture(){
	m_texture = 0;
}
	
Texture::~Texture(){

}

	
bool Texture::Initialize(ID3D11Device* device, WCHAR* filename, bool useMipMaps){
	HRESULT result;
	D3DX11_IMAGE_LOAD_INFO imageLoad;
	
	//setup image load info description
	imageLoad.Width = D3DX11_DEFAULT;
	imageLoad.Height = D3DX11_DEFAULT;
	imageLoad.Depth = D3DX11_DEFAULT;
	imageLoad.FirstMipLevel = D3DX11_DEFAULT;
	imageLoad.MipLevels = 1;  //only non default prevents gui window blurring
	imageLoad.Usage = (D3D11_USAGE) D3DX11_DEFAULT;
	imageLoad.BindFlags = D3DX11_DEFAULT;
	imageLoad.CpuAccessFlags = D3DX11_DEFAULT;
	imageLoad.MiscFlags = D3DX11_DEFAULT;
	imageLoad.Format = DXGI_FORMAT_FROM_FILE;
	imageLoad.Filter = D3DX11_DEFAULT;
	imageLoad.MipFilter = D3DX11_DEFAULT;
	imageLoad.pSrcInfo = NULL;




	// Load the texture in.
	if (useMipMaps){
		result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	}
	else {
		result = D3DX11CreateShaderResourceViewFromFile(device, filename, &imageLoad, NULL, &m_texture, NULL);
	}
	if(FAILED(result))
	{
		//textDump("failed");
		return false;
	}
	//textDump("done");
	return true;
}
	
void Texture::Shutdown(){
	if (m_texture){
		m_texture->Release();
		m_texture = 0;
	}
}
	
ID3D11ShaderResourceView* Texture::GetTexture(){
	return m_texture;
}