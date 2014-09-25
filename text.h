#ifndef _TEXT_H_
#define _TEXT_H_

#include "classlist.h"
#include "font.h"
#include "fontshader.h"

#define MAXSENTENCE 1024

class Text{
private:
	struct SentenceType{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
		bool visible;
	};

	struct VertexType  //needs to match font vertex type
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	Text();
	~Text();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, 
					D3DXMATRIX baseViewMatrix);

	int RegisterSentence(ID3D11Device* device, int maxLength);  
	//create a space for a sentence and give index so it can be accessed later
	bool ChangeSentence(ID3D11DeviceContext* deviceContext, int id, std::string text, int positionX, int positionY, float red, float green, float blue);  
	void SetVisible(int id, bool visible);
	//update sentence with given index
	void ClearSentence(int id);  //clear sentence and remove it from the array
	int GetLength(std::string text);

	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);

private:
	bool InitializeSentence(SentenceType * sentence, int maxLength, ID3D11Device* device);
	bool UpdateSentence(SentenceType* sentence, std::string text, int positionX, int positionY, float red, float green, float blue,
						ID3D11DeviceContext* deviceContext);  //sentence to update, position, text and color to make it
	void ReleaseSentence(SentenceType * sentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, 
						D3DXMATRIX orthoMatrix);


	Font* m_Font;
	FontShader* m_FontShader;
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;

	//sentence array
	//at some point may want to clean up.  Were delete issues when dynamically loaded
	SentenceType ** m_sentenceArray;
	int highestSentence;  //highest possible number currently in array, may be gaps

};




#endif