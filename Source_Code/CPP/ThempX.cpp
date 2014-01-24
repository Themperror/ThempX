#include "../Headers/ThempX.h"

//Entry point of the engine (This class is created in main.cpp where the program entrypoint is).
ThempX::ThempX(HWND handle,HINSTANCE hInstance)
{
	oldTicks = GetTickCount();
	handleWindow = handle;
	isDone = false;
	
	//claim graphics device
	p_Device = InitializeDevice(handleWindow);
	
	//instantiate all necessities
	resources = new ResourceManager(p_Device,handle);
	inputHandler = new InputHandler(handleWindow);
	soundHandler = new SoundHandler(handle,44100,16,2);

	WINDOWINFO winInfo;
	
	LoadLevel();
	
	SetUpCamera();

	//Init variables
	Initialize();

	particles.push_back(new Particle(resources,p_Device,"Resources/Particles/Lightning.png",&camera.m_View,D3DXVECTOR3(0,10,10),200,500,1,3));
	particles.at(0)->SetMovement(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,3,0));

	/*for(unsigned int x = 0; x < 15; x++)
	{
		for(unsigned int y = 0; y < 15; y++)
		{
			DebugCube* cube = new DebugCube(p_Device,D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(-1,-1,-1),D3DXVECTOR3(1,1,1),resources);
			debugCubes.push_back(cube);
			cube->collision = new CollisionGeo(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(-1,-1,-1),D3DXVECTOR3(1,1,1));
			cube->AddPositionAndRotation(&collisionLock,2.5f*x,12,2.5f*y,0,0,0);
		}
	}
	boost::thread* t = new boost::thread(&ThempX::CollisionThread,this);  */
	//following switch is for testing collisions
	
	 /*
	switch(cResult)
	{
	case CollisionGeo::Arg1NotCapsule:			std::cout << "Arg1NotCapsule" << std::endl; break;
	case CollisionGeo::Arg1NotSphere:			std::cout << "Arg1NotSphere" << std::endl; break;
	case CollisionGeo::Arg1NotStaticCube:		std::cout << "Arg1NotStaticCube" << std::endl; break;
	case CollisionGeo::Arg1NotOBBCube:			std::cout << "Arg1NotOBBCube" << std::endl; break;
	case CollisionGeo::Arg1NotCylinder:			std::cout << "Arg1NotCylinder" << std::endl; break;
	case CollisionGeo::Arg1NotStaticOrOBBCube:	std::cout << "Arg1NotStaticOrOBBCube" << std::endl; break;
	case CollisionGeo::Arg2NotCapsule:			std::cout << "Arg2NotCapsule" << std::endl; break;
	case CollisionGeo::Arg2NotSphere:			std::cout << "Arg2NotSphere" << std::endl; break;
	case CollisionGeo::Arg2NotStaticCube:		std::cout << "Arg2NotStaticCube" << std::endl; break;
	case CollisionGeo::Arg2NotOBBCube:			std::cout << "Arg2NotOBBCube" << std::endl; break;
	case CollisionGeo::Arg2NotCylinder:			std::cout << "Arg2NotCylinder" << std::endl; break;
	case CollisionGeo::Arg2NotStaticOrOBBCube:	std::cout << "Arg2NotStaticOrOBBCube" << std::endl; break;
	case CollisionGeo::Collision:				std::cout << "Collision Found" << std::endl;  break;
	case CollisionGeo::NoCollision:				std::cout << "No Collision Found" << std::endl; break;
	case CollisionGeo::NotACapsule:				std::cout << "NotACapsule" << std::endl; break;
	case CollisionGeo::NotACylinder:			std::cout << "NotACylinder" << std::endl; break;
	case CollisionGeo::NotASphere:				std::cout << "NotASphere" << std::endl; break;
	case CollisionGeo::NotAOBBCube:				std::cout << "NotAOBBCube" << std::endl; break;
	case CollisionGeo::NotAStaticCube:			std::cout << "NotAStaticCube" << std::endl; break;
	case CollisionGeo::NotAStaticOrOBBCube:		std::cout << "NotAStaticOrOBBCube" << std::endl; break;
	}	   */

	// Game loop starts here after everything is initialized
	g = new Game();

	MSG msg;
    while(!isDone)
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
		inputHandler->Update();
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
	resources->ReleaseResources();
	for(unsigned int i=0;i<modelObjs.size();i++)
	{
		modelObjs.at(i)->ReleaseResources(); 
		delete modelObjs.at(i);
	}
	for(unsigned int i=0;i<spriteObjs.size();i++)
	{
		spriteObjs.at(i)->ReleaseResources();  
		delete spriteObjs.at(i);
	}
	for(unsigned int i=0;i<debugCubes.size();i++)
	{
		debugCubes.at(i)->Release(); 
		delete debugCubes.at(i);
	}
	p_Device->Release();
}

