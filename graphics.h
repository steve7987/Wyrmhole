#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_


#include <Windows.h>
#include <string.h>
#include <map>
#include <queue>

#include "classlist.h"
#include "d3dgraphics.h"
#include "camera.h"
#include "colorshader.h"
#include "textureshader.h"
#include "shieldshader.h"
#include "renderable.h"
#include "bitmap.h"
#include "text.h"
#include "helpers.h"
#include "gui.h"
#include "light.h"
#include "lightshader.h"
#include "basecamera.h"
#include "billboard.h"
#include "tubeshader.h"

extern Gui * g_gui;

//global vars for directx settings

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 5000.0f;
const float SCREEN_NEAR = 0.1f;

#define SHADER_LIGHT 0
#define SHADER_COLOR 1
#define SHADER_TEXTURE 2
#define SHADER_SHIELD 3
#define SHADER_TUBE 4

class Graphics{
public:
	Graphics();
	~Graphics();
	bool Initialize(int x , int y , HWND hwnd);  //x = screen width, y = screen height
	void Shutdown();

	bool StartFrame(float time, BaseCamera * cam = NULL, Light * light = NULL);
	bool EndFrame();

	//should ONLY be called between calls to start frame and end frame
	//parameters is an array of floats only used for some shaders and will be freed by graphics class
	bool RenderObject(Renderable * m, int shaderType, float * parameters = 0);
	//if not called between start frame and end frame it may not be actual avtive camera
	BaseCamera * GetActiveCamera();

	ID3D11Device * GetDevice();

	//SENTENCE CONTROLS
	int RegisterSentence(int maxLength);  
	//create a space for a sentence and give index so it can be accessed later
	bool ChangeSentence(int id, std::string text, int positionX, int positionY, float red, float green, float blue);  
	int GetSentenceLength(std::string text);  //gives how many pixels wide the text will be
	int GetSentenceHeight();  //gives height of a text character (eventually should take a font type as well)
	//update sentence with given index
	void SetVisibleSentence(int id, bool visible);
	void ClearSentence(int id);  //clear sentence and remove it from the array

	//creates a ray into the screen from pixel mouseX, mouseY
	void GetMouseRay(int mouseX, int mouseY, Vector& origin, Vector& direction, BaseCamera * cam = NULL);

	//background commands
	void SetBackground(WCHAR* textureFilename);  
	void ClearBackground();
	
private:
	bool RenderObjectSwitch(Renderable * m, int shaderType, float * parameters = 0);
	bool RenderObjectLS(Renderable * m);    //render using light shader
	bool RenderObjectCS(Renderable * m);    //render using color shader
	bool RenderObjectTS(Renderable * m);	//render using texture shader
	bool RenderObjectSS(Renderable * m, D3DXVECTOR3 direction, float strength);  //render using shield shader
	bool RenderObjectsTube(Renderable * m, float textureOffset);

	D3DXMATRIX viewMatrix, projectionMatrix, orthoMatrix, worldMatrix; //used in rendering a frame
	
	D3Dgraphics * m_d3d;
	Camera * m_Camera;  //default scene camera
	Light * m_Light;  //default scene light
	ColorShader * m_colorShader;
	TextureShader * m_TextureShader;
	LightShader * m_LightShader;
	ShieldShader * m_ShieldShader;
	TubeShader * m_TubeShader;
	D3DXMATRIX baseViewMatrix;  //used for rendering text and gui

	priority_queue<RenderableType, vector<RenderableType>, CompareRenderableType> renderQueue;

	Light * activeLight;
	BaseCamera * activeCamera;

	int screenWidth;
	int screenHeight;
	
	Text * m_Text;

	Bitmap * m_background;
};

#endif