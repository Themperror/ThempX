#include "ThempX.h"

ThempX::ThempX(HWND handle,HINSTANCE hInstance)
{
	handleWindow = handle;
	isDone = false;
	//Init de engine
	p_Device = InitializeDevice(handleWindow);
	resources = new ResourceManager(p_Device,handle);
	inputHandler = new InputHandler(handleWindow);
	soundHandler = new SoundHandler(handle,44100,16,2);
	WINDOWINFO winInfo;
	
	//p_VertexBuffer = FillVertices();
	//p_IndexBuffer = FillIndices();
	
	LoadLevel();
	
	SetUpCamera();

	//Init variabelen
	Initialize();

	debugCubes.push_back(new DebugCube(p_Device,D3DXVECTOR3(0,5,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(-2,-2,-2),D3DXVECTOR3(2,2,2),resources));

	



	/*	switch(cResult)
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
	case CollisionGeo::Collision:				std::cout << "Collision Found" << std::endl; break;
	case CollisionGeo::NoCollision:				std::cout << "No Collision Found" << std::endl; break;
	case CollisionGeo::NotACapsule:				std::cout << "NotACapsule" << std::endl; break;
	case CollisionGeo::NotACylinder:			std::cout << "NotACylinder" << std::endl; break;
	case CollisionGeo::NotASphere:				std::cout << "NotASphere" << std::endl; break;
	case CollisionGeo::NotAOBBCube:				std::cout << "NotAOBBCube" << std::endl; break;
	case CollisionGeo::NotAStaticCube:			std::cout << "NotAStaticCube" << std::endl; break;
	case CollisionGeo::NotAStaticOrOBBCube:		std::cout << "NotAStaticOrOBBCube" << std::endl; break;
	}		*/



	//std::cout<< "Everything initialized correctly without errors, now drawing" << std::endl;
	//Init Game Loop
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
			//std::cout << "found QUIT message, Ending program" << std::endl;
			isDone = true;
			DestroyWindow(handleWindow);
		}							  
		Update();
		DrawScene();
		Sleep(0);
    }

	//release alles
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
void ThempX::Update()
{
	oldPos = camera.position;
	newPos = camera.position;
	oldTicks = newTicks;
	newTicks = GetTickCount();
	DWORD tempDeltaTime = newTicks - oldTicks;
	if(tempDeltaTime < 100)
	{
		deltaTime = tempDeltaTime*0.001f;
		DoInput();
		bool didCollide = false;
		if(!didCollide)
		{
			camera.position = newPos;
		}
		DoCameraStuff();
		for(unsigned int i =0 ;i<spriteObjs.size(); i++)
		{
			spriteObjs.at(i)->Animate(deltaTime);
		}
	}
	
}
D3DXVECTOR3 ThempX::AddVector3(D3DXVECTOR3* a, D3DXVECTOR3* b)
{
	return D3DXVECTOR3(a->x+b->x,a->y+b->y,a->z+b->z);
}
D3DXVECTOR3 ThempX::SubstractVector3(D3DXVECTOR3* a, D3DXVECTOR3* b)
{
	return D3DXVECTOR3(a->x-b->x,a->y-b->y,a->z-b->z);
}
bool ThempX::CheckCamBoxCollision(D3DXVECTOR3 pos, Object3D* obj)
{
/*	D3DXVECTOR3 innerPoint = AddVector3(&obj->position,&obj->innerBox); //- values
	D3DXVECTOR3 outerPoint = AddVector3(&obj->position,&obj->outerBox); //+ values

	if(pos.x > innerPoint.x && pos.y > innerPoint.y && pos.z > innerPoint.z && pos.x < outerPoint.x && pos.y < outerPoint.y && pos.z < outerPoint.z)
	{
		return true;
	}  */
	return false;
}

