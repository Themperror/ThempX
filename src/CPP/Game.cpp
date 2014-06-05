#include "../Headers/Game.h"

Game::Game(ResourceManager::DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev)
{
	data = b;
	handleWindow = windowHandle;
	resources = resMan;
	inputHandler = inputHand;
	soundHandler = soundHand;
	p_Device = d3dDev;
	physics = new PhysXEngine(resources);
	cam = new Camera(resMan,THEMPX_CAM_PERSPECTIVE);
	cam->AspectRatio = 1;
	cam->SetPosition(0,0,0);
	gui = NULL;
	
	soundHandler->LoadWaveFile(std::string("resources/sound/music/menu.wav"),std::string("menu"	));

	menu = new Menu(&data->loop,resources,inputHandler);
	if(data->loop != true) return;
	//Program stays on hold after entering the constructor of Menu, this loads and displays the menu, and it will stay there until you've pressed Start or Quit.
	//after you've pressed start or quit the program will exit menu's constructor and the class is useless afterwards, so we immediatly clean up and continue our game.
	std::cout << "After Menu" << std::endl;
	gui = new GUI(d3dDev,resMan);
	std::cout << "After GUI" << std::endl;
	Initialize();
	std::cout << "After Init" << std::endl;

	LoadLevel("Resources/level.txt");
	LoadItems("Resources/items1.txt");
	LoadEnemies("Resources/enemies1.txt");
	std::cout << "After LoadLevel" << std::endl;
}
Game::Object2DData Game::CreateObject2DData(char* filePath,bool hasAnim, D3DXVECTOR3 pos,D3DXVECTOR3 scale, D3DXVECTOR2 rows, PhysicsData pData)
{
	Game::Object2DData d;
	d.Nullify();
	d.physics = pData;
	d.filePath = filePath;
	d.hasAnimation = hasAnim;
	d.position = pos;
	d.scale = scale;
	d.xRowsAnim = (int)rows.x;
	d.yRowsAnim = (int)rows.y;
	return d;
}
Game::Object3DData Game::CreateObject3DData(char* filePath,D3DXVECTOR3 pos,D3DXVECTOR3 scale,D3DXVECTOR3 rot, PhysicsData pData)
{
	Game::Object3DData d;
	d.Nullify();
	d.filePath = filePath;
	d.physics = pData;
	d.position = pos;
	d.scale = scale;
	d.rotation = rot;
	return d;
}
void Game::Initialize()
{
	keys.resize(256,0);
	currentEditorObjIndex = 0;
	qualityLevel = 0;
	editorObjectDistance = 10;
	levelComplete = false;
	wireframe = false;
	camFall = false;
	scaleMultiplier = 3;
	EditorMode = false;
	player = new Player(cam,physics->player,gui,&gui->health,&gui->armour);
	dCube = new DebugCube(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),-D3DXVECTOR3(1,1,1),D3DXVECTOR3(1,1,1),resources);
	//Loading sounds
	
	soundHandler->LoadWaveFile(std::string("resources/sound/piew.wav"),std::string("Piew"));
	soundHandler->LoadWaveFile(std::string("resources/sound/DoorDown.wav"),std::string("DoorDown"));
	soundHandler->LoadWaveFile(std::string("resources/sound/DoorUp.wav"),std::string("DoorUp"));
	soundHandler->LoadWaveFile(std::string("resources/sound/MAN1DIE1.wav"),std::string("MAN1DIE1"));
	soundHandler->LoadWaveFile(std::string("resources/sound/MAN1DIE2.wav"),std::string("MAN1DIE2"));
	soundHandler->LoadWaveFile(std::string("resources/sound/MAN1HIT1.wav"),std::string("MAN1HIT1"));
	soundHandler->LoadWaveFile(std::string("resources/sound/MAN1HIT2.wav"),std::string("MAN1HIT2"));
	soundHandler->LoadWaveFile(std::string("resources/sound/MAN1HIT3.wav"),std::string("MAN1HIT3"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SAMHIT1.wav"),std::string("SAMHIT1"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SAMHIT2.wav"),std::string("SAMHIT2"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SAMHIT3.wav"),std::string("SAMHIT3"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SAMDIE1.wav"),std::string("SAMDIE1"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWIPE1.wav"),std::string("SWIPE1"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWIPE2.wav"),std::string("SWIPE2"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWIPE3.wav"),std::string("SWIPE3"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWIPE4.wav"),std::string("SWIPE4"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWIPE5.wav"),std::string("SWIPE5"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWORDHIT.wav"),std::string("SWORDHIT"));
	soundHandler->LoadWaveFile(std::string("resources/sound/pickup.wav"),std::string("pickup"));
	soundHandler->LoadWaveFile(std::string("resources/sound/deur.wav"),std::string("deur"));
	soundHandler->LoadWaveFile(std::string("resources/sound/DIGMARK.wav"),std::string("digmark"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWONWAL1.wav"),std::string("SWONWAL1"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWONWAL2.wav"),std::string("SWONWAL2"));
	soundHandler->LoadWaveFile(std::string("resources/sound/SWONWAL3.wav"),std::string("SWONWAL3"));
	soundHandler->LoadWaveFile(std::string("resources/sound/music/boss.wav"),std::string("boss"));
	soundHandler->LoadWaveFile(std::string("resources/sound/music/level.wav"),std::string("level"));
	soundHandler->LoadWaveFile(std::string("resources/sound/zeizo.wav"),std::string("zeizo"));
	
	Item* completeTrigger = new Item(&Game::LevelComplete,this,D3DXVECTOR3(186,-10,68),D3DXVECTOR3(5,5,5));
	items.push_back(completeTrigger);

	physics->player->setPosition(PxExtendedVec3(-11.18f,-8.94f,34.4f));
	//cam->SetPosition(5,7.0f,0);
	//resources->CreateTextObject("Arial","\n    THIS IS TESTING TEXT",12, 0, 0, 30, 20, 0xFFFF0000);

	player->hasRedKey = false;
	player->hasGreenKey =false;
}
void Game::LoadItems(char* txtPath)
{
	ifstream fin(txtPath);
	if (!fin.good())
	{
		MessageBox(handleWindow,"Could Not Find items.txt, scene will be empty", "Game::LoadItems()",MB_OK);
		std::cout << "Cannot find " << txtPath  << std::endl;
	}
	else
	{
		string path, objName;
		float posx,posy,posz,scalex,scaley,colx,coly,colz;
		string str;
		while(getline(fin, str))
		{
			path = "";
			objName = "";
			posx =0;posy=0;posz=0;scalex=0;scaley=0;colx=0;coly=0;colz=0;

			fin >> path >> objName >>posx >> posy >> posz >> scalex >> scaley >> colx >> coly >> colz;

			Item* item = new Item(objName,&Game::ItemTrigger,this,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(colx,coly,colz));
			item->obj2D = new Object2D(resources,_strdup(path.c_str()),&cam->GetView());
			item->obj2D->scaling = D3DXVECTOR3(scalex,scaley,scalex);

			items.push_back(item);
		}
		fin.close();
	}
}
void Game::LoadEnemies(char* txtPath)
{
	ifstream fin(txtPath);
	if (!fin.good())
	{
		std::cout << "Cannot find " << txtPath  << "No enemies are loaded"<< std::endl;
	}
	else
	{
		string path, tag;
		float health,damage,moveSpeed,posx,posy,posz,scalex,scaley,colRadius,colHeight;
		int animRowsX,animRowsY;
		string str;
		while(getline(fin, str))
		{
			path = "";
			tag = "";
			posx =0;posy=0;posz=0;scalex=0;scaley=0;colRadius=0;colHeight=0;
			animRowsX=0;animRowsY=0;

			fin >> path >> tag >>health >> damage >> moveSpeed >> posx >> posy >> posz >> scalex >> scaley >> animRowsX >> animRowsY >> colRadius >> colHeight;

			if(path.compare("") == 0)
			{
				std::cout <<"something"<<std::endl;
			}
			PlaceEnemy(path,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,scalex),animRowsX,animRowsY,colRadius,colHeight);
			Enemy* e = enemies.at(enemies.size()-1);
			e->SetSpeed(moveSpeed);
			e->SetHealth(health);
			e->SetDamage(damage);
		}
		fin.close();
	}
}
void Game::LevelComplete()
{
	if(levelComplete == false)
	{
		levelComplete = true;
		std::cout << "Level Completed" << std::endl;
		gui->levelCompleteGUI->render = true;
		gui->levelCompleteGUI->animations.at(0).showEnd = true;
		gui->PlayAnimation(gui->levelCompleteGUI,"Complete");
		gui->GetGUIObj("Resources/GUI/HealthArmor.png")->render = false;
//		gui->GetGUIObj("Resources/GUI/WeaponAmmo.png")->render = false;
		gui->GetGUIObj("Resources/GUI/CharacterState.png")->render = false;
		gui->GetGUIObj("Resources/Sprites/GreenKey.png")->render = false;
		gui->GetGUIObj("Resources/Sprites/RedKey.png")->render = false;
		
		gui->armourText->render = false;
		gui->healthText->render = false;
	}
}
void Game::PlaySound(std::string name)
{
	soundHandler->PlayWaveFile(_strdup(name.c_str()));
}
void Game::ItemTrigger(Item* item)
{
	item->name.resize(25);
	GUI::GUITexture* render = gui->GetGUIObj(item->obj2D->quad.textureName);
	if(render != NULL) render->render = true;
	soundHandler->PlayWaveFile("pickup");
	std::cout << "--------------------------------------------------------\n|                                                      | \n";
	std::cout << "|             Picked up item: " << item->name <<"|";
	std::cout << "\n|                                                      | \n--------------------------------------------------------"<< std::endl;
	
	if(strcmp(_strdup(item->name.c_str()),"Red_Keycard") == 0)
	{
		player->hasRedKey = true;
	}
	else if(strcmp(_strdup(item->name.c_str()),"Green_Keycard") == 0)
	{
		player->hasGreenKey = true;
	}
	else if(strcmp(_strdup(item->name.c_str()),"Health") == 0)
	{
		gui->health+=20;
		if(gui->health > 100)
		{
			gui->health = 100;
		}
	}
	else if(strcmp(_strdup(item->name.c_str()),"Armor") == 0)
	{
		gui->armour += 10;
		if(gui->armour > 100)
		{
			gui->armour = 100;
		}
	}				
	for(unsigned int i = 0; i < items.size(); i++)
	{
		if(items.at(i) == item)
		{
			items.erase(items.begin()+i);
		}
	}
	item->Release();
	delete item;
}
void Game::RemoveEnemyCollision(Enemy* enemy)
{
	if(enemy->GetActor() != NULL)
	{
		physics->RemoveActor(enemy->GetActor()->isRigidDynamic());
		physics->RemoveActor(enemy->GetActor()->isRigidStatic());
		enemy->SetActorNull();
	}
}
void Game::ReleaseEnemy(Enemy* enemy)
{
	for(unsigned int i = 0; i < enemies.size();i++)
	{
		if(enemies.at(i) == enemy)
		{
			enemies.erase(enemies.begin()+i);
		}
	}
	enemy->Release();
	delete enemy;
}