//This function is for the extra thread that is (going) to handle all collisions. 
void ThempX::CollisionThread(void)
{
	while(!isDone)
	{
		for(unsigned int x = 0; x < debugCubes.size(); x++)
		{
			for(unsigned int y = 0 ;y < debugCubes.size();y++)
			{
				if(debugCubes.at(x)->HasCollision(&collisionLock) == true && debugCubes.at(y)->HasCollision(&collisionLock) == true && debugCubes.at(x) != debugCubes.at(y))
				{
					if(debugCubes.at(x)->collision->DidAABBCollideWithAABB(debugCubes.at(y)->collision) == CollisionGeo::Collision)
					{
						if(debugCubes.at(x)->DidCollide(&collisionLock) == false)
						{
							debugCubes.at(x)->ChangeTexture("Resources/Models/CubeRed.png",&collisionLock);
						} 
						if(debugCubes.at(y)->DidCollide(&collisionLock) == false)
						{
							debugCubes.at(y)->ChangeTexture("Resources/Models/CubeRed.png",&collisionLock);
						}
						debugCubes.at(x)->SetCollide(true,&collisionLock);
						debugCubes.at(y)->SetCollide(true,&collisionLock);
							
					}
					else
					{
						if(debugCubes.at(x)->DidCollide(&collisionLock) == false)
						{
							debugCubes.at(x)->ChangeTexture("Resources/Models/CubeGreen.png",&collisionLock);
						} 
						if(debugCubes.at(y)->DidCollide(&collisionLock) == false)
						{
							debugCubes.at(y)->ChangeTexture("Resources/Models/CubeGreen.png",&collisionLock);
						}
					}
				}
			}
		}
		for(unsigned int i = 0; i < debugCubes.size();i++)
		{
			debugCubes.at(i)->SetCollide(false,&collisionLock);
		}
		Sleep(0);
	}
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
		for(unsigned int i =0 ;i<spriteObjs.size(); i++)
		{
			spriteObjs.at(i)->Animate(deltaTime);
		}
		for(unsigned int i = 0; i < particles.size(); i++)
		{
			particles.at(i)->Update(deltaTime);
		}
		if(EditorMode)
		{
			if(currentEditorObj->obj2D != NULL)
			{
				currentEditorObj->obj2D->position = AddVector3(&camera.lookAt,&MultiplyVector3(&camera.lookDir,15));
			}
			else
			{
				currentEditorObj->obj3D->position = AddVector3(&camera.lookAt,&MultiplyVector3(&camera.lookDir,15));
			}
		}
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
		DoInput(fixedDeltaTime);
		DoCameraStuff();
	}
}
//Unused, was used for testing early collisions with camera
bool ThempX::CheckCamBoxCollision(D3DXVECTOR3 pos, Object3D* obj)
{
	//This function will be replaced by the IsInX Functions from CollisionGeo
	
	/*	D3DXVECTOR3 innerPoint = AddVector3(&obj->position,&obj->innerBox); //- values
	D3DXVECTOR3 outerPoint = AddVector3(&obj->position,&obj->outerBox); //+ values

	if(pos.x > innerPoint.x && pos.y > innerPoint.y && pos.z > innerPoint.z && pos.x < outerPoint.x && pos.y < outerPoint.y && pos.z < outerPoint.z)
	{
		return true;
	}  */
	return false;
}

