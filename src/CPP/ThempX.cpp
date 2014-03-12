#include "../Headers/ThempX.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void ThempX::SetDisplayModeWindowed(int sizeX, int sizeY, int renderSizeX,int renderSizeY)
{
	
	data.applicationActive = false;

	D3DPRESENT_PARAMETERS dx_PresParams;
	data.windowed = true;
	oldWSizeX = wSizeX;
	oldWSizeY = wSizeY;
	wSizeX = sizeX;
	wSizeY = sizeY;
	ZeroMemory( &dx_PresParams, sizeof(dx_PresParams)); 
	
	int dHorizontal = 0;
	int dVertical = 0;
	GetDesktopResolution(dHorizontal, dVertical);

	SetWindowPos(handleWindow,HWND_TOPMOST,dHorizontal/2-(sizeX/2),dVertical/2-(sizeY/2),sizeX,sizeY, NULL);

	dx_PresParams.Windowed = TRUE;
	dx_PresParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	dx_PresParams.BackBufferHeight = renderSizeY;
	dx_PresParams.BackBufferWidth = renderSizeX;

	dx_PresParams.hDeviceWindow = handleWindow;
	dx_PresParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dx_PresParams.BackBufferCount = 1;
	dx_PresParams.MultiSampleQuality = 0;
	dx_PresParams.MultiSampleType = D3DMULTISAMPLE_NONE;
	dx_PresParams.EnableAutoDepthStencil = TRUE;
	dx_PresParams.AutoDepthStencilFormat = D3DFMT_D16;

	if(p_Device->Reset(&dx_PresParams) == D3D_OK)
	{
		SetDefaultRenderStateSettings();
		g->ReloadGUI();
	}

	resources->SetScreenResolution(renderSizeX,renderSizeY);
	data.d3dxpresentationparams = dx_PresParams;
}
void ThempX::SetDisplayModeFullScreen(int devmodeIndex)
{
	data.applicationActive = false;
	DEVMODE devmode = resources->GetDevMode(devmodeIndex);
	
	std::cout << "settings fullscreen display mode to " << devmode.dmPelsWidth << "x" << devmode.dmPelsHeight << " with a rendering size of " << devmode.dmPelsWidth << "x" << devmode.dmPelsHeight << std::endl;

	D3DPRESENT_PARAMETERS dx_PresParams;
	data.windowed = false;
	oldWSizeX = wSizeX;
	oldWSizeY = wSizeY;
	wSizeX = devmode.dmPelsWidth;
	wSizeY = devmode.dmPelsHeight;

	ZeroMemory( &dx_PresParams, sizeof(dx_PresParams)); 
	
	int dHorizontal = 0;
	int dVertical = 0;
	
	GetDesktopResolution(dHorizontal, dVertical);
	//WS_EX_TOPMOST | WS_POPUPGetDesktopResolution(dHorizontal, dVertical);
	if(ChangeDisplaySettings(&resources->GetDevMode(devmodeIndex),CDS_FULLSCREEN | CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
	{
		std::cout << "Changing display mode failed" << std::endl;
		return;
	}
	ChangeDisplaySettings(&resources->GetDevMode(devmodeIndex),CDS_FULLSCREEN);

	Sleep(200);
	//SetWindowLong(handleWindow,0,WS_EX_TOPMOST | WS_POPUP);
	SetWindowPos(handleWindow,HWND_TOPMOST,0,0, wSizeX,wSizeY, SWP_NOZORDER | SWP_SHOWWINDOW);

	dx_PresParams.BackBufferHeight = wSizeY;
	dx_PresParams.BackBufferWidth = wSizeX;
	dx_PresParams.Windowed = FALSE;
	dx_PresParams.FullScreen_RefreshRateInHz = devmode.dmDisplayFrequency;
	dx_PresParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	
	dx_PresParams.hDeviceWindow = handleWindow;
	dx_PresParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dx_PresParams.BackBufferCount = 1;
	dx_PresParams.MultiSampleQuality = 0;
	dx_PresParams.MultiSampleType = D3DMULTISAMPLE_NONE;
	dx_PresParams.EnableAutoDepthStencil = TRUE;
	dx_PresParams.AutoDepthStencilFormat = D3DFMT_D16;
	
	if(p_Device->Reset(&dx_PresParams) == D3D_OK)
	{
		SetDefaultRenderStateSettings();
		g->ReloadGUI();
	}
	
	data.d3dxpresentationparams = dx_PresParams;
	resources->SetScreenResolution(wSizeX,wSizeY);	
}
void ThempX::GetListofDisplayModes()
{
	DISPLAY_DEVICE device;
	device.cb = sizeof(DISPLAY_DEVICE);
	if(EnumDisplayDevices(NULL,0,&device,EDD_GET_DEVICE_INTERFACE_NAME))
	{
		DEVMODE mode;
		mode.dmSize = sizeof(DEVMODE);
		int i = 0;
		while(EnumDisplaySettings(device.DeviceName,i,&mode))
		{
			i++;
			if(mode.dmBitsPerPel == 32)
			{
				resources->PushDevmode(mode);
			}
		}
		/*D3DDISPLAYMODE mode;
		std::cout << "capable screen modes are: "<<std::endl;
		IDirect3D9* dev;
		p_Device->GetDirect3D(&dev);
		for(unsigned int i =0;i < dev->GetAdapterModeCount(0,D3DFMT_X8R8G8B8);i++)
		{
			dev->EnumAdapterModes(0,D3DFMT_X8R8G8B8, i,&mode);
			std::cout<<mode.Width << "x" << mode.Height << std::endl;
		}*/
	
	}
	else
	{
		MessageBox(handleWindow,"No display devices found","GetListofDisplayModes",MB_OK);
		data.loop = false;
		return;
	}
}
HWND ThempX::NewWindow(LPCTSTR windowName,int posX,int posY, int sizeX,int sizeY,bool isWindowed)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	wc.lpszClassName = "ThempX";
	wc.lpszMenuName = NULL;
	wc.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
		
	RegisterClassEx(&wc);
	
	if(isWindowed)
	{
		return CreateWindowEx(WS_EX_CONTROLPARENT, "ThempX", windowName,  WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_POPUP | WS_VISIBLE, posX, posY, sizeX, sizeY, NULL, NULL, GetModuleHandle(NULL), NULL);
	}
	else
	{
		return CreateWindowEx(WS_EX_CONTROLPARENT, "ThempX", windowName, WS_EX_TOPMOST | WS_POPUP, 0, 0, sizeX, sizeY, NULL, NULL,GetModuleHandle(NULL), NULL);
	}
}

void ThempX::GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}