void Game::Update(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
	if(deltaTimeF < 0.8f)
	{
		if(!soundHandler->IsPlaying("level"))
		{
			soundHandler->PlayWaveFile("level",85);
		}
		if(gui->health <= 0)
		{
			if(!player->isDead)
			{
				player->isDead = true;
				player->lockCam = false;
				camFall = true;
				player->physicsPlayer->setPosition(PxExtendedVec3(-99,-99,-99));
				gui->gameOverText->render = true;
				gui->health = 0;
			}
		}
		if(camFall)
		{
			cam->AddPosition(0,-2.3f*deltaTimeF,0);
			cam->Update(deltaTimeF,-4,-3);
			if(cam->GetPosition().y < -12.2f)
			{
				camFall = false;
			}
		}
		resources->SetCameraView(&cam->GetView());
		for(unsigned int i = 0; i < enemies.size();i++)
		{
			Enemy* enemy = enemies.at(i);
			Object2D* obj = enemy->GetObj();
			D3DXVECTOR3 temp = cam->ReturnDirection(cam->angleX-90,cam->angleY);
			PxVec3 playerRight = PxVec3(temp.x,0,temp.z);
			enemies.at(i)->SetInfo(player->GetPxVec3Position(),playerRight);
			enemies.at(i)->Update(deltaTimeF);
			
			if(obj->hasAnimation)
			{
				if(enemy->IsDead())
				{
					RemoveEnemyCollision(enemy);
				}
			}
		}
		if(levelComplete)
		{
			if(gui->levelCompleteGUI->animations.at(0).isFinished)
			{
				levelComplete = false;
				player->physicsPlayer->setPosition(PxExtendedVec3(-11.18f,-9.94f,34.4f));
				player->hasGreenKey = false;
				player->hasRedKey = false;
				DestroyLevel();
				LoadLevel("Resources/Level2.txt");
				LoadItems("Resources/items2.txt");
				LoadEnemies("Resources/enemies2.txt");
				gui->levelCompleteGUI->render = false;
				gui->GetGUIObj("Resources/GUI/HealthArmor.png")->render = true;
				//gui->GetGUIObj("Resources/GUI/WeaponAmmo.png")->render = true;
				gui->GetGUIObj("Resources/GUI/CharacterState.png")->render = true;
				gui->armourText->render = true;
				gui->healthText->render = true;
				Sleep(2000);
			}
		}
		for(unsigned int i =0; i < doors.size(); i++)
		{
			doors.at(i)->Update(deltaTimeF);
		}
		for(unsigned int i = 0; i < items.size();i++)
		{
			items.at(i)->CheckTrigger(&cam->GetPosition());
		}
		for(unsigned int i = 0; i < particles.size(); i++)
		{
			particles.at(i)->Update(deltaTimeF);
			if(particles.at(i)->GetBurstFinished())
			{
				particles.at(i)->Release();
				delete particles.at(i);
				particles.erase(particles.begin()+i);
			}
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
		PxVec3 moveDir = DoInput(deltaTimeF);
		if(!player->isDead)
		{
			player->Move(moveDir,deltaTimeF);
			HandlePlayerCollisions(moveDir);
			cam->Update(deltaTimeF,inputHandler->GetMousePosX(),inputHandler->GetMousePosY());
		}
		//physx::PxReal elapsed = 1.0f/60.0f;
	
		physics->gScene->simulate(deltaTimeF);
		while(!physics->gScene->fetchResults())
		{
		
		}
		gui->Update(deltaTimeF);
	}
}
void Game::HandlePlayerCollisions(PxVec3 moveDir)
{
	
	/*if(physics->player->getPosition().y > 20 || physics->player->getPosition().y < -20)
	{
		physics->player->setPosition(PxExtendedVec3(10,-5,0));
	}*/
	/*PxControllerState state;
	physics->player->getState(state);
	PxRigidDynamic* rigidBody = NULL;
	if(state.touchedShape != NULL)
	{
		rigidBody = state.touchedShape->getActor()->isRigidDynamic();
		if(rigidBody != NULL)
		{
			//rigidBody->addForce(moveDir*100,PxForceMode::eIMPULSE);
			//std::cout << "Rigidbody hit" << std::endl;
		}
	}*/
}
void Game::FixedUpdate(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
}
void Game::Render()
{
	D3DXMATRIX* camView = &cam->GetView();
	
	p_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	p_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	for(unsigned int i = 0; i < enemies.size();i++)
	{
		Object2D* obj = enemies.at(i)->GetObj();
		obj->cameraView = camView;
		obj->Draw();
		enemies.at(i)->RenderBullets(camView);
	}
	for(unsigned int i =0; i < sprites.size();i++)
	{
		sprites.at(i)->obj->cameraView = camView;
		sprites.at(i)->obj->Draw();
	}
	for(unsigned int i = 0; i < items.size();i++)
	{
		Item* item = items.at(i);
		if(item->obj2D != NULL)
		{
			item->obj2D->cameraView = &cam->GetView();
		}
		item->Draw();
	}
	for(unsigned int i = 0; i < particles.size(); i++)
	{
		particles.at(i)->Draw(camView);
	}
	if(EditorMode)
	{
		IFOBJ2D
		{
			p_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			p_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			currentEditorObj->obj2D->position = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			currentEditorObj->obj2D->Update(0.0166f);
			currentEditorObj->obj2D->cameraView = camView;
			currentEditorObj->obj2D->Draw();
		}
		else IFOBJ3D
		{
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
			currentEditorObj->obj3D->position = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			currentEditorObj->obj3D->DrawModel();
		}
		else IFCOL
		{
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
			p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
			currentEditorObj->col->position = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			currentEditorObj->col->Draw();
		}
	}
	physics->Draw(true,false,false);
	p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);

	D3DXMATRIX rotation;
	D3DXMATRIX position;
	D3DXMATRIX mScale;
	D3DXMATRIX rotScale;
	D3DXMATRIX world;

	for(unsigned int i = 0; i < modelObjs.size();i++)
	{
		if(modelObjs.at(i)->actor != NULL)
		{
			PxRigidDynamic* a = modelObjs.at(i)->actor->isRigidDynamic();
			if(a != NULL)
			{
				Object3D* obj = modelObjs.at(i)->obj;
				PxTransform t = a->getGlobalPose();
				PxVec3 pos = t.p;
				PxQuat rot = t.q;
				D3DXMatrixRotationQuaternion(&rotation,&D3DXQUATERNION(t.q.x,t.q.y,t.q.z,t.q.w));
				D3DXMatrixScaling(&mScale,obj->scaling.x,obj->scaling.y,obj->scaling.z);
				D3DXMatrixTranslation(&position,t.p.x,t.p.y,t.p.z);
				D3DXMatrixMultiply(&rotScale,&rotation,&mScale);
				D3DXMatrixMultiply(&world,&rotScale,&position);
				obj->worldMatrix = world;
			}
		}
		modelObjs.at(i)->obj->DrawModel();	
	}
	for(unsigned int i = 0; i < debugCubes.size();i++)
	{
		debugCubes.at(i)->Draw();
	}
	gui->Render();
	cam->RenderCamera(THEMPX_CAM_PERSPECTIVE);
	resources->DrawAllText();
}
void Game::ReloadGUI()
{
	gui->ReloadGUI();
}