//Left Mouse click function (to test things with), will be removed for engine use, as this is a gameplay feature (will be in Game.cpp)
void ThempX::LeftMouseClick()
{
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			if(!currentEditorObj->obj2D->hasAnimation)
			{
				Object2D* obj = new Object2D(resources,p_Device,currentEditorObj->obj2D->quad.textureName,&camera.m_View);
				obj->SetPosition(currentEditorObj->obj2D->position.x,currentEditorObj->obj2D->position.y,currentEditorObj->obj2D->position.z);
				obj->objName = currentEditorObj->obj2D->objName;
				spriteObjs.push_back(obj);
			}
			else
			{
				Object2D* obj = new Object2D(resources,p_Device,currentEditorObj->obj2D->quad.textureName,&camera.m_View,currentEditorObj->obj2D->GetXSize(),currentEditorObj->obj2D->GetYSize(),currentEditorObj->obj2D->GetXRows(),currentEditorObj->obj2D->GetYRows());
				obj->SetPosition(currentEditorObj->obj2D->position.x,currentEditorObj->obj2D->position.y,currentEditorObj->obj2D->position.z);
				obj->objName = currentEditorObj->obj2D->objName;
				spriteObjs.push_back(obj);
			}
		}
		else
		{
			Object3D* obj = new Object3D(resources,currentEditorObj->obj3D->model.meshPath,p_Device);
			obj->objName = currentEditorObj->obj3D->objName;
			obj->SetPosition(currentEditorObj->obj3D->position.x,currentEditorObj->obj3D->position.y,currentEditorObj->obj3D->position.z);
			modelObjs.push_back(obj);
		}
		CreateLevelFile();
	}
}

//Input Handling (for testing, this needs to be in game.cpp when a game is created)
void ThempX::DoInput(float dT)
{
	if(inputHandler->MouseButtonDown(0))
	{
		if(mouseLeftJustDown == false)
		{
			LeftMouseClick();
			mouseLeftJustDown = true;
		}
	}
	else
	{
		mouseLeftJustDown = false;
	}
	if(inputHandler->MouseButtonDown(1))
	{
		if(mouseRightJustDown == false)
		{
			lockCursor = !lockCursor;
			mouseRightJustDown = true;
		}
	}
	else
	{
		mouseRightJustDown = false;
	}
	int speed = 15;
	if(KeyPressed(DIK_LSHIFT))
	{
		speed = 35;
	}
	else
	{
		speed = 15;
	}
	if(KeyPressed(DIK_Q))
	{
		camera.position.y += dT*speed;
	}
	if(KeyPressed(DIK_E))
	{
		camera.position.y -= dT*speed;
	}
	if(KeyPressed(DIK_W))
	{
		camera.position.x += camera.lookDir.x *dT*speed;
		camera.position.z += camera.lookDir.z *dT*speed;
	}
	if(KeyPressed(DIK_S))
	{
		camera.position.x -= camera.lookDir.x *dT*speed;
		camera.position.z -= camera.lookDir.z *dT*speed;
	}
	if(KeyPressed(DIK_D))
	{
		D3DXVECTOR3 temp = ReturnDirection(angleX-90,angleY);
		camera.position.x += temp.x *dT*speed;
		camera.position.z += temp.z *dT*speed;
	}
	if(KeyPressed(DIK_A))
	{
		D3DXVECTOR3 temp = ReturnDirection(angleX+90,angleY);
		camera.position.x += temp.x *dT*speed;
		camera.position.z += temp.z *dT*speed;
	}

	if(KeyPressed(DIK_0) == 2)
	{
		EditorMode = !EditorMode;
		if(EditorMode == true)
		{
			SetUpEditorMode();
		}
		std::cout <<"Editor Mode has been set to: "<< EditorMode << std::endl;
	}
	if(KeyPressed(DIK_8) == 2)
	{
		currentEditorObjIndex--;
		if(currentEditorObjIndex < 0)
		{
			currentEditorObjIndex = editorObjs.size()-1;
		}
		currentEditorObj = &editorObjs.at(currentEditorObjIndex);
	}
	if(KeyPressed(DIK_9) == 2)
	{
		currentEditorObjIndex++;
		if(currentEditorObjIndex >= editorObjs.size())
		{
			currentEditorObjIndex = 0;
		}
		currentEditorObj = &editorObjs.at(currentEditorObjIndex);
	}

	if(KeyPressed(DIK_SPACE))
	{
		DestroyLevel();	 //This will destroy the current level 
		LoadLevel();	//load the level in level.txt (if edited, the changes will reflect on the world)
	}
	
	if(inputHandler->KeyPressed(DIK_ESCAPE))
	{
		PostQuitMessage(0);
		isDone = true;
	}
}

