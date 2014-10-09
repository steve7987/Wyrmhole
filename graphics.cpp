#include "graphics.h"

Graphics::Graphics(){
	m_d3d = 0;
	m_Camera = 0;
	m_colorShader = 0;
	m_TextureShader = 0;
	m_ShieldShader = 0;
	m_Text = 0;
	m_background = 0;
}

Graphics::~Graphics(){

}

bool Graphics::Initialize(int x , int y , HWND hwnd){
	m_d3d = new D3Dgraphics();
	if (!m_d3d || !m_d3d->Initialize(x, y, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)){
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	//output video card info
	textDump("directx initialized");
	int mem;
	char card[128];
	m_d3d->GetVideoCardInfo(card, mem);
	std::ostringstream oss;
	oss << "Video Card: ";
	oss << card;
	oss << " Memory: ";
	oss << mem;
	textDump(oss.str());

	screenWidth = x;
	screenHeight = y;

	// Create the camera object.
	m_Camera = new Camera();
	if(!m_Camera)
	{
		textDump("failed to make camera");
		return false;
	}
	textDump("updating camera");
	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render(0);
	m_Camera->GetViewMatrix(baseViewMatrix);
	

	//create text object
	m_Text = new Text();

	textDump("intializing text object");
	if (!m_Text){
		MessageBox(hwnd, L"Could not create the text object.", L"Error", MB_OK);
		m_Text = 0;
		return false;
	}
	if (!m_Text->Initialize(m_d3d->GetDevice(), m_d3d->GetDeviceContext(), hwnd, x, y, baseViewMatrix)){
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		delete m_Text;
		m_Text = 0;
		return false;
	}
	
	//create color shader
	m_colorShader = new ColorShader();
	if(!m_colorShader)
	{
		MessageBox(hwnd, L"Could not create the color shader object.", L"Error", MB_OK);
		m_colorShader = 0;
		return false;
	}
	if (!m_colorShader->Initialize(m_d3d->GetDevice(), hwnd)){
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		delete m_colorShader;
		m_colorShader = 0;
		return false;
	}

	//create texture shader
	m_TextureShader = new TextureShader();
	if(!m_TextureShader)
	{
		MessageBox(hwnd, L"Could not create the texture shader object.", L"Error", MB_OK);
		m_TextureShader = 0;
		return false;
	}
	if (!m_TextureShader->Initialize(m_d3d->GetDevice(), hwnd)){
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	//create light shader
	m_LightShader = new LightShader();
	if(!m_LightShader || !m_LightShader->Initialize(m_d3d->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader.", L"Error", MB_OK);
		m_LightShader = 0;
		return false;
	}
	//create shield shader
	m_ShieldShader = new ShieldShader();
	if (!m_ShieldShader || !m_ShieldShader->Initialize(m_d3d->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the shield shader.", L"Error", MB_OK);
		m_ShieldShader = 0;
		return false;
	}


	//create the light
	m_Light = new Light();
	if (!m_Light){
		return false;
	}
	//set light color and direction
	m_Light->SetDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
	//m_Light->SetDirection(0.2f, -1.0f, 0.1f);
	m_Light->SetDirection(0.0f, 0.3f, 1.0f);
	m_Light->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
	activeLight = m_Light;
	textDump("Graphics initialized");
	return true;
}


void Graphics::Shutdown(){
	ClearBackground();	
	//release the light object
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	textDump("freed light");
	//release shield shader
	if (m_ShieldShader){
		m_ShieldShader->Shutdown();
		delete m_ShieldShader;
		m_ShieldShader = 0;
	}

	//release the light shader object
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}
	//release texture shader
	if (m_TextureShader){
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}	
	//release color shader
	if(m_colorShader)
	{
		m_colorShader->Shutdown();
		delete m_colorShader;
		m_colorShader = 0;
	}

	//release text
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	textDump("freed text");

	//release camera
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	textDump("freed camera");
	//release d3d
	if (m_d3d){
		m_d3d->Shutdown();
		delete m_d3d;
		m_d3d = 0;
	}
	textDump("freed d3d");
}

bool Graphics::StartFrame(float time, BaseCamera * cam, Light * light){
	if (cam == NULL){
		activeCamera = m_Camera;
	}
	else {
		activeCamera = cam;
	}
	if (light == NULL){
		activeLight = m_Light;
	}
	else {
		activeLight = light;
	}
	
	// Clear the buffers to begin the scene.
	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//get view matrix from camera
	activeCamera->Render(time);

	//get matrices from cd3d object should both be identity
	activeCamera->GetViewMatrix(viewMatrix);
	m_d3d->GetWorldMatrix(worldMatrix);
	m_d3d->GetProjectionMatrix(projectionMatrix);
	m_d3d->GetOrthoMatrix(orthoMatrix);
	
	m_d3d->TurnZBufferOff();
	m_d3d->TurnOnAlphaBlending();
	//render backgroud image (could render more background 2d stuff here)
	if(m_background){
		if (!m_background->Render(m_d3d->GetDeviceContext())){
			return false;
		}
		if (!m_TextureShader->Render(m_d3d->GetDeviceContext(), m_background->GetIndexCount(), worldMatrix, 
									 baseViewMatrix, orthoMatrix, m_background->GetTexture()))     
		{
			return false;
		}
	}
	m_d3d->TurnZBufferOn();
	m_d3d->TurnOffAlphaBlending();
	//now prepared to render 3d
	return true;
}

bool Graphics::RenderObject(Renderable * m, int shaderType, float * parameters){
	if (m->HasAlpha()){  //if transparent, render it later
		RenderableType rt;
		rt.m = m;
		rt.shader = shaderType;
		rt.parameters = parameters;
		rt.depthSq = m->getDepthSq(activeCamera->GetPosition(), activeCamera->GetLookVector());
		renderQueue.push(rt);
		return true;
	}
	return RenderObjectSwitch(m, shaderType, parameters);
}

bool Graphics::RenderObjectSwitch(Renderable * m, int shaderType, float * parameters){
	bool ret;
	switch (shaderType) {
	case SHADER_LIGHT:
		if (parameters){
			delete parameters;
			parameters = 0;
		}
		return RenderObjectLS(m);
		break;
	case SHADER_COLOR:
		if (parameters){
			delete parameters;
			parameters = 0;
		}
		return RenderObjectCS(m);
		break;
	case SHADER_TEXTURE:
		if (parameters){
			delete parameters;
			parameters = 0;
		}
		return RenderObjectTS(m);
		break;
	case SHADER_SHIELD:
		if (!parameters){
			textDump("bad params for shield shader");
			return false;
		}
		ret = RenderObjectSS(m, D3DXVECTOR3(parameters[0], parameters[1], parameters[2]), parameters[3]);
		delete parameters;
		return ret;
		break;
	default:
		textDump("Unknown shader type");
		return false;
		break;
	}
}

bool Graphics::RenderObjectLS(Renderable * m){	
	if (!m->Render(m_d3d->GetDeviceContext(), activeCamera->GetLookVector())) {
		return false;
	}
	bool result = m_LightShader->Render(m_d3d->GetDeviceContext(), m->GetIndexCount(), m->GetWorldMatrix(), 
										viewMatrix, projectionMatrix,m->GetTexture(), activeLight->GetDirection(),  
										activeLight->GetDiffuseColor(), activeLight->GetAmbientColor());
	if (!result){
		return false;
	}
	return true;
}

bool Graphics::RenderObjectCS(Renderable * m){
	if (!m->Render(m_d3d->GetDeviceContext(), activeCamera->GetLookVector())) {
		return false;
	}
	bool result = m_colorShader->Render(m_d3d->GetDeviceContext(), m->GetIndexCount(), m->GetWorldMatrix(), 
										viewMatrix, projectionMatrix);
	if (!result){
		return false;
	}
	return true;
}

bool Graphics::RenderObjectTS(Renderable * m){
	if (!m->Render(m_d3d->GetDeviceContext(), activeCamera->GetLookVector())){
		return false;
	}
	bool result = m_TextureShader->Render(m_d3d->GetDeviceContext(), m->GetIndexCount(), m->GetWorldMatrix(), 
										  viewMatrix, projectionMatrix, m->GetTexture());
	if (!result){
		return false;
	}
	return true;
}

bool Graphics::RenderObjectSS(Renderable * m, D3DXVECTOR3 direction, float strength){
	
	if (!m->Render(m_d3d->GetDeviceContext(), activeCamera->GetLookVector())) {
		return false;
	}
	bool result = m_ShieldShader->Render(m_d3d->GetDeviceContext(), m->GetIndexCount(), m->GetWorldMatrix(), 
										viewMatrix, projectionMatrix,m->GetTexture(), direction, strength);
	if (!result){
		return false;
	}
	return true;
}

bool Graphics::EndFrame(){
	
	m_d3d->TurnOnAlphaBlending();
	//render transparent objects sorted by depth
	while (!renderQueue.empty()){
		RenderableType rt = renderQueue.top();
		if (!RenderObjectSwitch(rt.m, rt.shader, rt.parameters)){
			return false;
		}
		renderQueue.pop();
	}
	
	m_d3d->TurnZBufferOff();
	//render gui
	if (!g_gui->Render(m_d3d->GetDeviceContext(), m_TextureShader, worldMatrix, baseViewMatrix, orthoMatrix)){
		return false;
	}

	//RENDER TEXT
	if (!m_Text->Render(m_d3d->GetDeviceContext(), worldMatrix, orthoMatrix)){
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_d3d->TurnOffAlphaBlending();

	//turn off 2d rendering
	m_d3d->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_d3d->EndScene();
	
	return true;
}

BaseCamera * Graphics::GetActiveCamera(){
	return activeCamera;
}

ID3D11Device * Graphics::GetDevice(){
	return m_d3d->GetDevice();
}

int Graphics::RegisterSentence(int maxLength){
	return m_Text->RegisterSentence(m_d3d->GetDevice(), maxLength);
}
	
bool Graphics::ChangeSentence(int id, std::string text, int positionX, int positionY, float red, float green, float blue){
	return m_Text->ChangeSentence(m_d3d->GetDeviceContext(), id, text, positionX, positionY, red, green, blue);
}

void Graphics::SetVisibleSentence(int id, bool visible){
	m_Text->SetVisible(id, visible);
}

void Graphics::ClearSentence(int id){
	m_Text->ClearSentence(id);
}

int Graphics::GetSentenceLength(std::string text){
	return m_Text->GetLength(text);
}

int Graphics::GetSentenceHeight(){
	return m_Text->GetHeight();
}

void Graphics::GetMouseRay(int mouseX, int mouseY, Vector& origin, Vector& direction, BaseCamera * cam){
	float pointX, pointY;
	BaseCamera * currentCam;
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix;

	if (cam == NULL){
		currentCam = m_Camera;
	}
	else {
		currentCam = cam;
	}
	
	//change x, y to be in -1, 1 range
	pointX = ((2.0f * (float)mouseX) / (float)screenWidth) - 1.0f;
	pointY = (((2.0f * (float)mouseY) / (float)screenHeight) - 1.0f) * -1.0f;
		
	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	m_d3d->GetProjectionMatrix(projectionMatrix);
	pointX = pointX / projectionMatrix._11;
	pointY = pointY / projectionMatrix._22;

	// Get the inverse of the view matrix.
	cam->GetViewMatrix(viewMatrix);
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);

	// Calculate the direction of the picking ray in view space.
	direction.x = (pointX * inverseViewMatrix._11) + (pointY * inverseViewMatrix._21) + inverseViewMatrix._31;
	direction.y = (pointX * inverseViewMatrix._12) + (pointY * inverseViewMatrix._22) + inverseViewMatrix._32;
	direction.z = (pointX * inverseViewMatrix._13) + (pointY * inverseViewMatrix._23) + inverseViewMatrix._33;

	//get the origin of the mouse ray which is the location of the camera
	origin = cam->GetPosition();
}

void Graphics::SetBackground(WCHAR* textureFilename){
	//first remove old backgroud
	ClearBackground();
	//now make new one
	m_background = new Bitmap();
	m_background->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, textureFilename, (float) screenWidth, (float) 
							 screenHeight, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
}

void Graphics::ClearBackground(){
	if (m_background != 0){
		m_background->Shutdown();
		delete m_background;
		m_background = 0;
	}
}