void ThempX::PreCreateWindow()
{
	data.windowed = true;
	int dHorizontal = 0;
	int dVertical = 0;
	
	GetDesktopResolution(dHorizontal, dVertical);
	
	wSizeX = 800;
	wSizeY = 600;

	handleWindow = NewWindow("The Morph",dHorizontal/2-(wSizeX/2),dVertical/2-(wSizeY/2),wSizeX,wSizeY,true);


	AllocConsole();
	freopen("CONOUT$", "w", stdout);

}

//Entry point of the engine (This class is created in main.cpp where the program entrypoint is).
ThempX::ThempX(HINSTANCE hInstance,LPSTR lpCmdLine)
{
	PreCreateWindow();
	p_Device = InitializeDevice(handleWindow);
	SetDefaultRenderStateSettings();

	//Render loading screen
	CreateLoadingScreen();
	//SetDisplayMode(false,800,600);
	//instantiate all necessities
	resources = new ResourceManager(p_Device,handleWindow);
	resources->SetScreenResolution((float)wSizeX,(float)wSizeY);
	inputHandler = new InputHandler(handleWindow);
	soundHandler = new SoundHandler(handleWindow,44100,16,2);
	
	GetListofDisplayModes();
	//SetDisplayMode(false,320,200,320,200);

	WINDOWINFO winInfo;
	ZeroMemory(&data,sizeof(data));
	data.windowed = true;
	data.lockCursor = true;
	data.loop = true;
	data.applicationActive = true;
	data.changeDisplay = false;
	data.windowSizeX = wSizeX;
	data.windowSizeY = wSizeY;
	// Game loop starts here after everything is initialized

	std::cout << "Initialized OK, starting game..." << std::endl;

	g = new Game(&data,handleWindow,resources,inputHandler,soundHandler,p_Device);

	
	std::cout << "Game created, now starting game loop..." << std::endl;

	MSG msg;

	unsigned int frames = 0;
	int realframes = 0;
    DWORD cTicks = 0;
	DWORD oTicks = 0;
	while(data.loop)
    {
		
		QueryPerformanceFrequency(&currentFrequency);
		QueryPerformanceCounter(&currentTicks);
		currentTicks.QuadPart = currentTicks.QuadPart*10000 / currentFrequency.QuadPart;

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);	
			if(msg.message == WM_QUIT)
			{
				data.loop = false;
				DestroyWindow(handleWindow);
			}
			if(msg.message == WM_ACTIVATEAPP)
			{
				int dHorizontal = 0;
				int dVertical = 0;
				GetDesktopResolution(dHorizontal, dVertical);
				SetCursorPos(dHorizontal/2,dVertical/2);
				if(msg.wParam == TRUE)
				{
					//application being brought back from alt-tabbed state (window with lost focus)
					cout << "window restored" << endl;
					data.applicationActive = true;
					CheckDevice();
				}
			}	
        }
		if(data.changeDisplay) //must stay above the testcooperative function of p_Device
		{
			data.changeDisplay = false;
			if(data.windowed == true)
			{
				SetDisplayModeWindowed(data.windowSizeX,data.windowSizeY,data.renderSizeX,data.renderSizeY);
			}
			else
			{
				SetDisplayModeFullScreen(data.devmodeIndex);
			}
		}
		if(p_Device->TestCooperativeLevel() != D3D_OK)
		{
			CheckDevice();
			data.applicationActive = false;
		}
		else
		{
			data.applicationActive = true;
		}
		
		if(data.applicationActive)
		{
			if(data.lockCursor)
			{
				GetWindowInfo(handleWindow,&winInfo);
				int windowSizeX = winInfo.rcWindow.right-winInfo.rcWindow.left;
				int windowSizeY = winInfo.rcWindow.bottom-winInfo.rcWindow.top;
				int windowPosX = winInfo.rcWindow.left;
				int windowPosY = winInfo.rcWindow.top;
				SetCursorPos(windowPosX+(windowSizeX/2),windowPosY+(windowSizeY/2));
			}
			
			FixedUpdate();
			realframes++;

			

			cTicks = currentTicks.QuadPart;
			if(cTicks-oTicks > 10000+((cTicks-oTicks) % 10000))
			{
				std::cout << "FixedUpdate running at "<<realframes <<" times per second"<< std::endl;
				std::cout << "FPS: " << frames << std::endl;
				frames = 0;
				realframes = 0;
				oTicks = cTicks;
			}
			if(currentTicks.QuadPart > oldTicks.QuadPart + 166) 
			//why does 166 result in 120 fps limit on fast machines and 60 fps limit on slower machines? 1/60 = 0.0166
			//332 is 60 FPS and unlimited fixed update
			//166 is 120 fps and unlimited fixed update
			//83 = 120 FPS and limited fixed update to 120 times per second
			{
				//std::cout << currentTicks.QuadPart << " <-currentTicks   " << oldTicks.QuadPart <<" <- oldTicks" << std::endl; 
				frames++;
				/*if(p_Device->TestCooperativeLevel() == D3D_OK)
				{
					Update();
					DrawScene();
				}*/ //guarded, due to the nature of alt-tabbing and switching screen modes, this was a security to ensure no errors would come from a lost d3ddevice
				Update();
				DrawScene();
				oldTicks = currentTicks;
			}
		}
		else
		{
			Sleep(100);
		}
    }

	//t.join();
	//release everything
	resources->ReleaseResources();
	soundHandler->Shutdown();
	g->ReleaseAll();
	delete inputHandler;
	delete resources;
	delete g;
	delete soundHandler;
	
	//Sleep(2000);
	p_Device->Release();
	PostQuitMessage(0);

	DestroyWindow(handleWindow);
}
void ThempX::CheckDevice()
{
	HRESULT result = p_Device->TestCooperativeLevel();
	if(result == D3DERR_DEVICELOST)
	{
		data.lockCursor = false;
		resources->LostDeviceAllText();
		return;
	}
	if(result == D3DERR_DEVICENOTRESET)
	{
		resources->LostDeviceAllText();
		while(result != D3D_OK)
		{
			result = p_Device->Reset(&data.d3dxpresentationparams);
			if(result == D3D_OK)
			{
				SetDefaultRenderStateSettings();
				g->ReloadGUI();
			}
			Sleep(200);
		}
	//	CheckDevice();
		return;
	}
	if(result == D3DERR_DRIVERINTERNALERROR)
	{
		data.applicationActive = false;
		data.loop = false;
		return;
	}
	if(result == D3D_OK)
	{
		WINDOWINFO winInfo;
		GetWindowInfo(handleWindow,&winInfo);
		int windowSizeX = winInfo.rcWindow.right-winInfo.rcWindow.left;
		int windowSizeY = winInfo.rcWindow.bottom-winInfo.rcWindow.top;
		int windowPosX = winInfo.rcWindow.left;
		int windowPosY = winInfo.rcWindow.top;
		SetCursorPos(windowPosX+(windowSizeX/2),windowPosY+(windowSizeY/2));
		data.applicationActive = true;
		data.loop = true;
		return;
	}
}
//Update, this will run every frame (61 fps max)
void ThempX::Update()
{
	oldDelta = newDelta;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&newDelta);
	newDelta.QuadPart = newDelta.QuadPart*10000 / frequency.QuadPart;

	double delta;
	delta = (double)(oldDelta.QuadPart - newDelta.QuadPart);
	delta = abs(delta/10000);
	//cout << delta << ".\n";
	g->Update(delta);
}
//FixedUpdate, this will run every iteration of the main game loop
void ThempX::FixedUpdate()
{
	oldFixedDelta = newFixedDelta;
	QueryPerformanceFrequency(&fixedFrequency);
	QueryPerformanceCounter(&newFixedDelta);
	newFixedDelta.QuadPart = newFixedDelta.QuadPart*10000 / fixedFrequency.QuadPart;
	double fixedDelta;
	fixedDelta = (double)(oldFixedDelta.QuadPart - newFixedDelta.QuadPart) ;
	fixedDelta = abs(fixedDelta/10000);
	//cout << fixedDelta << " ms.\n";
	g->FixedUpdate(fixedDelta);
}