void Game::UndoEditorAction()
{
	std::cout << "Undo" << std::endl;
	if(GetLastAction() == ThrowCube || GetLastAction() == Dynamic)
	{
		physics->DeleteLastDynamic();
		RemoveLastAction();
	}
	if(GetLastAction() == Static)
	{
		Object3D* obj = GetLastObject3D();
		for(unsigned int i = 0; i < modelObjs.size();i++)
		{
			if(obj == modelObjs.at(i)->obj)
			{
				delete modelObjs.at(i)->obj;
				delete modelObjs.at(i);
				modelObjs.erase(modelObjs.begin()+i);
			}
		}
		physics->DeleteLastStatic();
		RemoveLastAction();
	}
	if(GetLastAction() == PlacedDoor)
	{
		Door* door = doors.at(doors.size()-1);
		doors.erase(doors.begin()+doors.size()-1);
		physics->RemoveActor(door->actor->isRigidDynamic());
		delete door->obj;
		delete door;
		RemoveLastAction();
	}
}
PxVec3 Game::DoInput(float dT)
{
	PxVec3 moveDir = PxVec3(0,physics->playerGravity,0);
	PxControllerState state;
	physics->player->getState(state);
	if(state.standOnObstacle)
	{
		physics->playerGravity = 0;
	}
	else
	{
		physics->playerGravity -= dT;
		if(physics->playerGravity < -0.98f)
		{
			physics->playerGravity = -0.98f;
		}
	}
	if(KeyPressed(DIK_SPACE) == 2)
	{
		physics->playerGravity = 0.4f;
	}
	if(KeyPressed(DIK_LCONTROL) && KeyPressed(DIK_Z) == 2)
	{
		UndoEditorAction();
	}
	
	if(KeyPressed(DIK_CAPSLOCK) == 2)
	{
		data->lockCursor = !data->lockCursor;
	}
	if(KeyPressed(DIK_Q) == 2)
	{
		PxRaycastBuffer hit = RayFromPlayer();
		if(hit.block.actor != NULL)
		{
			PxRigidDynamic* p = hit.block.actor->isRigidDynamic();
			if(p != NULL && !p->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC))
			{
				p->setAngularVelocity(ToVec3(&(cam->GetLookDir()*5)));
				p->setLinearVelocity(ToVec3(&(cam->GetLookDir()*5)));
			}
		}
		std::cout << "Player Pos X: " << physics->player->getFootPosition().x << " Y: " << physics->player->getFootPosition().y << " Z: "<< physics->player->getFootPosition().z << std::endl;
		player->TakeDamage(10);
	}
	if(KeyPressed(DIK_COMMA) == 2)
	{
		data->changeDisplay = true;
		data->windowed = false;
		data->renderSizeX = 800;
		data->renderSizeY = 600;
		data->devmodeIndex = resources->GetDevMode(1024,800);
		return PxVec3(0,0,0);
	}
	if(KeyPressed(DIK_M) == 2)
	{
		data->changeDisplay = true;
		data->windowed = true;
		data->renderSizeX = 800;
		data->renderSizeY = 600;
		data->windowSizeX = 800;
		data->windowSizeY = 600;
		return PxVec3(0,0,0);
	}
	
	int speed = 60;
	float scaleSpeed = 0.9f;
	if(KeyPressed(DIK_LSHIFT))
	{
		speed = 10;
		scaleSpeed = 0.4f;
	}
	if(!player->isDead)
	{
		if(KeyPressed(DIK_W))
		{
			moveDir+= PxVec3(cam->GetLookDir().x *dT*speed,0,cam->GetLookDir().z *dT*speed);
		}
		if(KeyPressed(DIK_S))
		{
			moveDir+= PxVec3(-cam->GetLookDir().x *dT*speed,0,-cam->GetLookDir().z *dT* speed);
		}
		if(KeyPressed(DIK_D))
		{
			D3DXVECTOR3 temp = cam->ReturnDirection(cam->angleX-90,cam->angleY);
			moveDir+= PxVec3(temp.x *dT*(speed/1.5f),0,temp.z *dT*(speed/1.5f));
		}
		if(KeyPressed(DIK_A))
		{
			D3DXVECTOR3 temp = cam->ReturnDirection(cam->angleX+90,cam->angleY);
			moveDir+= PxVec3(temp.x *dT*(speed/1.5f),0,temp.z *dT*(speed/1.5f));
		}
		if(KeyPressed(DIK_E) == 2)
		{
			OpenDoor();
		}
		if(KeyPressed(DIK_F1) == 2 && KeyPressed(DIK_LALT) == 1)
		{
			gui->health = 100;
			soundHandler->PlayWaveFile("zeizo");
		}
		if(KeyPressed(DIK_F2) == 2 && KeyPressed(DIK_LALT) == 1)
		{
			gui->armour = 100;
			soundHandler->PlayWaveFile("zeizo");
		}
		if(KeyPressed(DIK_F3) == 2 && KeyPressed(DIK_LALT) == 1)
		{
			player->hasGreenKey = true;
			gui->GetGUIObj("Resources/Sprites/GreenKey.png")->render = true;
			soundHandler->PlayWaveFile("zeizo");
		}
		if(KeyPressed(DIK_F5) == 2 && KeyPressed(DIK_LALT) == 1)
		{
			player->hasRedKey = true;
			gui->GetGUIObj("Resources/Sprites/RedKey.png")->render = true;
			soundHandler->PlayWaveFile("zeizo");
		}
		if(inputHandler->MouseButtonDown(0))
		{
			//LeftMouseClick();
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
			RightMouseClick();
			if(mouseRightJustDown == false)
			{
				mouseRightJustDown = true;
			}
		}
		else
		{
			mouseRightJustDown = false;
		}
		if(KeyPressed(DIK_G) == 2)
		{
			RightMouseClick();
		}
	}
	else
	{
		if(KeyPressed(DIK_R) == 2)
		{
			player->isDead = false;
			player->lockCam = true;
			player->hasGreenKey = false;
			player->hasRedKey = false;
			player->cam->angleX = 0;
			player->cam->angleY = 0;
			gui->armour = 0;
			gui->health = 100;
			camFall = false;
			gui->gameOverText->render = false;
			DestroyLevel();
			LoadLevel("Resources/level.txt");
			LoadItems("Resources/items1.txt");
			LoadEnemies("Resources/enemies1.txt");
			player->physicsPlayer->setPosition(PxExtendedVec3(-11.18f,-8.94f,34.4f));
		}
	}
	if(KeyPressed(DIK_Z) == 2)
	{
		physics->RemoveAllBoxes();
	}
	if(KeyPressed(DIK_NUMPADPLUS) == 2)
	{
		cam->sensitivity += 0.05f;
		std::cout << "Mouse Sensitivity set to: " <<cam->sensitivity << std::endl;
	}
	if(KeyPressed(DIK_NUMPADMINUS) == 2)
	{
		cam->sensitivity -= 0.05f;
		std::cout << "Mouse Sensitivity set to: " <<cam->sensitivity << std::endl;
	}
	if(KeyPressed(DIK_1))
	{
		cam->FoV += 0.1f*dT;
		std::cout << "Field of View: "<< cam->FoV << std::endl;
	}
	if(KeyPressed(DIK_2))
	{
		cam->FoV -= 0.1f*dT;
		std::cout << "Field of View: "<< cam->FoV << std::endl;
	}
	if(KeyPressed(DIK_3))
	{
		cam->AspectRatio += 0.1f*dT;
		std::cout << "Aspect Ratio: "<< cam->AspectRatio << std::endl;
	}
	if(KeyPressed(DIK_4))
	{
		cam->AspectRatio -= 0.1f*dT;
		std::cout << "Aspect Ratio: "<< cam->AspectRatio << std::endl;
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
	if(EditorMode)
	{
		if(KeyPressed(DIK_RCONTROL) == 2)
		{
			scaleMultiplier = -scaleMultiplier;
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
				currentEditorObj->obj3D->scaling.x += scaleMultiplier*dT*scaleSpeed;
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
				currentEditorObj->obj3D->scaling.y += scaleMultiplier*dT*scaleSpeed;
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
				currentEditorObj->obj3D->scaling.z += scaleMultiplier*dT*scaleSpeed;
			}
		}
		if(KeyPressed(DIK_I) == 2)
		{
			IFCOL
			{
				currentEditorObj->col->rotation.x += 5*dT*5;
			}
			else IFOBJ3D
			{
				currentEditorObj->obj3D->rotation.x += 45*ToRadian;
			}
		}
		if(KeyPressed(DIK_K) == 2)
		{
			IFCOL
			{
				currentEditorObj->col->rotation.x -= 5*dT*5;
			}
			else IFOBJ3D
			{
				currentEditorObj->obj3D->rotation.x -= 45*ToRadian;
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
			if(currentEditorObjIndex < 0)
			{
				currentEditorObjIndex = editorObjs.size()-1;
			}
			currentEditorObj = &editorObjs.at(currentEditorObjIndex);
			if(currentEditorObj->obj2D != NULL && currentEditorObj->obj2D->hasAnimation)
			{
				currentEditorObj->obj2D->PlayAnimation("WalkForward");
				currentEditorObj->obj2D->SetUVValues();
			}
		}
		if(KeyPressed(DIK_9) == 2)
		{
			currentEditorObjIndex++;
			if(currentEditorObjIndex > (signed int)editorObjs.size()-1)
			{
				currentEditorObjIndex = 0;
			}
			currentEditorObj = &editorObjs.at(currentEditorObjIndex);
		
			if(currentEditorObj->obj2D != NULL && currentEditorObj->obj2D->hasAnimation)
			{
				currentEditorObj->obj2D->PlayAnimation("WalkForward");
				currentEditorObj->obj2D->SetUVValues();
			}
		}
		editorObjectDistance+=inputHandler->GetMouseScroll()*dT;
	}
	if(KeyPressed(DIK_F))
	{
		soundHandler->PlayWaveFile("digmark",81);
		D3DXVECTOR3 camPos = cam->GetPosition()+cam->GetLookDir()*3.5f;
		physics->ThrowCube(PxVec3(camPos.x,camPos.y,camPos.z),PxVec3(cam->GetLookDir().x*60,cam->GetLookDir().y*60,cam->GetLookDir().z*60));
		lastAction.push_back(ThrowCube);
	}

	
	if(inputHandler->KeyPressed(DIK_ESCAPE))
	{
		soundHandler->Stop("level");
		menu->ShowMenu(Menu::PauseScreen);
		
		gui->SetTextRenderingTrue();
	}
	return moveDir;
}
bool Game::Create3DObject(bool hasPhysics, Object3DData* data)
{
	if(data != NULL)
	{
		Model* model = new Model();
		if(hasPhysics)
		{
			Object3D* obj = new Object3D(resources,data->filePath);
			obj->scaling = data->scale;
			model->obj = obj;
			PxRigidActor* actor = NULL;
			if(data->physics.cType == PhysicsData::Mesh)
			{
				physics->BakeTriangleMesh(obj->model.mesh,PxVec3(data->position.x,data->position.y,data->position.z),PxVec3(obj->scaling.x,obj->scaling.y,obj->scaling.z),data->physics.flipNormals);
			}
			else if(data->physics.cType == PhysicsData::BoundingBox)
			{
				actor = physics->CreateCube(PxVec3(data->position.x,data->position.y,data->position.z),PxVec3(data->rotation.x,data->rotation.y,data->rotation.z),ToVec3(&obj->GetBoundingBox()),10,data->physics.isStatic,data->physics.isKinematic);//rot scale mass statis kinematic);
			}
			else
			{
				actor = physics->CreateCube(PxVec3(data->position.x,data->position.y,data->position.z),PxVec3(data->rotation.x,data->rotation.y,data->rotation.z),PxVec3(data->scale.x,data->scale.y,data->scale.z),10,data->physics.isStatic,data->physics.isKinematic);//rot scale mass statis kinematic);
			}
			model->actor = actor;
			obj->position = data->position;
			obj->scaling = data->scale;
			obj->rotation = data->rotation;
			modelObjs.push_back(model);
			//physics->Create3DPhysicsObject(data->filePath,pData);
		}
		else
		{
			Object3D* obj = new Object3D(resources,data->filePath);
			model->obj = obj;
			obj->position = data->position;
			obj->scaling = data->scale;
			obj->rotation = data->rotation;
			modelObjs.push_back(model);
		}
		return true;
	}
	return false;
}
bool Game::CreateEnemy(bool hasPhysics, Object2DData* data)
{
	Object2D* obj;
	Enemy* enemy = new Enemy(resources,physics,gui);
	enemy->SetHealth(60);
	enemy->SetDamage(10);
	if(hasPhysics)
	{
		if(data->hasAnimation)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,ToVec3(&data->position), 10, data->physics.isStatic, data->physics.isKinematic);
			enemy->SetActor(actor);
			enemy->SetColRadius(data->physics.radius);
			enemy->SetColHeight(data->physics.capsuleHeight);
			actor->setName("Enemy");
			PhysicsUserData* uData = new PhysicsUserData();
			uData->Nullify();
			uData->related2D = obj;
			uData->enemy = enemy;
			actor->userData = uData; //userdata = een void* type variabele (kun je dus alles instoppen)
			obj->PlayAnimation("WalkForward");
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
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			obj->PlayAnimation("WalkForward");
		}
		else
		{
			return false;
		}
	}
	enemy->SetObj(obj);
	enemies.push_back(enemy);
	return true;
}
bool Game::CreateAnimatedSprite(bool hasPhysics, Object2DData* data)
{
	Object2D* obj;
	Sprite* sprite = new Sprite();
	if(hasPhysics)
	{
		if(data->hasAnimation)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10, data->physics.isStatic, data->physics.isKinematic);
			actor->setName("Sprite");
			obj->PlayAnimation("Idle");
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
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			obj->PlayAnimation("Idle");
		}
		else
		{
			return false;
		}
	}
	
	sprite->obj = obj;
	sprites.push_back(sprite);
	return true;
}
bool Game::CreateStaticSprite(bool hasPhysics, Object2DData* data)
{
	if(data != NULL)
	{
		Sprite* sprite = new Sprite;
		Object2D* obj;

		if(hasPhysics)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10,data->physics.isStatic,data->physics.isKinematic);
			sprite->actor = actor;
		}
		else
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			//physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10, false, true);
		}
		sprite->obj = obj;
		sprites.push_back(sprite);
		return true;
	}
	return false;
}
void Game::OpenDoor()
{
	PxRaycastBuffer hit = RayFromPlayer();
	if(hit.block.actor != NULL)
	{
		PxRigidDynamic* p = hit.block.actor->isRigidDynamic();
		if(p != NULL && p->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC))
		{
			if(p->getName() != NULL && strcmp(p->getName(),"Door") == 0)
			{
				Door* door;
				door = (Door*)p->userData;
				if(door->breakable == false)
				{
					if(door->requiresGreenKey && player->hasGreenKey || door->requiresRedKey && player->hasRedKey || !door->requiresGreenKey && !door->requiresRedKey)
					{
						door->activated = true;
					}
				}
			}
		}
	}
}
void Game::PlaceEnemy(std::string textureString, D3DXVECTOR3 position, D3DXVECTOR3 scaling, int xRows,int yRows, float colRadius, float colHeight)
{
	CreateEnemy(true,&CreateObject2DData(_strdup(textureString.c_str()),true,position,scaling,D3DXVECTOR2((float)xRows,(float)yRows),CreatePhysicsData(colRadius,colHeight)));
	Enemy* e = enemies.at(enemies.size()-1);
	Object2D* obj = enemies.at(enemies.size()-1)->GetObj();
	obj->objName = "Enemy";
	obj->PlayAnimation("WalkForward");
	e->GetActor()->setGlobalPose(PxTransform(ToVec3(&position)));
}
void Game::LeftMouseClick()
{
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			if(!currentEditorObj->obj2D->hasAnimation)
			{
				CreateStaticSprite(true,&CreateObject2DData(currentEditorObj->obj2D->quad.textureName,false,cam->GetPosition()+cam->GetLookDir()*editorObjectDistance,currentEditorObj->obj2D->scaling,D3DXVECTOR2(0,0),CreatePhysicsData(1,0,true,false)));
				Object2D* obj = sprites.at(sprites.size()-1)->obj;
				obj->objName = currentEditorObj->obj2D->objName;
				obj->tag = currentEditorObj->obj2D->tag;
			}
			else
			{
				PlaceEnemy(currentEditorObj->obj2D->quad.textureName,cam->GetPosition()+cam->GetLookDir()*editorObjectDistance,currentEditorObj->obj2D->scaling,currentEditorObj->obj2D->GetXRows(),currentEditorObj->obj2D->GetYRows(),1.5f,3);
			}
		}
		else if(currentEditorObj->obj3D != NULL)
		{
			D3DXVECTOR3 pos = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;

			Create3DObject(true,&CreateObject3DData(currentEditorObj->obj3D->model.meshPath,pos,currentEditorObj->obj3D->scaling,currentEditorObj->obj3D->rotation,CreatePhysicsData(ToVec3(&currentEditorObj->obj3D->GetBoundingBox()),true,false,0)));
			GetLastObject3D()->tag = currentEditorObj->obj3D->tag;
			GetLastObject3D()->objName = currentEditorObj->obj3D->objName;
			lastAction.push_back(Static);

			/*
			Door* door = new Door(&Game::PlaySound,this);
			soundHandler->PlayWaveFile("deur");
			D3DXVECTOR3 pos = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			Create3DObject(true,&CreateObject3DData(currentEditorObj->obj3D->model.meshPath,pos,currentEditorObj->obj3D->scaling,currentEditorObj->obj3D->rotation,CreatePhysicsData(PxVec3(currentEditorObj->obj3D->scaling.x/2,currentEditorObj->obj3D->scaling.y/2,currentEditorObj->obj3D->scaling.z/2),false,true,0)));
			//Object3D* obj = new Object3D(resources,currentEditorObj->obj3D->model.meshPath);
			Object3D* obj = GetLastObject3D();
			modelObjs.erase(modelObjs.begin()+modelObjs.size()-1);
			obj->objName = currentEditorObj->obj3D->objName;
			door->obj = obj;
			door->position = pos;
			door->actor = physics->GetLastDynamic();
			door->actor->userData = door;
			if(strcmp(_strdup(currentEditorObj->obj3D->tag.c_str()),"RedKey") == 0)
			{
				door->requiresRedKey = true;
			}
			else if(strcmp(_strdup(currentEditorObj->obj3D->tag.c_str()),"GreenKey") == 0)
			{
				door->requiresGreenKey = true;
			}
			door->actor->setName(_strdup(currentEditorObj->obj3D->tag.c_str()));
			doors.push_back(door);
			lastAction.push_back(PlacedDoor);*/
		}
		CreateLevelFile();
	}
	else
	{
		if(gui->guiObjs.size() > 0) // need a dynamic system to apply animations to with events //in txt say type: Attack and the animation with the Attack tag will be used for attack.
		{
			gui->attackGUI->mirrored = !gui->attackGUI->mirrored;
			if(!gui->attackGUI->mirrored)
			{
				gui->PlayAnimation(gui->attackGUI,"Attack");

			}
			else
			{
				gui->PlayAnimation(gui->attackGUI,"ReversedAttack");
			}
		}
		
		std::vector<std::string> names;
		names.push_back("SWIPE1");
		names.push_back("SWIPE2");
		names.push_back("SWIPE3");
		names.push_back("SWIPE4");
		names.push_back("SWIPE5");
		soundHandler->PlayRandom(&names,100);

		PxRaycastBuffer hit = RayFromPlayer();
		if(hit.block.actor != NULL)
		{
			//std::cout << "Raycast Single hit" << std::endl;
			PxRigidDynamic* p = hit.block.actor->isRigidDynamic();
			if(p != NULL && p->userData != NULL && p->getName() != NULL && strcmp(p->getName(),"Enemy") == 0)
			{
				PhysicsUserData* data = (PhysicsUserData*)p->userData;
				if(data != NULL)
				{
					if(data->related2D != NULL && !data->enemy->IsDead())
					{
						data->enemy->TakeDamage(10);
						soundHandler->PlayWaveFile("SWORDHIT",100);
					}
				}
			}
			else if(p != NULL && p->userData != NULL && p->getName() != NULL && strcmp(p->getName(),"BreakableDoor") == 0)
			{
				Door* door = (Door*)p->userData;
				if(data !=NULL)
				{
					door->health -= 5;
					std::vector<std::string> names;
					names.push_back("SWONWAL1");
					names.push_back("SWONWAL2");
					names.push_back("SWONWAL3");
					soundHandler->PlayRandom(&names,100);
					if(door->health < 1)
					{
						particles.push_back(new Particle(resources,resources->GetDevice(),"Resources/Particles/Lightning.png",&cam->GetView(),door->obj->position+D3DXVECTOR3(0,1,0),true,5,D3DXVECTOR3(0,-0.5f,0),D3DXVECTOR3(0,-1,0)));
						Particle* p = particles.at(particles.size()-1);
						p->SetEllipsoid(D3DXVECTOR3(4,5,2));
						p->SetLifeTime(10,15);
						p->SetMovement(D3DXVECTOR3(0,-2.5f,0),D3DXVECTOR3(0,-7.0f,0));
						
						door->actor->setGlobalPose(PxTransform(PxVec3(0,500,0)));
						for(unsigned int i =0; i < doors.size(); i++)
						{
							if(doors.at(i) == door)
							{
								doors.erase(doors.begin()+i);
							}
						}
						delete door;

					}
				}
			}
		}
	}
}