//Camera rotation handling, also needs to be in game.cpp)
void ThempX::DoCameraStuff()
{
	angleX-=inputHandler->GetMousePosX()*deltaTime*sensitivity;
	(angleY < -50 ? angleY = -50 : (angleY> 50 ? angleY = 50 : 0));
	angleY-=inputHandler->GetMousePosY()*deltaTime*sensitivity;

	SetCameraLookX(angleX,angleY);
}
//returns the camera's looking direction
D3DXVECTOR3 ThempX::ReturnDirection(float anglesX,float anglesY)
{
	D3DXVECTOR3 final;
	D3DXVECTOR3 camPos = camera.position;
    final.x = 0;
    final.y = 0;
    final.z = 0;

    float x = 1 * std::cos(anglesX * 3.141592f / 180);
    float z = 1 * std::sin(anglesX * 3.141592f / 180); 
    float y = 1 * std::tan(anglesY * 3.141592f / 200);

    float fX = camPos.x+x;
    float fY = camPos.y+y;
    float fZ = camPos.z+z;

    final.x = fX - camPos.x;
    final.y = fY - camPos.y;
    final.z = fZ - camPos.z;                                                                                                                                                          

    return final;
}

//used in DoCameraStuff
void ThempX::SetCameraLookX(float anglesX,float anglesY)
{
	float x = 1 * std::cos(anglesX * 3.141592f / 180);
	float z = 1 * std::sin(anglesX * 3.141592f / 180);
	float y = 1 * std::tan(anglesY * 3.141592f / 200);
	camera.lookAt.x = camera.position.x+x;
	camera.lookAt.y = camera.position.y+y;
	camera.lookAt.z = camera.position.z+z;

	camera.lookDir.x = camera.lookAt.x - camera.position.x;
	camera.lookDir.y = camera.lookAt.y - camera.position.y;
	camera.lookDir.z = camera.lookAt.z - camera.position.z;
}

//sets all variables
void ThempX::Initialize()
{
	camera.lookDir.x = 0;
	camera.lookDir.y = 1;
	camera.lookDir.z = 0;
	sensitivity = 5;
	EditorMode = false;
	soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	angleX = 0;
	angleY = 0;
	camera.position.x = 10;
	camera.position.y = 10;
	camera.position.z = 0;
	snapCursor = false;
	camera.lookAt.x = 0;
	camera.lookAt.y = 0;
	camera.lookAt.z = 0;
	keys.resize(256,0);
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
	
	SetUpCamera();

    ////////////////////////

	////////////////////////
	//Draw Objects
	
	p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	for(unsigned int i = 0; i < modelObjs.size();i++)
	{
		modelObjs.at(i)->DrawModel();	
	}
	for(unsigned int i = 0; i < debugCubes.size();i++)
	{
		debugCubes.at(i)->Draw();
	}


	p_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	p_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	for(unsigned int i = 0; i < spriteObjs.size();i++)
	{
		spriteObjs.at(i)->Draw();
	}
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		particles.at(i)->Draw();
	}
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			p_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			p_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			currentEditorObj->obj2D->Draw();
		}
		else
		{
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
			currentEditorObj->obj3D->DrawModel();
		}
	}
	
	////////////////////////

	////////////////////////
	//End drawing
    p_Device->EndScene();
    p_Device->Present(NULL, NULL, NULL, NULL);
	////////////////////////
}

