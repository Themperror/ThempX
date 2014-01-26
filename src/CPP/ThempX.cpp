#include "../Headers/ThempX.h"

//Entry point of the engine (This class is created in main.cpp where the program entrypoint is).
ThempX::ThempX(HWND handle,HINSTANCE hInstance)
{
	oldTicks = GetTickCount();
	handleWindow = handle;
	isDone = false;
	lockCursor = false;
	//claim graphics device
	p_Device = InitializeDevice(handleWindow);
	
	//instantiate all necessities
	resources = new ResourceManager(p_Device,handle);
	inputHandler = new InputHandler(handleWindow);
	soundHandler = new SoundHandler(handle,44100,16,2);

	WINDOWINFO winInfo;
	Game::TrueStruct loop;
	loop.val = true;
	// Game loop starts here after everything is initialized
	g = new Game(&loop,handleWindow,resources,inputHandler,soundHandler,p_Device);

	MSG msg;
    while(loop.val)
    {
		if(lockCursor)
		{
			GetWindowInfo(handle,&winInfo);
			windowSizeX = winInfo.rcWindow.right-winInfo.rcWindow.left;
			windowSizeY = winInfo.rcWindow.bottom-winInfo.rcWindow.top;
			windowPosX = winInfo.rcWindow.left;
			windowPosY = winInfo.rcWindow.top;
			SetCursorPos(windowPosX+(windowSizeX/2),windowPosY+(windowSizeY/2));
		}
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);		
        }
		if(msg.message == WM_QUIT)
		{
			isDone = true;
			DestroyWindow(handleWindow);
		}		

		currentTicks = GetTickCount();
		FixedUpdate();
		if(currentTicks > oldTicks+16)
		{
			Update();
			DrawScene();
			oldTicks = currentTicks;
		}
		//Sleep(0);
    }


	//t->join();
	//release everything
	PostQuitMessage(0);
	resources->ReleaseResources();
	g->ReleaseAll();
	p_Device->Release();
}

//Update, this will run every frame (61 fps max)
void ThempX::Update()
{
	oldDelta = newDelta;
	newDelta = GetTickCount();
	DWORD tempDeltaTime = newDelta - oldDelta;
	if(tempDeltaTime < 100)
	{
		deltaTime = tempDeltaTime*0.001f;
		g->Update(deltaTime);
	}
}
//FixedUpdate, this will run every iteration of the main game loop
void ThempX::FixedUpdate()
{
	oldFixedDelta = newFixedDelta;
    newFixedDelta = GetTickCount();
    DWORD tempDeltaTime = newFixedDelta - oldFixedDelta;
	if(tempDeltaTime < 100)
	{
		fixedDeltaTime = tempDeltaTime*0.001f;
		g->FixedUpdate(fixedDeltaTime);
	}
}


//sets all variables
void ThempX::Initialize()
{
	lockCursor = false;
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
		isDone = true; 
		return 0;
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//Initialize DX9 parameters
	D3DPRESENT_PARAMETERS dx_PresParams;
 
	ZeroMemory( &dx_PresParams, sizeof(dx_PresParams)); 
	dx_PresParams.Windowed = TRUE;
	dx_PresParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dx_PresParams.BackBufferFormat = D3DFMT_UNKNOWN;
	dx_PresParams.BackBufferCount = 1;
	dx_PresParams.EnableAutoDepthStencil = TRUE;
	dx_PresParams.AutoDepthStencilFormat = D3DFMT_D16;

	//////////////////////////////////////////

	//////////////////////////////////////////
	// Try claiming the graphics device in hardware mode
	HRESULT result;
	LPDIRECT3DDEVICE9 p_dx_Device = NULL;

	result = p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, han_WindowToBindTo, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device);
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
		result = p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, han_WindowToBindTo, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device);
		switch(result) //if theres any error, exit the program as we couldn't claim a graphics device
		{
		case D3D_OK: 
			MessageBox(han_WindowToBindTo,"Running in software mode","InitializeDevice()",MB_OK);
			break;
		case D3DERR_DEVICELOST: 
			MessageBox(han_WindowToBindTo,"Error: Device Lost (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			isDone = true;
			return 0;
			break;
		case D3DERR_INVALIDCALL:
			MessageBox(han_WindowToBindTo,"Error: Invalid Call (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			isDone = true;
			return 0;
			break; 
		case D3DERR_NOTAVAILABLE:
			MessageBox(han_WindowToBindTo,"Error: No Device Available (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			isDone = true;
			return 0;
			break;  
		case D3DERR_OUTOFVIDEOMEMORY: 
			MessageBox(han_WindowToBindTo,"Error: Out of Video Memory (Software Mode), Quitting engine","InitializeDevice()",MB_OK);
			isDone = true;
			return 0;
			break;
		}
	}
	
	//////////////////////////////////////////
	p_dx_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); //double sided plane
	//p_dx_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); //single sided plane
    p_dx_Device->SetRenderState(D3DRS_LIGHTING,false);
	p_dx_Device->SetRenderState(D3DRS_ZENABLE, true);
	p_dx_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	
	p_dx_Device->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x0000008f);
	p_dx_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
	p_dx_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	p_dx_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	p_dx_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);

	//////////////////////////////////////////
	//Everything worked correctly, now exiting function
	return p_dx_Device;
	//////////////////////////////////////////
}



/*  //raycasting for cubes and sprites
for(unsigned int i = 0; i < debugCubes.size();i++)
{
DebugCube* obj = debugCubes.at(i);

if(obj->collision != NULL && Vector3Distance(&obj->position,&camera.position) < 100)
{	
if(obj->collision->GetType() == CollisionGeo::OBBCube || obj->collision->GetType() == CollisionGeo::StaticCube)
{
D3DXVECTOR3 LLFPos = AddVector3(&obj->collision->GetLowerLeftFrontPos(),&obj->position);
D3DXVECTOR3 URBPos = AddVector3(&obj->collision->GetUpperRightBackPos(),&obj->position);

std::vector<D3DXVECTOR3> vertices;

vertices.push_back(D3DXVECTOR3(LLFPos.x, LLFPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, LLFPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, URBPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, URBPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, LLFPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, URBPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, LLFPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, URBPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, URBPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, URBPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, URBPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, URBPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, LLFPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, LLFPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, LLFPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, LLFPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, LLFPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, URBPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, LLFPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(URBPos.x, URBPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, LLFPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, LLFPos.y, URBPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, URBPos.y, LLFPos.z));
vertices.push_back(D3DXVECTOR3(LLFPos.x, URBPos.y, URBPos.z));

float distToHit = 0;
for(unsigned int x=0;x < 36; x+=3)
{
if(D3DXIntersectTri(&vertices[obj->cubeIndices[x]],&vertices[obj->cubeIndices[x+1]],&vertices[obj->cubeIndices[x+2]],&camera.position,&camera.lookDir,NULL,NULL,&distToHit)) 
{
cout << "hit target debugcube, NR: "<<i << "  at distance: " << distToHit << endl;
}
}
} 
}
}
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