void LogVector(D3DXVECTOR3 a)
{
	//std::cout << a.x << "<- X  " << a.y << "<- Y  " << a.z << "<- Z  " << endl;
}
void ThempX::LeftMouseClick()
{
	
	D3DXVECTOR3 outCamPos;
	D3DXMATRIX inverseWorld;
	D3DXVECTOR3 lookDirInverse;
	const D3DXMATRIX* temp;

	for(unsigned int i = 0; i < modelObjs.size();i++)
	{
		if(Vector3Distance(&modelObjs.at(i)->position,&camera.position) < 100)
		{
			float distance = Vector3Distance(&modelObjs.at(i)->position,&camera.position);
			BOOL hit = FALSE;
			FLOAT distToHit = 100;
			temp = &modelObjs.at(i)->worldMatrix;
			D3DXMatrixInverse(&inverseWorld,0,temp);
			D3DXVec3TransformCoord(&outCamPos,&camera.position,temp);
			D3DXVec3TransformNormal(&lookDirInverse,&camera.lookDir,temp);

			D3DXIntersect(modelObjs.at(i)->model.mesh,&outCamPos,&lookDirInverse,&hit,NULL,NULL,NULL,&distToHit,NULL,NULL);  //dit werkt niet want hij verwijst naar de ingelade model via resourceManager, Wat ik moet doen is opnieuw inladen elke keer, niet verzinnen om hier om heen te komen, teveel werk voor te weinig winst atm, je zal hoe dan ook aparte meshes in je ram krijgen en de models lezen duurt niet lang
		
			if(hit == TRUE)
			{
				cout << "hit target: "<<modelObjs.at(i)->objName << "  at distance: " << distToHit << endl;
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
}
void ThempX::DoInput()
{
	if(inputHandler->MouseButtonDown(0))
	{
		if(mouseLeftJustDown == false)
		{
			//LeftMouseClick();
			mouseLeftJustDown = true;
		}
	}
	else
	{
		mouseLeftJustDown = false;
	}
	if(inputHandler->MouseButtonDown(1))
	{
		//std::cout<< camera.position.x << "<-X "  <<camera.position.y  << "<-Y " <<camera.position.z  <<"<-Z " <<std::endl;
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
		newPos.y += deltaTime*speed;
	}
	if(KeyPressed(DIK_E))
	{
		newPos.y -= deltaTime*speed;
	}
	if(KeyPressed(DIK_W))
	{
		newPos.x += camera.lookDir.x *deltaTime*speed;
		newPos.z += camera.lookDir.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_S))
	{
		newPos.x -= camera.lookDir.x *deltaTime*speed;
		newPos.z -= camera.lookDir.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_D))
	{
		D3DXVECTOR3 temp = ReturnDirection(angleX-90,angleY);
		newPos.x += temp.x *deltaTime*speed;
		newPos.z += temp.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_A))
	{
		D3DXVECTOR3 temp = ReturnDirection(angleX+90,angleY);
		newPos.x += temp.x *deltaTime*speed;
		newPos.z += temp.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_I))
	{
		spriteObjs.at(1)->position.x += speed*deltaTime;
	}
	if(KeyPressed(DIK_K))
	{
		spriteObjs.at(1)->position.x -= speed*deltaTime;
	}
	if(KeyPressed(DIK_J))
	{
		spriteObjs.at(1)->position.z += speed*deltaTime;
	}
	if(KeyPressed(DIK_L))
	{
		spriteObjs.at(1)->position.z -= speed*deltaTime;
	}
	if(KeyPressed(DIK_U))
	{
		spriteObjs.at(1)->position.y += speed*deltaTime;
	}
	if(KeyPressed(DIK_O))
	{
		spriteObjs.at(1)->position.y -= speed*deltaTime;
	}

	if(KeyPressed(DIK_1))
	{
		for(unsigned int i = 0; i < spriteObjs.size();i++)
		{
			if(spriteObjs.at(i)->hasAnimation)
			{
				spriteObjs.at(i)->PlayAnimation("Idle");
			}
		}
	}
	if(KeyPressed(DIK_2))
	{
		for(unsigned int i = 0; i < spriteObjs.size();i++)
		{
			if(spriteObjs.at(i)->hasAnimation)
			{
				spriteObjs.at(i)->PlayAnimation("Walk");
			}
		}
	}

	if(KeyPressed(DIK_SPACE))
	{
		//cout << "Reloaded level" << endl;
		DestroyLevel();
		// dat is het rare, pas geleden was het 7 MB
		//also de muis locked in center bij start , rechtsklik om de lock eraf te halen
		LoadLevel();
	}
	
	if(inputHandler->KeyPressed(DIK_ESCAPE))
	{
		PostQuitMessage(0);
		isDone = true;
		
	}
}

