#include "../Headers/Game.h"

Game::Game(Game::DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev)
{
	data = b;

	handleWindow = windowHandle;
	resources = resMan;
	inputHandler = inputHand;
	soundHandler = soundHand;
	p_Device = d3dDev;
	LoadLevel();
	Initialize();

	particles.push_back(new Particle(resources,p_Device,"Resources/Particles/Lightning.png",&camera.m_View,D3DXVECTOR3(0,10,10),200,500,1,3));
	particles.at(0)->SetMovement(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,3,0));
}
void Game::Initialize()
{
	camera.lookDir.x = 0;
	camera.lookDir.y = 1;
	camera.lookDir.z = 0;
	sensitivity = 50;
	EditorMode = false;
	soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	angleX = 0;
	angleY = 0;
	camera.position.x = 10;
	camera.position.y = 10;
	camera.position.z = 0;
	camera.lookAt.x = 0;
	camera.lookAt.y = 0;
	camera.lookAt.z = 0;
	keys.resize(256,0);
}
void Game::Update(double deltaTime)
{
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
void Game::FixedUpdate(double deltaTime)
{
	inputHandler->Update();
	DoInput(deltaTime);
	DoCameraStuff(deltaTime);
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
	//Make sure this is last, as setting this before any renders/changes it will make the scene behind 1 frame;
	SetUpCamera();
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
	for(unsigned int i=0;i<lights.size();i++)
	{
		free(lights.at(i));
		delete lights.at(i);
	}
	lights.clear();
}
void Game::SetUpEditorMode()
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
void Game::CreateLevelFile()
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


//returns the camera's looking direction
D3DXVECTOR3 Game::ReturnDirection(float anglesX,float anglesY)
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
void Game::SetCameraLook(float anglesX,float anglesY)
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
//sets the camera for use
void Game::SetUpCamera()
{
	D3DXVECTOR3 m_UpVector(0, 1, 0);
	D3DXMatrixLookAtLH(&camera.m_View, &camera.position, &camera.lookAt, &m_UpVector);
	p_Device->SetTransform(D3DTS_VIEW, &camera.m_View);
	D3DXMatrixPerspectiveFovLH(&camera.m_Projection, D3DX_PI/4, 500/500, 1, 2000);
	p_Device->SetTransform(D3DTS_PROJECTION, &camera.m_Projection);
}
//Camera rotation handling, also needs to be in game.cpp)
void Game::DoCameraStuff(double deltaTime)
{
	angleX-=inputHandler->GetMousePosX()*deltaTime*sensitivity;
	angleY-=inputHandler->GetMousePosY()*deltaTime*sensitivity;
	(angleY < -50 ? angleY = -50 : (angleY> 50 ? angleY = 50 : 0));
	if(inputHandler->GetMousePosX() > 0 || inputHandler->GetMousePosX() < 0)
	{
		//cout <<"deltatime: " <<deltaTime << endl;
	}
	SetCameraLook(angleX,angleY);
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
		data->loop = false;
	}
}

//Left Mouse click function (to test things with), will be removed for engine use, as this is a gameplay feature (will be in Game.cpp)
void Game::LeftMouseClick()
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
//This function is for the extra thread that is (going) to handle all collisions. 
void Game::CollisionThread(void)
{
	while(data->loop)
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
}