//sets all variables
void ThempX::Initialize()
{
	
}

//draws the scene
void ThempX::DrawScene()
{
	////////////////////////
	// Clear the scene and Z buffer
    p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	p_Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    p_Device->BeginScene();


	g->Render();
	

	////////////////////////
	//End drawing
    p_Device->EndScene();
    p_Device->Present(NULL, NULL, NULL, NULL);
	////////////////////////
}


//claims the graphics device
LPDIRECT3DDEVICE9 ThempX::InitializeDevice(HWND han_WindowToBindTo) 
{
	//////////////////////////////////////////
	//Check if the computer has the correct DirectX version (or higher) else show an error and exit the program after
	LPDIRECT3D9 p_dx_Object = NULL;
	p_dx_Object = Direct3DCreate9(D3D_SDK_VERSION);
	if (p_dx_Object == NULL)
	{
		MessageBox(han_WindowToBindTo,"DirectX Runtime library not installed!","InitializeDevice()",MB_OK);
		data.loop = false;
		return 0;
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//Initialize DX9 parameters
	D3DPRESENT_PARAMETERS dx_PresParams;
 
	ZeroMemory( &dx_PresParams, sizeof(dx_PresParams)); 
	if(data.windowed)
	{
		dx_PresParams.Windowed = TRUE;
		dx_PresParams.BackBufferFormat = D3DFMT_UNKNOWN;
	}
	else
	{
		dx_PresParams.Windowed = FALSE;
		dx_PresParams.FullScreen_RefreshRateInHz = 60;
		dx_PresParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	}
	dx_PresParams.BackBufferHeight = wSizeY;
	dx_PresParams.BackBufferWidth = wSizeX;
	dx_PresParams.hDeviceWindow = handleWindow;
	dx_PresParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dx_PresParams.BackBufferCount = 1;
	dx_PresParams.MultiSampleQuality = 0;
	dx_PresParams.MultiSampleType = D3DMULTISAMPLE_NONE;
	dx_PresParams.EnableAutoDepthStencil = TRUE;
	dx_PresParams.AutoDepthStencilFormat = D3DFMT_D16;

	//////////////////////////////////////////

	//////////////////////////////////////////
	// Try claiming the graphics device in hardware mode
	HRESULT result;
	LPDIRECT3DDEVICE9 p_dx_Device = NULL;

	result = p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, handleWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device);
	switch(result)
	{
	case D3DERR_DEVICELOST: 
		MessageBox(han_WindowToBindTo,"Error: Device Lost","InitializeDevice()",MB_OK);
		break;
	case D3DERR_INVALIDCALL:
		MessageBox(han_WindowToBindTo,"Error: Invalid Call","InitializeDevice()",MB_OK);
		break; 
	case D3DERR_NOTAVAILABLE:
		MessageBox(han_WindowToBindTo,"Error: No Device Available","InitializeDevice()",MB_OK);
		break;  
	case D3DERR_OUTOFVIDEOMEMORY: 
		MessageBox(han_WindowToBindTo,"Error: Out of Video Memory","InitializeDevice()",MB_OK);
		break;
	}
	if(p_dx_Device == NULL) // Device claiming didn't work, let's try in software mode
	{
		result = p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, handleWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device);
		switch(result) //if theres any error, exit the program as we couldn't claim a graphics device
		{
		case D3D_OK: 
			MessageBox(han_WindowToBindTo,"Running in software mode","InitializeDevice()",MB_OK);
			break;
		case D3DERR_DEVICELOST: 
			MessageBox(han_WindowToBindTo,"Error: Device Lost (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			data.loop = false;
			return 0;
			break;
		case D3DERR_INVALIDCALL:
			MessageBox(han_WindowToBindTo,"Error: Invalid Call (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			data.loop = false;
			return 0;
			break; 
		case D3DERR_NOTAVAILABLE:
			MessageBox(han_WindowToBindTo,"Error: No Device Available (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			data.loop = false;
			return 0;
			break;  
		case D3DERR_OUTOFVIDEOMEMORY: 
			MessageBox(han_WindowToBindTo,"Error: Out of Video Memory (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			data.loop = false;
			return 0;
			break;
		}
	}
	

	//////////////////////////////////////////
	//Everything worked correctly, now exiting function
	return p_dx_Device;
	//////////////////////////////////////////
}
void ThempX::SetDefaultRenderStateSettings()
{
	p_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); //double sided plane
//p_dx_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); //single sided plane
	p_Device->SetRenderState(D3DRS_LIGHTING,false);
	p_Device->SetRenderState(D3DRS_ZENABLE, true);
	p_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	p_Device->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,1);
	p_Device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
	p_Device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
	p_Device->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
	p_Device->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x0000008f);
	p_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
	p_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	p_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	p_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
}