PxRaycastBuffer Game::RayFromPlayer()
{
	D3DXVECTOR3 camPos = cam->GetPosition();
	D3DXVECTOR3 camAt = cam->GetLookAt();
	D3DXVECTOR3 camDir =  cam->GetLookDir();
	PxVec3 pCamAt = PxVec3(camAt.x,camAt.y,camAt.z);
	PxVec3 pCamDir = PxVec3(camDir.x,camDir.y,camDir.z);
	PxVec3 pCamPos = PxVec3(camPos.x,camPos.y,camPos.z)+pCamDir*2;
	PxRaycastBuffer hit=  physics->RaycastSingle(pCamPos,pCamDir,10);
	return hit;
}
void Game::RightMouseClick()
{
	D3DXVECTOR3 camPos = cam->GetPosition();
	D3DXVECTOR3 camAt = cam->GetLookAt();
	D3DXVECTOR3 camDir =  cam->GetLookDir();
	PxVec3 pCamAt = PxVec3(camAt.x,camAt.y,camAt.z);
	PxVec3 pCamDir = PxVec3(camDir.x,camDir.y,camDir.z);
	PxVec3 pCamPos = PxVec3(camPos.x,camPos.y,camPos.z)+pCamDir*2;
	PxRaycastBuffer hit= physics->RaycastSingle(pCamPos,pCamDir,50);
	if(hit.block.actor != NULL)
	{
		PxRigidDynamic* p = hit.block.actor->isRigidDynamic();
		if(p != NULL && !p->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC))
		{
			p->addForce(PxVec3(0,50,0),PxForceMode::eVELOCITY_CHANGE);
		}
	}


}
void Game::Release()
{
	DestroyLevel();
	menu->Release();
	delete menu;
	physics->Release();
	if(gui != NULL)
	{
		gui->Release();
		delete gui;
	}
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
	physics->RemoveAllActors();
	for(unsigned int i=0;i<modelObjs.size();i++)
	{
		delete modelObjs.at(i)->obj;
	}
	for(unsigned int i=0; i< items.size(); i++)
	{
		items.at(i)->Release();
		delete items.at(i);
	}
	items.clear();
	modelObjs.clear(); 
	for(unsigned int i=0;i<enemies.size();i++)
	{
		enemies.at(i)->Release();
		delete enemies.at(i);
	}
	enemies.clear();
	for(unsigned int i=0;i<sprites.size();i++)
	{
		sprites.at(i)->obj->ReleaseResources();
		delete sprites.at(i)->obj;
	}
	sprites.clear();
	for(unsigned int i=0;i<doors.size();i++)
	{
		Door* d = doors.at(i);
		physics->RemoveActor(d->actor->isRigidDynamic());
		delete d;
	}
	doors.clear();
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
		else if(editorObjs.at(i).col != NULL)
		{
			editorObjs.at(i).col->Release();
			delete editorObjs.at(i).col;
		}
	}
	editorObjs.clear();

	string line;
	ifstream myfile ("resources/editorresources.txt");
	if (!myfile.good())
	{
		MessageBox(handleWindow,"Could Not Find editorresources.txt, EditorMode won't work", "ThempX()",MB_OK);
		cout << "Cannot find editorresources.txt" << endl;
		return;
	}
	if (myfile.is_open())
	{
		MessageBox(handleWindow,"Editor mode activated","SetUpEditorMode",MB_OK);
		EditorObj obj;
		obj.col = NULL;
		obj.obj2D = NULL;
		obj.obj3D = NULL;
		int animatedSpriteNr = 0;
		int staticSpriteNr = 0;
		int modelNr = 0;
		while ( getline (myfile,line) )
		{
			obj.col = NULL;
			obj.obj2D = NULL;
			obj.obj3D = NULL;
			string name, tag;
			int xRows,yRows;
			bool hasAnim = false;
			string check = "x";
			myfile >> name >> hasAnim >> xRows >> yRows >> tag;
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
				if(hasAnim == 0)
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
					Object2D* spriteObj = new Object2D(resources,_strdup(name.c_str()),GetCameraView(),xRows,yRows);
					spriteObj->handleWindow = handleWindow;
					std::ostringstream oss;
					oss << "AnimatedSprite" << animatedSpriteNr;
					spriteObj->objName = oss.str();
					obj.obj2D = spriteObj;
					spriteObj->tag = tag;
					editorObjs.push_back(obj);
					spriteObj->PlayAnimation("WalkForward");
					spriteObj->SetUVValues();
				}
			}
		}
		myfile.close();
		if(editorObjs.size() > 0)
		{
			currentEditorObj = &editorObjs.at(0);
		}
	}
	else
	{
		EditorMode = false;
		MessageBox(handleWindow,"No EditorResources.txt found", "SetUpEditorMode",MB_OK);
	}
	currentEditorObjIndex = 0;
	cout <<"array size is: "<< editorObjs.size() << endl;
}
void Game::CreateLevelFile()
{
	ofstream level("level.txt");
	if (level.is_open())
	{
		level << "name" << "\t"<< "objName" << "\t"<< "posx" << "\t"<< "posy" << "\t"<< "posz" << "\t"<< "scalex" << "\t"<< "scaley" << "\t"<< "scalez" << "\t"<< "rotx" << "\t"<< "roty" << "\t"<< "rotz" << "\t"<< "XAnimRows" << "\t"<< "YAnimRows" << "\t"<< "Tag" << "\t"<< "hasPhysics"<< "\t" <<	"isStatic" << "\t"<< "isKinematic"<< "\t" <<	"meshCollision"	<< "\t" << "FlipNormals"<< "\t"<<"doDraw" 	<< "\t"<< "hitBoxX" << "\t"<<	"hitBoxY" << "\t"<<	"hitBoxZ" << "\t"<<	"radius"<< "\t"<<	"capsuleHeight" <<"\n";
		for(unsigned int i = 0; i < modelObjs.size();i++)
		{
			Object3D* obj = modelObjs.at(i)->obj;
			level << obj->model.meshPath << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" <<0 << "\t" <<obj->tag << "\t" << (modelObjs.at(i)->actor != NULL)  << "\t" << (modelObjs.at(i)->actor != NULL && modelObjs.at(i)->actor->isRigidStatic() != NULL) << "\t" << (modelObjs.at(i)->actor != NULL && modelObjs.at(i)->actor->isRigidDynamic() != NULL && modelObjs.at(i)->actor->isRigidDynamic()->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) << "\t" <<0 << "\t" << 0 << "\t" << 1 << "\t" << 0 << "\t" <<0 << "\t" <<0 << "\t" <<0 << "\t" <<0 << "\n";
		}
		for(unsigned int i = 0; i < enemies.size();i++)
		{
			Object2D* obj = enemies.at(i)->GetObj();
			Enemy* d = enemies.at(i);
			level << obj->quad.textureName << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << obj->GetXRows() << "\t" << obj->GetYRows() << "\t" << obj->tag  << "\t" <<  (d->GetActor()!=NULL) << "\t" << 0 << "\t" <<1 << "\t" << 0 << "\t" << 1 << "\t" << 0 << "\t" << 0 << "\t" <<0 << "\t" <<0 << "\t" << d->GetCollisionRadius() << "\t" <<d->GetCollisionHeight() << "\n";
		}
		for(unsigned int i = 0; i < sprites.size();i++)
		{
			Object2D* obj = sprites.at(i)->obj;
			level << obj->quad.textureName << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << obj->GetXRows() << "\t" << obj->GetYRows() << "\t" << obj->tag  << "\t" <<  (sprites.at(i)->actor!=NULL) << "\t" << (sprites.at(i)->actor != NULL && sprites.at(i)->actor->isRigidStatic() != NULL) << "\t" <<(sprites.at(i)->actor != NULL && sprites.at(i)->actor->isRigidDynamic() != NULL && sprites.at(i)->actor->isRigidDynamic()->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) << "\t" << 0 << "\t" << 1 << "\t" << 0 << "\t" << 0 << "\t" <<0 << "\t" <<0 << "\t" << 1 << "\t" << 0 << "\n";
		}
		
		for(unsigned int i = 0; i < debugCubes.size();i++)
		{
			DebugCube* obj = debugCubes.at(i);
			level << "Collision"  << "\t" << "Collision"<< "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" << 0 << obj->tag <<"\n";
		}
		
		level.close();
		/*
		for(unsigned int i = 0; i < doors.size(); i++)
		{
			Door* d = doors.at(i);
			std::string toTag = "NoKey";
			if(d->requiresGreenKey)
			{
				toTag = "GreenKey";
			}
			if(d->requiresRedKey)
			{
				toTag = "RedKey";
			}
			level << d->obj->model.meshPath << "\t" << "Door" << "\t" << d->position.x << "\t" << d->position.y << "\t" << d->position.z << "\t" << d->obj->scaling.x << "\t" << d->obj->scaling.y << "\t" << d->obj->scaling.z << "\t" << d->obj->rotation.x << "\t" << d->obj->rotation.y << "\t" << d->obj->rotation.z << "\t" << 0 << "\t" << 0 << "\t" << toTag << "\t" <<  1 << "\t" << 0 << "\t" <<1 << "\t" << 0 << "\t" << 0 << "\t" << 1 << "\t" << d->obj->scaling.x << "\t" << d->obj->scaling.y << "\t" << d->obj->scaling.z << "\t" << 0 << "\t" << 0 << "\n";
		}
		*/
	}
	else
	{
		std::cout << "Unable to create the new level file" << std::endl;
	}
}
//speaks for itself
void Game::LoadLevel(char* txtPath)
{
	ifstream fin(txtPath);
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
			int XAnimRows;
			int YAnimRows;
			float posx,posy,posz,scalex,scaley,scalez,rotx,roty,rotz,hitBoxX,hitBoxY,hitBoxZ,radius,capHeight;
			bool hasPhysics,isStatic,isKinematic,doDraw,meshCollision,flipNormals;
			fin >> name >> objName >>posx >> posy >> posz >> scalex >> scaley >> scalez >> rotx >> roty >> rotz >> XAnimRows >> YAnimRows >> tag >> hasPhysics >> isStatic >> isKinematic >> meshCollision >> flipNormals>> doDraw >>hitBoxX >> hitBoxY >> hitBoxZ >> radius >> capHeight;
			if(tag.compare("BAKE") == 0)
			{
				physics->BakeTriangleMesh(resources->GetMeshFromVector(resources->GetMeshData(name)),PxVec3(posx,posy,posz),PxVec3(scalex,scaley,scalez),flipNormals);
			}
			else
			{
				if(name.at(name.size()-1) == check[0])
				{
					char* mName = _strdup(name.c_str());
					PhysicsData pData;
					pData.Nullify();
					Object3DData data = CreateObject3DData(mName,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,scalez),D3DXVECTOR3(rotx,roty,rotz),pData);
					if(!hasPhysics)
					{
						Create3DObject(hasPhysics,&data);
						Object3D* obj = modelObjs.at(modelObjs.size()-1)->obj;
						obj->objName = objName;
						obj->tag = tag;
					}
					else
					{
						if(meshCollision)
						{
							data.physics.cType = PhysicsData::Mesh;
							data.physics.flipNormals = flipNormals;
						}
						else if (radius != 0)
						{
							data.physics = CreatePhysicsData(radius,capHeight);
						}
						else
						{
							pData.cType = PhysicsData::BoundingBox;
							data.physics = CreatePhysicsData(PxVec3(hitBoxX,hitBoxY,hitBoxZ),isStatic,isKinematic);
						}
						Create3DObject(hasPhysics,&data);
						Object3D* obj = modelObjs.at(modelObjs.size()-1)->obj;
						obj->objName = objName;
						obj->tag = tag;
					}
					if(strcmp(_strdup(objName.c_str()),"Door") == 0 || tag.compare("RedDoor") == 0 || tag.compare("BlueDoor") == 0 || tag.compare("BreakableDoor") == 0)
					{
						Door* door = new Door(&Game::PlaySound,this);
						door->actor = physics->GetLastDynamic();
						door->actor->isRigidDynamic()->setRigidDynamicFlag(PxRigidBodyFlag::eKINEMATIC,true);
						door->actor->userData = door;
						door->actor->setName("Door");
						door->obj = GetLastObject3D();
						door->position = door->obj->position;
						doors.push_back(door);
						if(tag.compare("BreakableDoor") == 0)
						{
							door->actor->setName("BreakableDoor");
							door->breakable = true;
							door->health = 50;
						}

						if(strcmp(_strdup(tag.c_str()),"GreenKey") == 0)
						{
							door->requiresGreenKey = true;
						}
						else if(strcmp(_strdup(tag.c_str()),"RedKey") == 0)
						{
							door->requiresRedKey = true;
						}
					}
				}
				else
				{
					Object2D* obj;
					if(XAnimRows == 0 && YAnimRows == 0)
					{
						if(tag.compare("Health") == 0 || tag.compare("Armor") == 0)
						{
							Item* item = new Item(tag,&Game::ItemTrigger,this,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(2,5,2));
							item->obj2D = new Object2D(resources,_strdup(name.c_str()),&cam->GetView());
							item->obj2D->scaling = D3DXVECTOR3(scalex,scaley,scalex);
							items.push_back(item);
						}
						else
						{
							CreateStaticSprite(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),false,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2(0,0),CreatePhysicsData(radius,capHeight)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
							obj = sprites.at(sprites.size()-1)->obj;
							obj->handleWindow = handleWindow;
							obj->objName = objName;
							obj->tag = tag;
						}
					}
					else if(tag.compare("Enemy") == 0)
					{
						PlaceEnemy(_strdup(name.c_str()),D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),XAnimRows,YAnimRows,radius,capHeight);
					}
					else
					{
						CreateAnimatedSprite(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),true,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2((float)XAnimRows,(float)YAnimRows),CreatePhysicsData(radius,capHeight)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
						obj = sprites.at(sprites.size()-1)->obj;
						obj->handleWindow = handleWindow;
						obj->objName = objName;
						obj->tag = tag;	
					}
				}
			}
		}
		fin.close();
		CreateLevelFile();
	}
	physics->CreateUpperAndLowerLimits(PxVec3(0,-13.6f,0),PxVec3(0,-2.5f,0));
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