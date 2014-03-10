#include "../Headers/Game.h"

Game::Game(Game::DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev)
{
	data = b;
	handleWindow = windowHandle;
	gui = new GUI(d3dDev,resMan);
	resources = resMan;
	inputHandler = inputHand;
	soundHandler = soundHand;
	p_Device = d3dDev;
	physics = new PhysXEngine(resources);
	cam = new Camera(resMan,THEMPX_CAM_PERSPECTIVE);
	cam->SetPosition(0,0,0);
	LoadLevel();
	Initialize();

	//particles.push_back(new Particle(resources,p_Device,"Resources/Particles/Lightning.png",&camera.m_View,D3DXVECTOR3(0,10,10),200,500,1,3));
	//particles.at(0)->SetMovement(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,3,0));
	//particles.at(0)->Release();
}
Game::Object2DData Game::CreateObject2DData(char* filePath,bool hasAnim, D3DXVECTOR3 pos,D3DXVECTOR3 scale,D3DXVECTOR2 tSize, D3DXVECTOR2 rows)
{
	Game::Object2DData d;
	d.Nullify();
	d.filePath = filePath;
	d.hasAnimation = hasAnim;
	d.position = pos;
	d.scale = scale;
	d.textureSizeX = tSize.x;
	d.textureSizeY = tSize.y;
	d.xRowsAnim = rows.x;
	d.yRowsAnim = rows.y;
	return d;
}
Game::Object3DData Game::CreateObject3DData(char* filePath,D3DXVECTOR3 pos,D3DXVECTOR3 scale,D3DXVECTOR3 rot)
{
	Game::Object3DData d;
	d.Nullify();
	d.filePath = filePath;
	d.position = pos;
	d.scale = scale;
	d.rotation = rot;
	return d;
}
void Game::Initialize()
{
	currentEditorObjIndex = 0;
	qualityLevel = 0;
	
	wireframe = false;
	scaleMultiplier = 3;
	EditorMode = false;
	soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	
	keys.resize(256,0);
	resources->CreateTextObject("Arial","\n    THIS IS TESTING TEXT",12, 0, 0, 30, 20, 0xFFFF0000);
}
void Game::Update(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
	for(unsigned int i =0 ;i<spriteObjs.size(); i++)
	{
		spriteObjs.at(i)->Animate(deltaTimeF);
	}
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		particles.at(i)->Update(deltaTimeF);
	}
	if(EditorMode)
	{
		IFOBJ2D
		{
			currentEditorObj->obj2D->position = AddVector3(&cam->GetLookAt(),&MultiplyVector3(&cam->GetLookDir(),15));
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->position = AddVector3(&cam->GetLookAt(),&MultiplyVector3(&cam->GetLookDir(),15));
		}
		else IFCOL
		{
			currentEditorObj->col->position = AddVector3(&cam->GetLookAt(),&MultiplyVector3(&cam->GetLookDir(),15));
		}
	}
	inputHandler->Update();
	if(data->changeDisplay)
	{
		return;
	}
	DoInput(deltaTimeF);
	cam->Update(deltaTimeF,inputHandler->GetMousePosX(),inputHandler->GetMousePosY());
	physx::PxReal elapsed = 1.0f/60.0f;
	physics->gScene->simulate(elapsed);
	while(!physics->gScene->fetchResults())
	{
		
	}
	gui->Update(deltaTimeF);
}
void Game::FixedUpdate(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
}
void Game::Render()
{
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
		spriteObjs.at(i)->cameraView = &cam->GetView();
		spriteObjs.at(i)->Draw();
	}
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		particles.at(i)->Draw();
	}
	if(EditorMode)
	{
		IFOBJ2D
		{
			p_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			p_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			currentEditorObj->obj2D->Draw();
		}
		else IFOBJ3D
		{
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
			currentEditorObj->obj3D->DrawModel();
		}
		else IFCOL
		{
			currentEditorObj->col->Draw();
		}
	}
	physics->DrawBoxes();
	gui->Render();
	cam->RenderCamera(THEMPX_CAM_PERSPECTIVE);
	resources->DrawAllText();
}
void Game::ReloadGUI()
{
	gui->ReloadGUI();
}
//Input Handling (for testing, this needs to be in game.cpp when a game is created)
void Game::DoInput(float dT)
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
			data->lockCursor = !data->lockCursor;
			mouseRightJustDown = true;
		}
	}
	else
	{
		mouseRightJustDown = false;
	}
	if(KeyPressed(DIK_COMMA) == 2)
	{
		data->changeDisplay = true;
		data->windowed = !data->windowed;
		if(data->windowed)
		{
			data->windowSizeX = 800;
			data->windowSizeY = 600;
			data->renderSizeX = 800;
			data->renderSizeY = 600;
		}
		else
		{
			data->devmodeIndex = resources->GetDevModeWithHeight(768);
		}
		//p_Device->EvictManagedResources();
		//resources->LostDeviceAllText();
		return;
	}
	if(KeyPressed(DIK_M) == 2)
	{
		data->changeDisplay = true;
		data->windowed = true;
		if(data->windowed)
		{
			qualityLevel++;
			if(qualityLevel == 1)
			{
				data->renderSizeX = 240;
				data->renderSizeY = 180;
			}
			else if(qualityLevel == 2)
			{
				data->renderSizeX = 320;
				data->renderSizeY = 240;
			}
			else if(qualityLevel == 3)
			{
				data->renderSizeX = 480;
				data->renderSizeY = 360;
			}
			else if(qualityLevel == 4)
			{
				data->renderSizeX = 640;
				data->renderSizeY = 480;
			}
			else if(qualityLevel == 5)
			{
				data->renderSizeX = 800;
				data->renderSizeY = 600;
			}
			else if(qualityLevel == 6)
			{
				data->renderSizeX = 1024;
				data->renderSizeY = 800;
			}
			else if(qualityLevel == 7)
			{
				data->renderSizeX = 4096;
				data->renderSizeY = 3072;
			}
			else if(qualityLevel == 8)
			{
				qualityLevel = 1;
				data->renderSizeX = 240;
				data->renderSizeY = 180;
			}

		}
		//p_Device->EvictManagedResources();
		//resources->LostDeviceAllText();
		return;
	}
	if(KeyPressed(DIK_N) == 2)
	{
		data->changeDisplay = true;
		data->windowed = false;
		data->renderSizeX = 1366;
		data->renderSizeY = 768;
		resources->GetDesktopResolution(data->windowSizeX,data->windowSizeY);
		if(!data->windowed)
		{
			qualityLevel++;
			if(qualityLevel == 1)
			{
				data->renderSizeX = 240;
				data->renderSizeY = 180;
			}
			else if(qualityLevel == 2)
			{
				data->renderSizeX = 320;
				data->renderSizeY = 240;
			}
			else if(qualityLevel == 3)
			{
				data->renderSizeX = 480;
				data->renderSizeY = 360;
			}
			else if(qualityLevel == 4)
			{
				data->renderSizeX = 640;
				data->renderSizeY = 480;
			}
			else if(qualityLevel == 5)
			{
				data->renderSizeX = 800;
				data->renderSizeY = 600;
			}
			else if(qualityLevel == 6)
			{
				data->renderSizeX = 1024;
				data->renderSizeY = 800;
			}
			else if(qualityLevel == 7)
			{
				data->renderSizeX = 1280;
				data->renderSizeY = 1024;
			}
			else if(qualityLevel == 8)
			{
				qualityLevel = 1;
				data->renderSizeX = 240;
				data->renderSizeY = 180;
			}

		}
		//p_Device->EvictManagedResources();
		//resources->LostDeviceAllText();
		return;
	}
	int speed = 60;
	if(KeyPressed(DIK_LSHIFT))
	{
		speed = 90;
	}
	if(KeyPressed(DIK_Q))
	{
		cam->AddPosition(0,dT*speed,0);
	}
	if(KeyPressed(DIK_E))
	{
		cam->AddPosition(0,-dT*speed,0);
	}
	if(KeyPressed(DIK_W))
	{
		cam->AddPosition(cam->GetLookDir().x *dT*speed,0,cam->GetLookDir().z *dT*speed);
	}
	if(KeyPressed(DIK_S))
	{
		cam->AddPosition(-cam->GetLookDir().x *dT*speed,0,-cam->GetLookDir().z *dT*speed);
	}
	if(KeyPressed(DIK_D))
	{
		D3DXVECTOR3 temp = cam->ReturnDirection(cam->angleX-90,cam->angleY);
		cam->AddPosition(temp.x *dT*speed,0,temp.z *dT*speed);
	}
	if(KeyPressed(DIK_A))
	{
		D3DXVECTOR3 temp = cam->ReturnDirection(cam->angleX+90,cam->angleY);
		cam->AddPosition(temp.x *dT*speed,0,temp.z *dT*speed);
	}
	if(KeyPressed(DIK_NUMPADPLUS) == 2)
	{
		cam->sensitivity += 30;
		std::cout << "Mouse Sensitivity set to: " <<cam->sensitivity << std::endl;
	}
	if(KeyPressed(DIK_NUMPADMINUS) == 2)
	{
		cam->sensitivity -= 30;
		std::cout << "Mouse Sensitivity set to: " <<cam->sensitivity << std::endl;
	}


	if(KeyPressed(DIK_0) == 2)
	{
		EditorMode = false;
		if(EditorMode == true)
		{
			currentEditorObjIndex = 0;
			SetUpEditorMode();
		}
		std::cout <<"Editor Mode has been set to: "<< EditorMode << std::endl;
	}
	
	if(KeyPressed(DIK_PERIOD) == 2)
	{
		wireframe = !wireframe;
		if(wireframe)
		{
			p_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		}
		else
		{
			p_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
		}
	}
	if(KeyPressed(DIK_NUMPADMINUS) == 2)
	{
		scaleMultiplier = -3;
		cout << "Scale Adding has been set to false" << endl;
	}
	if(KeyPressed(DIK_NUMPAD4) || KeyPressed(DIK_NUMPAD6))
	{
		IFCOL
		{
			currentEditorObj->col->scaling.x += scaleMultiplier*dT;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->scaling.x += scaleMultiplier*dT;
		}
		else IFOBJ2D
		{
			currentEditorObj->obj2D->scaling.x += scaleMultiplier*dT;
		}
	}
	if(KeyPressed(DIK_NUMPAD2) || KeyPressed(DIK_NUMPAD8))
	{
		IFCOL
		{
			currentEditorObj->col->scaling.y += scaleMultiplier*dT;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->scaling.y += scaleMultiplier*dT;
		}
		else IFOBJ2D
		{
			currentEditorObj->obj2D->scaling.y += scaleMultiplier*dT;
		}
	}
	if(KeyPressed(DIK_NUMPAD5) || KeyPressed(DIK_NUMPAD0))
	{
		IFCOL
		{
			currentEditorObj->col->scaling.z += scaleMultiplier*dT;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->scaling.z += scaleMultiplier*dT;
		}
	}
	if(KeyPressed(DIK_I))
	{
		IFCOL
		{
			currentEditorObj->col->rotation.x += 5*dT*5;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->rotation.x += dT*5;
		}
	}
	if(KeyPressed(DIK_K))
	{
		IFCOL
		{
			currentEditorObj->col->rotation.x -= 5*dT*5;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->rotation.x -=dT*5;
		}
	}
	if(KeyPressed(DIK_J))
	{
		IFCOL
		{
			currentEditorObj->col->rotation.y += 5*dT*5;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->rotation.y += dT*5;
		}
	}
	if(KeyPressed(DIK_L))
	{
		IFCOL
		{
			currentEditorObj->col->rotation.y -= 5*dT*5;
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->rotation.y -= dT*5;
		}
	}
	if(KeyPressed(DIK_8) == 2)
	{
		currentEditorObjIndex--;
		cout << currentEditorObjIndex << endl;
		if(currentEditorObjIndex < 0)
		{
			currentEditorObjIndex = editorObjs.size()-1;
		}
		currentEditorObj = &editorObjs.at(currentEditorObjIndex);
	}
	if(KeyPressed(DIK_9) == 2)
	{
		currentEditorObjIndex++;
		cout << currentEditorObjIndex << endl;
		cout << "pressed 9 " << endl;
		if(currentEditorObjIndex == editorObjs.size())
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
		data->loop = false;
	}
}
bool Game::Create3DObject(bool hasPhysics, Object3DData* data)
{
	if(data != NULL)
	{
		if(hasPhysics)
		{
			//pData->scaleModel = SPEVector(data->scale.x,data->scale.y,data->scale.z);
			std::cout << hasPhysics << "<- has PhysX" << endl;
			//physics->Create3DPhysicsObject(data->filePath,pData);
		}
		else
		{
			Object3D* obj = new Object3D(resources,data->filePath);
			obj->position = data->position;
			obj->scaling = data->scale;
			obj->rotation = data->rotation;
			modelObjs.push_back(obj);
		}
		return true;
	}
	return false;
}
bool Game::CreateAnimated2DObject(bool hasPhysics, Object2DData* data)
{
	Object2D* obj;
	if(hasPhysics)
	{
		if(data->hasAnimation)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->textureSizeX,data->textureSizeY,data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			obj->PlayAnimation("Test");
			//physics->Create2DPhysicsObject(pData);
			//if(pData->isStatic != false)
			//{
			//	obj->linkedPhysicsObj = physics->GetLastStaticBody();
			//}
			//else
			//{
			//	obj->linkedPhysicsObj = physics->GetLastRigidBody();
			//}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(data != NULL)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->textureSizeX,data->textureSizeY,data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			obj->PlayAnimation("Test");
			//if(pData->isStatic != false)
			//{
			//	obj->linkedPhysicsObj = physics->GetLastStaticBody();
			//}
			//else
			//{
			//	obj->linkedPhysicsObj = physics->GetLastRigidBody();
			//}
		}
		else
		{
			return false;
		}
	}
	spriteObjs.push_back(obj);
	return true;
}
bool Game::CreateStatic2DObject(bool hasPhysics, Object2DData* data)
{
	if(data != NULL)
	{
		Object2D* obj;
		if(hasPhysics)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			//physics->Create2DPhysicsObject(pData);
			//if(pData->isStatic != false)
			//{
			//	obj->linkedPhysicsObj = physics->GetLastStaticBody();
			//}
			//else
			//{
			//	obj->linkedPhysicsObj = physics->GetLastRigidBody();
			//}
		}
		else
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			//if(pData->isStatic != false)
			//{
			//	obj->linkedPhysicsObj = physics->GetLastStaticBody();
			//}
			//else
			//{
			//	obj->linkedPhysicsObj = physics->GetLastRigidBody();
			//}
		}
		spriteObjs.push_back(obj);
		return true;
	}
	return false;
}
//Left Mouse click function (to test things with), will be removed for engine use, as this is a gameplay feature
void Game::LeftMouseClick()
{
	if(gui->guiObjs.size() > 0)
	{
		gui->PlayAnimation(&gui->guiObjs.at(1),"Attack");
	}
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			if(!currentEditorObj->obj2D->hasAnimation)
			{
				Object2D* obj = new Object2D(resources,currentEditorObj->obj2D->quad.textureName,GetCameraView());
				obj->SetPosition(currentEditorObj->obj2D->position.x,currentEditorObj->obj2D->position.y,currentEditorObj->obj2D->position.z);
				obj->objName = currentEditorObj->obj2D->objName;
				spriteObjs.push_back(obj);
			}
			else
			{
				Object2D* obj = new Object2D(resources,currentEditorObj->obj2D->quad.textureName,GetCameraView(),currentEditorObj->obj2D->GetXSize(),currentEditorObj->obj2D->GetYSize(),currentEditorObj->obj2D->GetXRows(),currentEditorObj->obj2D->GetYRows());
				obj->SetPosition(currentEditorObj->obj2D->position.x,currentEditorObj->obj2D->position.y,currentEditorObj->obj2D->position.z);
				obj->objName = currentEditorObj->obj2D->objName;
				spriteObjs.push_back(obj);
			}
		}
		else if(currentEditorObj->obj3D != NULL)
		{
			Object3D* obj = new Object3D(resources,currentEditorObj->obj3D->model.meshPath);
			obj->objName = currentEditorObj->obj3D->objName;
			obj->SetPosition(currentEditorObj->obj3D->position.x,currentEditorObj->obj3D->position.y,currentEditorObj->obj3D->position.z);
			modelObjs.push_back(obj);
		}
		else
		{
			DebugCube* src = currentEditorObj->col;
			DebugCube* obj = new DebugCube(src->position,src->rotation,src->LLFPos,src->URBPos,resources);
			obj->SetPosRotScale(src->position,src->rotation,src->scaling);
			debugCubes.push_back(obj);
		}
		CreateLevelFile();
	}
}
void Game::ReleaseAll()
{
	DestroyLevel();
	for(unsigned int i=0;i<modelObjs.size();i++)
	{
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
	physics->ReleaseAll();
	gui->Release();
	delete gui;
	delete cam;
}
//easy-for-use function for testing keys to be pressed
int Game::KeyPressed(int key)
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
}
//speaks for itself
void Game::DestroyLevel()
{
	for(unsigned int i=0;i<modelObjs.size();i++)
	{
		delete modelObjs.at(i);
	}
	modelObjs.clear(); 
	for(unsigned int i=0;i<spriteObjs.size();i++)
	{
		spriteObjs.at(i)->ReleaseResources();
		delete spriteObjs.at(i);
	}
	spriteObjs.clear();
	for(unsigned int i=0;i<debugCubes.size();i++)
	{
		debugCubes.at(i)->Release();
		delete debugCubes.at(i);
	}
	debugCubes.clear();
	for(unsigned int i=0;i<lights.size();i++)
	{
		free(lights.at(i));
		delete lights.at(i);
	}
	lights.clear();
}
void Game::SetUpEditorMode()
{
	for(unsigned int i = 0; i < editorObjs.size(); i++)
	{
		if(editorObjs.at(i).obj2D != NULL)
		{
			editorObjs.at(i).obj2D->ReleaseResources();
			delete editorObjs.at(i).obj2D;
		}
		else if(editorObjs.at(i).obj3D != NULL)
		{
			delete editorObjs.at(i).obj3D;
		}
		else
		{
			editorObjs.at(i).col->Release();
			delete editorObjs.at(i).col;
		}
	}
	editorObjs.clear();

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
		EditorObj obj;
		obj.col = new DebugCube(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(-1,-1,-1),D3DXVECTOR3(1,1,1),resources);
		obj.col->scaling = D3DXVECTOR3(1,1,1);
		obj.obj2D = NULL;
		obj.obj3D = NULL;
		editorObjs.push_back(obj);
		int animatedSpriteNr = 0;
		int staticSpriteNr = 0;
		int modelNr = 0;
		while ( getline (myfile,line) )
		{
			obj.col = NULL;
			obj.obj2D = NULL;
			obj.obj3D = NULL;
			string name, tag;
			float sizeX,sizeY,xRows,yRows;
			bool hasAnim = false;
			string check = "x";
			myfile >> name >> hasAnim >> sizeX >> sizeY >> xRows >> yRows >> tag;
			if(name[name.size()-1] == check[0])
			{
				modelNr++;
				Object3D* modelObj = new Object3D(resources,_strdup(name.c_str()));
				std::ostringstream oss;
				oss<<"Model"<<modelNr;
				modelObj->objName = oss.str();
				obj.obj3D = modelObj;
				modelObj->tag = tag;
				editorObjs.push_back(obj);
			}
			else
			{
				if(hasAnim == false)
				{
					staticSpriteNr++;
					Object2D* spriteObj = new Object2D(resources,_strdup(name.c_str()),GetCameraView());
					std::ostringstream oss;
					oss<<"StaticSprite"<<staticSpriteNr;
					spriteObj->objName = oss.str();
					spriteObj->handleWindow = handleWindow;
					obj.obj2D = spriteObj;
					spriteObj->tag = tag;
					editorObjs.push_back(obj);

				}
				else
				{
					animatedSpriteNr++;
					Object2D* spriteObj = new Object2D(resources,_strdup(name.c_str()),GetCameraView(),sizeX,sizeY,xRows,yRows);
					spriteObj->handleWindow = handleWindow;
					std::ostringstream oss;
					oss << "AnimatedSprite" << animatedSpriteNr;
					spriteObj->objName = oss.str();
					obj.obj2D = spriteObj;
					spriteObj->tag = tag;
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
	currentEditorObjIndex = 0;
	cout <<"array size is: "<< editorObjs.size() << endl;
}
void Game::CreateLevelFile()
{
	ofstream level("level.txt");
	if (level.is_open())
	{
		level << "name" << "\t"<< "objName" << "\t"<< "posx" << "\t"<< "posy" << "\t"<< "posz" << "\t"<< "scalex" << "\t"<< "scaley" << "\t"<< "scalez" << "\t"<< "rotx" << "\t"<< "roty" << "\t"<< "rotz" << "\t"<< "sizeX" << "\t"<< "sizeY" << "\t"<< "XAnimRows" << "\t"<< "YAnimRows" << "Tag" << "\n";
		for(unsigned int i = 0; i < modelObjs.size();i++)
		{
			Object3D* obj = modelObjs.at(i);
			level << obj->model.meshPath << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" <<0 <<obj->tag << "\n";
		}
		for(unsigned int i = 0; i < spriteObjs.size();i++)
		{
			Object2D* obj = spriteObjs.at(i);
			level << obj->quad.textureName << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << obj->GetXSize() << "\t" << obj->GetYSize() << "\t" << obj->GetXRows() << "\t" << obj->GetYRows() <<obj->tag << "\n";
		}
		for(unsigned int i = 0; i < debugCubes.size();i++)
		{
			DebugCube* obj = debugCubes.at(i);
			level << "Collision"  << "\t" << "Collision"<< "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" << 0 << obj->tag <<"\n";
		}

		level.close();
	}
	else
	{
		std::cout << "Unable to create/open file" << std::endl;
	}
}
//speaks for itself
void Game::LoadLevel()
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
			string name,tag;
			string check = "x";
			string objName;
			float XAnimRows;
			float YAnimRows;
			float sizeX;
			float sizeY;
			float posx,posy,posz,scalex,scaley,scalez,rotx,roty,rotz,hitBoxX,hitBoxY,hitBoxZ;
			bool hasPhysics,isStatic,doDraw;
			fin >> name >> objName >>posx >> posy >> posz >> scalex >> scaley >> scalez >> rotx >> roty >> rotz >> sizeX >> sizeY >> XAnimRows >> YAnimRows >> tag >> hasPhysics >> isStatic >> doDraw >>hitBoxX >> hitBoxY >> hitBoxZ;

			if(name.at(name.size()-1) == check[0])
			{
				char* mName = _strdup(name.c_str());
				Object3DData data = CreateObject3DData(mName,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,scalez),D3DXVECTOR3(rotx,roty,rotz));
				//SPEEngine::RigidData pData = CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(scalex,scaley,scalez),SPEVector(0,0,0),SPEVector(0,0,0));
				Create3DObject(hasPhysics,&data);
				if(!hasPhysics)
				{
					Object3D* obj = modelObjs.at(modelObjs.size()-1);
					obj->objName = objName;
					obj->tag = tag;
				}
			}
			else
			{
				Object2D* obj;
				if(XAnimRows == 0 && YAnimRows == 0)
				{
					CreateStatic2DObject(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),false,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2(0,0),D3DXVECTOR2(0,0)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
					obj = spriteObjs.at(spriteObjs.size()-1);
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->tag = tag;
				}
				else
				{
					std::cout << "Animation PNG added" << std::endl;
					CreateAnimated2DObject(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),true,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2(sizeX,sizeY),D3DXVECTOR2(XAnimRows,YAnimRows)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
					obj = spriteObjs.at(spriteObjs.size()-1);
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->tag = tag;	
				}
			}
		}
		fin.close();
		CreateLevelFile();
	}
}
//unused light function
D3DLIGHT9* Game::CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff)
{
	D3DLIGHT9* light = new D3DLIGHT9();
	light->Direction = direction;
	light->Diffuse = lightColor;
	light->Position = position;
	light->Range = range;
	light->Falloff = falloff;
	light->Type = lightType;
	lights.push_back(light);
	return light;
}