void ThempX::DoCameraStuff()
{
	angleX-=inputHandler->GetMousePosX()*deltaTime*sensitivity;
	(angleY < -50 ? angleY = -50 : (angleY> 50 ? angleY = 50 : 0));
	angleY-=inputHandler->GetMousePosY()*deltaTime*sensitivity;

	SetCameraLookX(angleX,angleY);
}
D3DXVECTOR3 ThempX::ReturnDirection(float anglesX,float anglesY)
{
	D3DXVECTOR3 final;
	D3DXVECTOR3 camPos = oldPos;
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

	//std::cout << lookDir.x << " <- X  " << lookDir.y << "<- normY  " << lookDir.z << "<- normZ" << std::endl;
	return final;
}
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
	//float normX = camera.position.x-camera.lookAt.x;
	//float normY = camera.position.y-camera.lookAt.y;
	//float normZ = camera.position.z-camera.lookAt.z;
	//std::cout << lookDir.x << " <- X  " << lookDir.y << "<- normY  " << lookDir.z << "<- normZ" << std::endl;
}
void ThempX::Initialize()
{
	camera.lookDir.x = 0;
	camera.lookDir.y = 1;
	camera.lookDir.z = 0;
	sensitivity = 5;
	soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	angleX = 0;
	angleY = 0;
	oldTicks = 0;
	newTicks = 0;
	camera.position.x = 10;
	camera.position.y = 10;
	camera.position.z = 0;
	snapCursor = false;
	camera.lookAt.x = 0;
	camera.lookAt.y = 0;
	camera.lookAt.z = 0;
	keys.resize(256,0);
}
void ThempX::DrawScene()
{
	////////////////////////
	// Clear de scene en begin met het drawen
    p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(58,155,255), 1.0f, 0);
	p_Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    p_Device->BeginScene();

	SetUpCamera();

    ////////////////////////

	////////////////////////
	//Draw mijn objecten
	
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

	
	////////////////////////

	////////////////////////
	//eindig het drawen en laat het zien op het scherm (switch backbuffer met frontbuffer)
    p_Device->EndScene();
    p_Device->Present(NULL, NULL, NULL, NULL);
	////////////////////////
}
void ThempX::SetUpCamera()
{
	D3DXVECTOR3 m_UpVector(0, 1, 0);
	D3DXMatrixLookAtLH(&camera.m_View, &camera.position, &camera.lookAt, &m_UpVector);
	p_Device->SetTransform(D3DTS_VIEW, &camera.m_View);
	D3DXMatrixPerspectiveFovLH(&camera.m_Projection, D3DX_PI/4, 500/500, 1, 2000);
	p_Device->SetTransform(D3DTS_PROJECTION, &camera.m_Projection);
}

LPDIRECT3DDEVICE9 ThempX::InitializeDevice(HWND han_WindowToBindTo) 
{
	//////////////////////////////////////////
	//Check of de PC DirectX heeft, anders laat een error zien;
	LPDIRECT3D9 p_dx_Object = NULL;
	p_dx_Object = Direct3DCreate9(D3D_SDK_VERSION);
	if (p_dx_Object == NULL)
	{
		MessageBox(han_WindowToBindTo,"DirectX Runtime library not installed!","InitializeDevice()",MB_OK);
		isDone = true; // laat de applicatie zich afsluiten
		return 0;
	}
	//////////////////////////////////////////

	//////////////////////////////////////////
	//initialiseer parameters voor DX9
	D3DPRESENT_PARAMETERS dx_PresParams;
 
	ZeroMemory( &dx_PresParams, sizeof(dx_PresParams)); //zet alles op NULL in de parameter class zodat alles ge-init is (NULL = 0x000000, het is dus een waarde (pointer) en niet null als in niks)
	dx_PresParams.Windowed = TRUE;
	dx_PresParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dx_PresParams.BackBufferFormat = D3DFMT_UNKNOWN;
	dx_PresParams.BackBufferCount = 1;
	dx_PresParams.EnableAutoDepthStencil = TRUE;
	dx_PresParams.AutoDepthStencilFormat = D3DFMT_D16;

	//////////////////////////////////////////

	//////////////////////////////////////////
	// Claim de device in Hardware mode als dat lukt
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
	if(p_dx_Device == NULL) //device claimen is dus niet gelukt, laten we proberen of we het in software-mode kunnen krijgen
	{
		result = p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, han_WindowToBindTo, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device);
		switch(result)
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
	//alles is goed gegaan, we returnen nu onze geclaimde device
	return p_dx_Device;
	//////////////////////////////////////////
}

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
void ThempX::LoadLevel()
{
	ifstream fin("level.txt");
	if (!fin.good())
	{
		MessageBox(handleWindow,"Could Not Find Level.txt, scene will be empty", "ThempX()",MB_OK);
		cout << "Cannot find level.txt" << endl;
	}
	else
	{
		string str;	   //Zijn strings niet like.. long pointers? ehm volgensmij een soort wrapper om Char* heen moet je hem dan niet deleten..? Waar ben je klaar met al die strings?
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
				
				//hier voor models
			}
			else
			{
				if(XAnimRows == 0 && YAnimRows == 0 || XAnimRows == 1 && YAnimRows == 1)
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
					//hier voor sprites
				}
			}
		}
		//einde functie
		fin.close();
		
	}
}
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