/* 
for(unsigned int i = 0; i < spriteObjs.size();i++)
{
if(Vector3Distance(&spriteObjs.at(i)->position,&camera.position) < 150)
{
BOOL hit = FALSE;
FLOAT distToHit = 100;

float x = 1 * std::cos((angleX+90) * 3.141592f / 180);
float z = 1 * std::sin((angleX+90) * 3.141592f / 180);
D3DXVECTOR3 v1 = D3DXVECTOR3((-2.5f	*spriteObjs.at(i)->scaling.x / 2)* x	+ spriteObjs.at(i)->position.x	,( 2.5f *spriteObjs.at(i)->scaling.y)	+spriteObjs.at(i)->position.y		,(-2.5f	*spriteObjs.at(i)->scaling.z / 2)* z + spriteObjs.at(i)->position.z);//
D3DXVECTOR3 v2 = D3DXVECTOR3(( 2.5f	*spriteObjs.at(i)->scaling.x / 2)* x	+ spriteObjs.at(i)->position.x	,( 2.5f *spriteObjs.at(i)->scaling.y)	+spriteObjs.at(i)->position.y		,( 2.5f	*spriteObjs.at(i)->scaling.z / 2)* z + spriteObjs.at(i)->position.z);
D3DXVECTOR3 v3 = D3DXVECTOR3((-2.5f	*spriteObjs.at(i)->scaling.x / 2)* x	+ spriteObjs.at(i)->position.x	,( -2.5f *spriteObjs.at(i)->scaling.y)	+spriteObjs.at(i)->position.y		,(-2.5f	*spriteObjs.at(i)->scaling.z / 2)* z + spriteObjs.at(i)->position.z);
D3DXVECTOR3 v4 = D3DXVECTOR3(( 2.5f	*spriteObjs.at(i)->scaling.x / 2)* x	+ spriteObjs.at(i)->position.x	,( -2.5f *spriteObjs.at(i)->scaling.y)	+spriteObjs.at(i)->position.y		,( 2.5f	*spriteObjs.at(i)->scaling.z / 2)* z + spriteObjs.at(i)->position.z);
if(D3DXIntersectTri(&v1,&v2,&v3,&camera.position,&camera.lookDir,NULL,NULL,&distToHit)) 
{
cout << "hit target: "<<spriteObjs.at(i)->objName << "  at distance: " << distToHit << endl;
}
if(D3DXIntersectTri(&v2,&v3,&v4,&camera.position,&camera.lookDir,NULL,NULL,&distToHit)) 
{
cout << "hit target: "<<spriteObjs.at(i)->objName << "  at distance: " << distToHit << endl;
}
}
}

*/
void ThempX::CreateLoadingScreen()
{

	GUI::Vertex2D triangleVerts[] = 
	{
		{0,0,0,1,D3DXVECTOR2(0,0)},
		{(float)wSizeX,0,0,1,D3DXVECTOR2(1,0)},
		{0,(float)wSizeY,0,1,D3DXVECTOR2(0,1)},
		{(float)wSizeX,(float)wSizeY,0,1,D3DXVECTOR2(1,1)},
	};
	LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

	HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(GUI::Vertex2D), 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
	switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Invalid Call while creating VertexBuffer","FillVertices()",MB_OK);
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating VertexBuffer","FillVertices()",MB_OK);
		break;
	case E_OUTOFMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Memory while creating VertexBuffer","FillVertices()",MB_OK);
		break;
	}
	void* p_Vertices;
	result = p_dx_VertexBuffer->Lock(0, 4*sizeof(GUI::Vertex2D), (void**)&p_Vertices, 0);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Error trying to lock","FillVertices()",MB_OK);
		break;
	}//we konden de vertexbuffer locken dus ga door
	memcpy(p_Vertices, triangleVerts, 4*sizeof(GUI::Vertex2D));
	p_dx_VertexBuffer->Unlock();

	 short s_Indices[4];
    s_Indices[0]=0;
    s_Indices[1]=1;
    s_Indices[2]=2;
	s_Indices[3]=3;


	LPDIRECT3DINDEXBUFFER9 p_dx_IndexBuffer = NULL;
	result = p_Device->CreateIndexBuffer(4*sizeof(short), D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&p_dx_IndexBuffer,NULL);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Invalid Call while creating IndexBuffer","FillIndices()",MB_OK);
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating IndexBuffer","FillIndices()",MB_OK);
		break;
	case E_OUTOFMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Memory while creating IndexBuffer","FillIndices()",MB_OK);
		break;
	}

	void* p_Indices;
	result = p_dx_IndexBuffer->Lock(0, 4*sizeof(short), (void**)&p_Indices, 0);
	 
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Error trying to lock","FillIndices()",MB_OK);
		break;
	}
	memcpy(p_Indices, s_Indices, 4*sizeof(short));
	p_dx_IndexBuffer->Unlock();

	LPDIRECT3DTEXTURE9 texture;
	D3DXCreateTextureFromFile(p_Device,"resources/sprites/loading.jpg",&texture);

	D3DXMATRIX world;
		
	D3DXMatrixTranslation(&world,0,0,0);
		
	p_Device->BeginScene();

    p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	p_Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	result = p_Device->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	result = p_Device->SetStreamSource(0, p_dx_VertexBuffer, 0, sizeof(GUI::Vertex2D));
	result = p_Device->SetIndices(p_dx_IndexBuffer);
	
	D3DXMATRIX projection;
	D3DXMatrixOrthoLH(&projection,(float)wSizeX,(float)wSizeY,0,1);
	p_Device->SetTransform(D3DTS_PROJECTION, &projection);

	result = p_Device->SetTexture(0,texture);
	result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);

	p_Device->EndScene();
    p_Device->Present(NULL, NULL, NULL, NULL);

	texture->Release();
	p_dx_VertexBuffer->Release();
	p_dx_IndexBuffer->Release();
	return;
}