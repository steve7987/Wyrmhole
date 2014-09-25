#include "font.h"

Font::Font(){
	m_Font = 0;
	m_Texture = 0;
}

Font::~Font(){

}

	
bool Font::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename){
	//textDump("loading font data");
	if (!LoadFontData(fontFilename)){
		return false;
	}

	//textDump("loading texture data");
	if (!LoadTexture(device, textureFilename)){
		return false;
	}

	return true;
}
	
void Font::Shutdown(){
	ReleaseTexture();
	ReleaseFontData();
}

	
ID3D11ShaderResourceView* Font::GetTexture(){
	return m_Texture->GetTexture();
}

	
void Font::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY){
	VertexType* vertexPtr;
	int numLetters, index, letter;
	float initialX = drawX;

	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	//loop through chars to build arrays, update x coord as needed
	for(int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else if (letter == '\t' - 32){
			float length = drawX - initialX + 3.0f;  //size of space added
			int numtabs = (int) (length/TAB_LENGTH) + 1;
			drawX = initialX + numtabs * TAB_LENGTH;
		}
		else if (letter == '\n' - 32){
			drawX = initialX;
			drawY -= 13;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
}
	
int Font::GetLength(const char* sentence){
	int numLetters = (int)strlen(sentence);
	int length = 0;
	
	for(int i = 0; i < numLetters; i++){
		int letter = ((int)sentence[i]) - 32;
		//move over 3 for space
		if (letter == 0){
			length += 3;  
		}
		else{
			length += m_Font[letter].size + 1;
		}
	}
	return length;
}

bool Font::LoadFontData(char* filename){
	ifstream fin;
	char temp;

	//create array to hold spacing data
	m_Font = new FontType[95];
	if(!m_Font)
	{
		return false;
	}

	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	//textDump("entering font loop");
	//now read 95 ascii chars
	for(int i = 0; i < 95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// Close the file.
	fin.close();
	return true;
}
	
void Font::ReleaseFontData(){
	if (m_Font){
		delete m_Font; 
		m_Font = 0;
	}
}
	
bool Font::LoadTexture(ID3D11Device* device, WCHAR* filename){
	m_Texture = g_textureManager->GetTexture(device, filename);
	return true;

}
	
void Font::ReleaseTexture(){

}