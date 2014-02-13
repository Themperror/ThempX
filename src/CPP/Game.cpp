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
	LoadLevel();
	Initialize();
	physics = new SPEEngine(resources);

	SPEEngine::RigidData physicsData2;
	physicsData2.Nullify();
	physicsData2.doRender = false;
	physicsData2.isStatic = true;
	physicsData2.scaleModel = SPEVector(2,2,2);
	physicsData2.position = SPEVector(0,-10,0);
	physics->Create3DPhysicsObject("resources/models/level1.x",&physicsData2);

	for(unsigned int i = 0; i < 100; i++)
	{
		SPEEngine::RigidData physicsData;
		physicsData.Nullify();
		physicsData.doRender = true;
		physicsData.density = 1;
		physicsData.mass = 10;
		physicsData.isStatic = false;
		physicsData.scaleModel = SPEVector(1,1,1);
		physicsData.position = SPEVector(0,i*1.1f,3*sin(i*0.5f));
		physics->Create3DPhysicsObject("resources/models/cube.x",&physicsData);
	}

	player = new FirstPersonPlayer(D3DXVECTOR3(0,0,0),VECTOR3ZERO,resources,inputHandler,&keys);
	//cout << debugCubes.at(0)->collision->GetPosition().x <<"  " <<debugCubes.at(0)->collision->GetPosition().y << "  " <<debugCubes.at(0)->collision->GetPosition().z << endl;
	//particles.push_back(new Particle(resources,p_Device,"Resources/Particles/Lightning.png",&camera.m_View,D3DXVECTOR3(0,10,10),200,500,1,3));
	//particles.at(0)->SetMovement(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,3,0));
	//particles.at(0)->Release();
}
void Game::Initialize()
{
	currentEditorObjIndex = 0;
	
	scaleMultiplier = 3;
	EditorMode = false;
	soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	
	keys.resize(256,0);
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
			currentEditorObj->obj2D->position = AddVector3(&player->GetLookAt(),&MultiplyVector3(&player->GetLookDir(),15));
		}
		else IFOBJ3D
		{
			currentEditorObj->obj3D->position = AddVector3(&player->GetLookAt(),&MultiplyVector3(&player->GetLookDir(),15));
		}
		else IFCOL
		{
			currentEditorObj->col->position = AddVector3(&player->GetLookAt(),&MultiplyVector3(&player->GetLookDir(),15));
			//currentEditorObj->col->HardUpdateCollisionGeo(&collisionLock);
		}
	}
	player->Update(deltaTimeF);
	gui->Update(deltaTimeF);
}
void Game::FixedUpdate(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
	physics->OnFrameMove(deltaTimeF);
	inputHandler->Update();
	DoInput(deltaTimeF);

	player->FixedUpdate(deltaTimeF);
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
	physics->OnFrameRender(resources->GetDevice());

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
	gui->Render();
	//Make sure this is last, as setting this before any renders/changes it will make the scene behind 1 frame;
	player->Render();
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

	if(KeyPressed(DIK_0) == 2)
	{
		EditorMode = !EditorMode;
		if(EditorMode == true)
		{
			currentEditorObjIndex = 0;
			SetUpEditorMode();
		}
		std::cout <<"Editor Mode has been set to: "<< EditorMode << std::endl;
	}
	if(KeyPressed(DIK_NUMPADPLUS) == 2)
	{
		scaleMultiplier = 3;
		cout << "Scale Adding has been set to true" << endl;
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
void Game::Create3DObject(bool hasPhysics, Object3DData* data, SPEEngine::RigidData* pData)
{
	if(hasPhysics)
	{

	}
	else
	{
		Object3D* obj = new Object3D(resources,data->filePath);
		obj->position = data->position;
		obj->scaling = data->scale;
		obj->rotation = data->rotation;
		modelObjs.push_back(obj);
	}
}
void Game::CreateAnimated2DObject(bool hasPhysics, Object2DData* data, SPEEngine::RigidData* pData)
{
	if(hasPhysics)
	{

	}
	else
	{
		Object2D* obj = new Object2D(resources,data->filePath,player->GetCameraView(),data->textureSizeX,data->textureSizeY,data->xRowsAnim,data->yRowsAnim);
		obj->position = data->position;
		obj->scaling = data->scale;
		obj->rotation = data->rotation;
		spriteObjs.push_back(obj);
	}
}
void Game::CreateStatic2DObject(bool hasPhysics, Object2DData* data, SPEEngine::RigidData* pData)
{
	if(hasPhysics)
	{

	}
	else
	{
		Object2D* obj = new Object2D(resources,data->filePath,player->GetCameraView());
		obj->position = data->position;
		obj->scaling = data->scale;
		obj->rotation = data->rotation;
		spriteObjs.push_back(obj);
	}
}
//Left Mouse click function (to test things with), will be removed for engine use, as this is a gameplay feature
void Game::LeftMouseClick()
{
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			if(!currentEditorObj->obj2D->hasAnimation)
			{
				Object2D* obj = new Object2D(resources,currentEditorObj->obj2D->quad.textureName,player->GetCameraView());
				obj->SetPosition(currentEditorObj->obj2D->position.x,currentEditorObj->obj2D->position.y,currentEditorObj->obj2D->position.z);
				obj->objName = currentEditorObj->obj2D->objName;
				spriteObjs.push_back(obj);
			}
			else
			{
				Object2D* obj = new Object2D(resources,currentEditorObj->obj2D->quad.textureName,player->GetCameraView(),currentEditorObj->obj2D->GetXSize(),currentEditorObj->obj2D->GetYSize(),currentEditorObj->obj2D->GetXRows(),currentEditorObj->obj2D->GetYRows());
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
			obj->SetPosRotScale(&collisionLock,src->position,src->rotation,src->scaling);
			debugCubes.push_back(obj);
		}
		CreateLevelFile();
	}
}
void Game::ReleaseAll()
{
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
	gui->Release();
	delete player;
	delete gui;
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
	return 0;
}
//speaks for itself
void Game::DestroyLevel()
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
			editorObjs.at(i).obj3D->ReleaseResources();
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
					Object2D* spriteObj = new Object2D(resources,_strdup(name.c_str()),player->GetCameraView());
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
					Object2D* spriteObj = new Object2D(resources,_strdup(name.c_str()),player->GetCameraView(),sizeX,sizeY,xRows,yRows);
					spriteObj->handleWindow = handleWindow;
					std::ostringstream oss;
					oss<<"AnimatedSprite"<<animatedSpriteNr;
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
			string colCheck = "Collision";
			string objName;
			float XAnimRows;
			float YAnimRows;
			float sizeX;
			float sizeY;
			float posx,posy,posz,scalex,scaley,scalez,rotx,roty,rotz;
			
			fin >> name >> objName >>posx >> posy >> posz >> scalex >> scaley >> scalez >> rotx >> roty >> rotz >> sizeX >> sizeY >> XAnimRows >> YAnimRows >> tag;

			if(name.at(name.size()-1) == check[0])
			{
				char* mName = _strdup(name.c_str());
				Object3D* obj = new Object3D(resources,mName);

				obj->objName = objName;
				obj->tag = tag;
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
			else if(name != colCheck)
			{
				if(XAnimRows == 0 && YAnimRows == 0)
				{
					Object2D* obj = new Object2D(resources,_strdup(name.c_str()),player->GetCameraView());
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->tag = tag;
					obj->SetPosition(posx,posy,posz);
					if(scalex != 1 && scaley != 1 && scalex != 1)
					{
						obj->SetScale(scalex,scaley,scalez);
					}
					spriteObjs.push_back(obj);

				}
				else
				{
					Object2D* obj = new Object2D(resources,_strdup(name.c_str()),player->GetCameraView(),sizeX,sizeY,XAnimRows,YAnimRows);
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->tag = tag;	
					obj->SetPosition(posx,posy,posz);
					if(scalex != 1 && scaley != 1 && scalex != 1)
					{
						obj->SetScale(scalex,scaley,scalez);
					}
					spriteObjs.push_back(obj);
				}
			}
			else
			{
				DebugCube* obj = new DebugCube(D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(rotx,roty,rotz),D3DXVECTOR3(-1,-1,-1),D3DXVECTOR3(1,1,1),resources);
				obj->objName = "Collision";
				obj->tag = tag;
				obj->scaling = D3DXVECTOR3(scalex,scaley,scalez);
				obj->AddPositionAndRotation(&collisionLock,0,0,0,0,0,0);
				debugCubes.push_back(obj);
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
	light->Diffuse = lightColor;
	light->Position = position;
	light->Range = range;
	light->Falloff = falloff;
	light->Type = lightType;
	lights.push_back(light);
	return light;
}