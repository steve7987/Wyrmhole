#include "gapp.h"



Gapp::Gapp(){
	m_applicationName = 0;
	m_hinstance = 0;
	m_hwnd = 0;
	m_graphics = 0;
	m_input = 0;
	m_fps = 0;
	m_timer = 0;
	m_gui = 0;
	m_textureManager = 0;
	m_gameStateManager = 0;
	m_modelManager = 0;
	done = false;

}

Gapp::~Gapp(){

}


bool Gapp::Initialize() {
	screenWidth= 0;
	screenHeight = 0;
	//clear log file
	std::ofstream myfile;
	myfile.open (LOGFILE, std::ios::out);
	myfile << "Log file\n";
	myfile.close();
	//initialize the window
	InitializeWindows(screenWidth, screenHeight);
	g_screenWidth = screenWidth;
	g_screenHeight = screenHeight;
	//initialize input and graphics
	m_input = new Input();
	if (!m_input || !m_input->Initialize(m_hwnd, screenWidth, screenHeight)){
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		textDump("failed to init input class");
		return false;
	}
	//pmouseX = -1;
	//pmouseY = -1;

	//init texture manager
	m_textureManager = new TextureManager();
	g_textureManager = m_textureManager;
	if (!m_textureManager || !m_textureManager->Initialize()){
		textDump("failed to init texture manager class");
		return false;
	}
	//init model manager
	m_modelManager = new ModelManager();
	g_modelManager = m_modelManager;
	if (!m_modelManager || !m_modelManager->Initialize()){
		textDump("failed to init model manager class");
		return false;
	}
	//init graphics
	m_graphics = new Graphics();
	g_graphics = m_graphics;
	if (!m_graphics || !m_graphics->Initialize(screenWidth, screenHeight, m_hwnd)){
		textDump("failed to init graphics class");
		return false;
	}
	
	//initialize fps
	m_fps = new Fps();
	if (!m_fps || !m_fps->Initialize()){
		textDump("failed to init fps class");
		return false;
	}
	//pfps = -1;
	//initialize timer
	m_timer = new Timer();
	if (!m_timer || !m_timer->Initialize()){
		textDump("failed to init timer class");
		return false;
	}
	
	//init gui
	m_gui = new Gui();
	g_gui = m_gui;
	if (!m_gui || !m_gui->Initialize(m_graphics, "./Assets/gui.txt", screenWidth, screenHeight, m_graphics->GetDevice())){
		textDump("failed to init gui");
		return false;
	}
	//init gamestate manager
	m_gameStateManager = new GameStateManager();
	g_gameStateManager = m_gameStateManager;
	if (!m_gameStateManager || !m_gameStateManager->Initialize(m_input)){
		textDump("failed to initialize game state manager");
		return false;
	}
	MainMenuState * mms = new MainMenuState();
	if (!mms || !mms->Initialize()){
		textDump("failed to initialize main menu state");
		return false;
	}
	m_gameStateManager->add("main menu", mms);
	m_gameStateManager->change("main menu");  //set menu as starting state
	
	TrackState * ts = new TrackState();
	if (!ts || !ts->Initialize()){
		textDump("failed to initialize track state");
		return false;
	}
	g_gameStateManager->add("track", ts);
	
	SelectionState * ss = new SelectionState();
	if (!ss || !ss->Initialize()){
		textDump("failed to initialize selection state");
		return false;
	}
	m_gameStateManager->add("selection", ss);

	TestingState * tests = new TestingState();
	if (!tests || !tests->Initialize()){
		textDump("failed to initialize testing state");
		return false;
	}
	m_gameStateManager->add("testing", tests);

	textDump("finished main load");
	return true;
}

void Gapp::Shutdown() {
	if (m_gameStateManager){
		m_gameStateManager->Shutdown();
		delete m_gameStateManager;
		m_gameStateManager = 0;
	}
	if (m_modelManager){
		m_modelManager->Shutdown();
		delete m_modelManager;
		m_modelManager = 0;
	}
	
	if (m_textureManager){
		m_textureManager->Shutdown();
		delete m_textureManager;
		m_textureManager = 0;
	}
	if (m_gui){
		m_gui->Shutdown();
		delete m_gui;
		m_gui = 0;
	}
	if (m_timer){
		delete m_timer;
		m_timer = 0;
	}
	
	if (m_fps){
		delete m_fps;
		m_fps = 0;
	}
	
	if (m_input){
		m_input->Shutdown();
		delete m_input;
		m_input = 0;
	}
	if (m_graphics){
		m_graphics->Shutdown();
		delete m_graphics;
		m_graphics = 0;
	}
	//shutdown windows
	ShutdownWindows();
}

void Gapp::Run(){
	MSG msg;
	//initialize msg
	ZeroMemory(&msg, sizeof(MSG));

	//loop until we get a quit message
	done = false;
	while (!done){
		//check windows messages and remove it for list
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//if message is quit message
		if (msg.message == WM_QUIT){
			done = true;
		}
		//update time and fps
		m_timer->Frame();
		m_fps->Frame();
		if(!m_gameStateManager->update(m_timer->GetLastFrameTime(), m_fps)){
			done = true;
		}
		m_gameStateManager->render(m_timer->GetLastFrameTime());

	}
}

void Gapp::InitializeWindows(int& x, int& y){
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.
	GappHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Wyrmhole";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	//get screen size
	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN){
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long) x;
		dmScreenSettings.dmPelsHeight = (unsigned long) y;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else {
		//run in windowed mode
		x  = 800;
		y = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - x)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - y) / 2;
	}
	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
				posX, posY, x, y, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}


void Gapp::ShutdownWindows(){
	//remove window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//remove app instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//clear pointer
	GappHandle = NULL;
}

LRESULT CALLBACK Gapp::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam){
	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_input->KeyUp((unsigned int)wparam);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			//tell input that mouse moved and pass point
			m_input->MouseMoved(hwnd, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			//tell input
			m_input->MouseButtonDown(MK_LBUTTON);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			//tell input
			m_input->MouseButtonUp(MK_LBUTTON);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			//tell input
			m_input->MouseButtonDown(MK_RBUTTON);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			//tell input
			m_input->MouseButtonUp(MK_RBUTTON);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			//tell input
			m_input->MouseButtonDown(MK_MBUTTON);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			//tell input
			m_input->MouseButtonUp(MK_MBUTTON);
			return 0;
		}
		//eventually need mouse presses and releases
		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam){
	switch(umessage){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	default:
		return GappHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}