//sets the camera for use
void ThempX::SetUpCamera()
{
	D3DXVECTOR3 m_UpVector(0, 1, 0);
	D3DXMatrixLookAtLH(&camera.m_View, &camera.position, &camera.lookAt, &m_UpVector);
	p_Device->SetTransform(D3DTS_VIEW, &camera.m_View);
	D3DXMatrixPerspectiveFovLH(&camera.m_Projection, D3DX_PI/4, 500/500, 1, 2000);
	p_Device->SetTransform(D3DTS_PROJECTION, &camera.m_Projection);
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

//easy-for-use function for testing keys to be pressed
int ThempX::KeyPressed(int key)
{
	if(inputHandler->KeyPressed(key))
	{
		if(keys[key] == 0)
		{
			keys[key] = 1;
			return 2 ;
		}
		return 1;
	}
	else
	{
		keys[key] = 0;
		return 0;
	}
	return 0;
}

//speaks for itself
void ThempX::DestroyLevel()
{
	for(unsigned int i=0;i<modelObjs.size();i++)
	{
		modelObjs.at(i)->ReleaseResources();
		delete modelObjs.at(i);
	}
	modelObjs.clear(); 
	for(unsigned int i=0;i<spriteObjs.size();i++)
	{
		spriteObjs.at(i)->ReleaseResources();
		delete spriteObjs.at(i);
	}
	spriteObjs.clear();
	for(unsigned int i=0;i<lights.size();i++)
	{
		free(lights.at(i));
		delete lights.at(i);
	}
	lights.clear();
}


void ThempX::SetUpEditorMode()
{
	string line;
	ifstream myfile ("editorresources.txt");
	if (!myfile.good())
	{
		MessageBox(handleWindow,"Could Not Find editorresources.txt, EditorMode won't work", "ThempX()",MB_OK);
		cout << "Cannot find editorresources.txt" << endl;
		return;
	}
	if (myfile.is_open())
	{
		int animatedSpriteNr = 0;
		int staticSpriteNr = 0;
		int modelNr = 0;
		while ( getline (myfile,line) )
		{
			std::cout << "read a line" << std::endl;
			EditorObj obj;
			string name;
			float sizeX,sizeY,xRows,yRows;
			bool hasAnim = false;
			string check = "x";
			myfile >> name >> hasAnim >> sizeX >> sizeY >> xRows >> yRows;
			if(name[name.size()-1] == check[0])
			{
				modelNr++;
				Object3D* modelObj = new Object3D(resources,_strdup(name.c_str()),p_Device);
				std::ostringstream oss;
				oss<<"Model"<<modelNr;
				modelObj->objName = oss.str();
				obj.obj3D = modelObj;
				obj.obj2D = NULL;
				editorObjs.push_back(obj);
			}
			else
			{
				if(hasAnim == false)
				{
					staticSpriteNr++;
					Object2D* spriteObj = new Object2D(resources,p_Device,_strdup(name.c_str()),&camera.m_View);
					std::ostringstream oss;
					oss<<"StaticSprite"<<staticSpriteNr;
					spriteObj->objName = oss.str();
					spriteObj->handleWindow = handleWindow;
					obj.obj2D = spriteObj;
					obj.obj3D = NULL;
					editorObjs.push_back(obj);

				}
				else
				{
					animatedSpriteNr++;
					Object2D* spriteObj = new Object2D(resources,p_Device,_strdup(name.c_str()),&camera.m_View,sizeX,sizeY,xRows,yRows);
					spriteObj->handleWindow = handleWindow;
					std::ostringstream oss;
					oss<<"AnimatedSprite"<<animatedSpriteNr;
					spriteObj->objName = oss.str();
					obj.obj2D = spriteObj;
					obj.obj3D = NULL;
					editorObjs.push_back(obj);
				}
			}
		}
		myfile.close();
		if(editorObjs.size() > 0)
		{
			currentEditorObj = &editorObjs.at(0);
		}
	}
}
void ThempX::CreateLevelFile()
{
	ofstream level("level.txt");
	if (level.is_open())
	{
		level << "name" << "\t"<< "objName" << "\t"<< "posx" << "\t"<< "posy" << "\t"<< "posz" << "\t"<< "scalex" << "\t"<< "scaley" << "\t"<< "scalez" << "\t"<< "rotx" << "\t"<< "roty" << "\t"<< "rotz" << "\t"<< "sizeX" << "\t"<< "sizeY" << "\t"<< "XAnimRows" << "\t"<< "YAnimRows" << "\n";
		for(unsigned int i = 0; i < modelObjs.size();i++)
		{
			Object3D* obj = modelObjs.at(i);
			level << obj->model.meshPath << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" <<0 << "\n";
		}
		for(unsigned int i = 0; i < spriteObjs.size();i++)
		{
			Object2D* obj = spriteObjs.at(i);
			level << obj->quad.textureName << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << obj->GetXSize() << "\t" << obj->GetYSize() << "\t" << obj->GetXRows() << "\t" << obj->GetYRows() << "\n";
		}
		
		level.close();
	}
	else
	{
		std::cout << "Unable to create/open file" << std::endl;
	}
}
//speaks for itself
void ThempX::LoadLevel()
{
	ifstream fin("Resources/level.txt");
	if (!fin.good())
	{
		MessageBox(handleWindow,"Could Not Find Level.txt, scene will be empty", "ThempX()",MB_OK);
		cout << "Cannot find level.txt" << endl;
	}
	else
	{
		string str;
		while(getline(fin, str))
		{
			string name;
			string check = "x";
			string objName;
			float XAnimRows;
			float YAnimRows;
			float sizeX;
			float sizeY;
			float posx,posy,posz,scalex,scaley,scalez,rotx,roty,rotz;
			fin >> name >> objName >>posx >> posy >> posz >> scalex >> scaley >> scalez >> rotx >> roty >> rotz >> sizeX >> sizeY >> XAnimRows >> YAnimRows;
			if(name.at(name.size()-1) == check[0])
			{
				Object3D* obj = new Object3D(resources,_strdup(name.c_str()),p_Device);
				obj->objName = objName;
				obj->SetPosition(posx,posy,posz);
				if(rotx != 0 && roty != 0 && rotz != 0)
				{
					obj->SetRotation(rotx,roty,rotz);
				}
				if(scalex != 1 && scaley != 1 && scalex != 1)
				{
					obj->SetScale(scalex,scaley,scalez);
				}
				modelObjs.push_back(obj);
			}
			else
			{
				if(XAnimRows == 0 && YAnimRows == 0)
				{
					Object2D* obj = new Object2D(resources,p_Device,_strdup(name.c_str()),&camera.m_View);
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->SetPosition(posx,posy,posz);
					if(scalex != 1 && scaley != 1 && scalex != 1)
					{
						obj->SetScale(scalex,scaley,scalez);
					}
					spriteObjs.push_back(obj);

				}
				else
				{
					Object2D* obj = new Object2D(resources,p_Device,_strdup(name.c_str()),&camera.m_View,sizeX,sizeY,XAnimRows,YAnimRows);
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->SetPosition(posx,posy,posz);
					if(scalex != 1 && scaley != 1 && scalex != 1)
					{
						obj->SetScale(scalex,scaley,scalez);
					}
					spriteObjs.push_back(obj);
				}
			}
		}
		fin.close();
		CreateLevelFile();
	}
}

//unused light function
D3DLIGHT9* ThempX::CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff)
{
	D3DLIGHT9* light = new D3DLIGHT9();
	light->Diffuse = lightColor;
	light->Position = position;
	light->Range = range;
	light->Falloff = falloff;
	light->Type = lightType;
	lights.push_back(light);
	return light;
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