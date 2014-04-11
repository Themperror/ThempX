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
	Initialize();
	LoadLevel();

	//particles.push_back(new Particle(resources,p_Device,"Resources/Particles/grid.png",&cam->GetView(),D3DXVECTOR3(4,-12.5f,0),200,500,1,3));
//	particles.at(0)->SetMovement(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,10,0));
	//particles.at(0)->SetParticleCount(2000,5000);
	//particles.at(0)->
	//particles.at(0)->Release();
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
	d.xRowsAnim = rows.x;
	d.yRowsAnim = rows.y;
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
	wireframe = false;
	scaleMultiplier = 3;
	EditorMode = false;
	player = new Player(cam,physics->player);
	dCube = new DebugCube(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),-D3DXVECTOR3(1,1,1),D3DXVECTOR3(1,1,1),resources);
	//Loading sounds
	//soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	
	soundHandler->LoadWaveFile("resources/sound/piew.wav","Piew",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/DoorDown.wav","DoorDown",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/DoorUp.wav","DoorUp"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/MAN1DIE1.wav","MAN1DIE1",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/MAN1DIE2.wav","MAN1DIE2",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/MAN1HIT1.wav","MAN1HIT1",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/MAN1HIT2.wav","MAN1HIT2",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/MAN1HIT3.wav","MAN1HIT3",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SAMHIT1.wav","SAMHIT1"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SAMHIT2.wav","SAMHIT2"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SAMHIT3.wav","SAMHIT3"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SAMDIE1.wav","SAMDIE1"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWIPE1.wav","SWIPE1"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWIPE2.wav","SWIPE2"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWIPE3.wav","SWIPE3"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWIPE4.wav","SWIPE4"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWIPE5.wav","SWIPE5"	,22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWORDHIT.wav","SWORDHIT",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/pickup.wav","pickup",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/deur.wav","deur",22050,16,1);
	
	soundHandler->LoadWaveFile("resources/sound/SWONWAL1.wav","SWONWAL1",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWONWAL2.wav","SWONWAL2",22050,16,1);
	soundHandler->LoadWaveFile("resources/sound/SWONWAL3.wav","SWONWAL3",22050,16,1);
	
	//soundHandler->PlayWaveFile("DoorDown");

	Item* KeyRed = new Item("Red Keycard",&Game::ItemTrigger,this,D3DXVECTOR3(0,-12.5f,0),D3DXVECTOR3(3,10,3));
	KeyRed->obj2D = new Object2D(resources,"Resources/Sprites/Redkey.png",&cam->GetView());
	KeyRed->obj2D->scaling = D3DXVECTOR3(0.3f,0.3f,0.3f);

	Item* KeyGreen = new Item("Green Keycard",&Game::ItemTrigger,this,D3DXVECTOR3(0,-12.5f,5),D3DXVECTOR3(3,10,3));
	KeyGreen->obj2D = new Object2D(resources,"Resources/Sprites/Greenkey.png",&cam->GetView());
	KeyGreen->obj2D->scaling = D3DXVECTOR3(0.3f,0.3f,0.3f);
	
	items.push_back(KeyRed);
	items.push_back(KeyGreen);

	//itemTest->tBoxVisualized = dCube;

	physics->player->setPosition(PxExtendedVec3(15,5.0f,0));
	//cam->SetPosition(5,7.0f,0);
	//resources->CreateTextObject("Arial","\n    THIS IS TESTING TEXT",12, 0, 0, 30, 20, 0xFFFF0000);
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
	
	if(item->name.compare("Red Keycard"))
	{
		player->hasRedKey = true;
	}
	else if(item->name.compare("Green Keycard"))
	{
		player->hasGreenKey = true;
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
	if(enemy->actor != NULL)
	{
		physics->RemoveActor(enemy->actor->isRigidDynamic());
		physics->RemoveActor(enemy->actor->isRigidStatic());
		enemy->actor = NULL;
	}
}
void Game::ReleaseEnemy(Enemy* enemy)
{
	for(unsigned int i = 0; i < spriteObjs.size();i++)
	{
		if(spriteObjs.at(i) == enemy)
		{
			spriteObjs.erase(spriteObjs.begin()+i);
		}
	}
	enemy->obj->ReleaseResources();
}
void Game::DoAI(float dT)
{
	for(int i = 0; i < spriteObjs.size();i++)
	{
		Enemy* e = spriteObjs.at(i);
		if(!e->IsDead)
		{
			
			Object2D::Animation* cAnim = e->obj->GetCurrentAnim();
			PxVec3 currentEPos = PxVec3(e->obj->position.x,e->obj->position.y,e->obj->position.z);
			PxExtendedVec3 currentPPos = physics->player->getPosition();
			PxVec3 lookDir = -PxVec3(currentEPos-PxVec3(currentPPos.x,currentPPos.y,currentPPos.z)).getNormalized();
			e->lookDirection = D3DXVECTOR3(lookDir.x,lookDir.y,lookDir.z);
			
			PxRaycastBuffer hit = physics->RaycastSingle(currentEPos+lookDir*3,lookDir,100);
		
			switch(e->cState)
			{
			case Enemy::Shooting:
					e->cState = Enemy::Shooting;
					if(cAnim->AnimationName.compare("Shoot") != 0)
					{
						e->obj->PlayAnimation("Shoot");
					}
					if(cAnim->AnimationName.compare("Shoot") == 0 && cAnim->doAction)
					{
						cAnim->doAction = false;
						e->cState = Enemy::Moving;
						e->cMState = Enemy::Forward;
						e->obj->PlayAnimation("WalkForward");
						soundHandler->PlayWaveFile("Piew");
						bullets.push_back(new Bullet(new Object2D(resources,"resources/sprites/bullet.png",&cam->GetView(),-D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.5f,0.5f)),e->lookDirection));
						Bullet* bullet = bullets.at(bullets.size()-1);
						bullet->obj->scaling = D3DXVECTOR3(1,1,1);
						bullet->obj->position = e->obj->position+e->lookDirection*3;
						e->lastTimeShot = 0;
					}
				break;
			case Enemy::Idle:
				break;
			case Enemy::Moving:
				if(hit.block.actor != NULL && hit.block.actor == physics->player->getActor())
				{
					e->lastTimeShot+=dT;
					if(e->lastTimeShot > e->shootDelay)
					{
						e->cState = Enemy::Shooting;
					}
				}
				if(e->originalPos == D3DXVECTOR3(0,0,0)) e->originalPos = e->obj->position;
				if(e->currentMoveTime > e->movementSwitchTime)
				{
					int xMove = rand()%100;
					int zMove = rand()%100;
					e->moveDir = D3DXVECTOR3((xMove-xMove/2) /10,0,(zMove-zMove/2) /10);
					e->currentMoveTime = 0;
				}
				if(Vector3Distance(&e->originalPos,&D3DXVECTOR3(e->obj->position+(e->moveDir*dT))) < 30)
				{
					e->cState = Enemy::Moving;
					if(cAnim->isFinished)
					{
						switch(e->cMState)
						{
						case Enemy::Forward:
							e->obj->PlayAnimation("WalkForward");
							break;
						case Enemy::Back:
							e->obj->PlayAnimation("WalkBack");
							break;
						case Enemy::Right:
							e->obj->PlayAnimation("WalkRight");
							break;
						case Enemy::Left:
							e->obj->PlayAnimation("WalkLeft");
							break;
						default:
							e->cMState = Enemy::Forward;
							e->obj->PlayAnimation("WalkForward");
							break;
						}
					}
					e->Move(e->moveDir,dT);
					e->currentMoveTime+=dT;
				}
				else
				{
					e->currentMoveTime = e->movementSwitchTime/2;
				}
				break;
			case Enemy::Damaged:
				if(cAnim->doAction)
				{
					e->cState = e->prevState;
				}
				break;
			default: 
				e->cState = Enemy::Moving;
				break;
			}
		}
	}
}
void Game::Update(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
	
	DoAI(deltaTimeF);
	for(unsigned int i =0 ;i<spriteObjs.size(); i++)
	{
		Enemy* enemy = spriteObjs.at(i);
		enemy->obj->Update(deltaTimeF);
		if(enemy->obj->hasAnimation)
		{
			if(enemy->IsDead)
			{
				RemoveEnemyCollision(enemy);
			}
			/*if(enemy->IsDead && enemy->obj->GetCurrentAnim()->isFinished)
			{
				//enemy->obj->PlayAnimation("DeadBody");//ReleaseEnemy(enemy);
			//	enemy->IsDead = false;
			}*/
		}
	}
	for(unsigned int i =0; i < doors.size(); i++)
	{
		doors.at(i)->MoveUp(deltaTime);
		doors.at(i)->MoveDown(deltaTime);
	}
	for(unsigned int i = 0; i < items.size();i++)
	{
		items.at(i)->CheckTrigger(&cam->GetPosition());
	}
	for(unsigned int i = 0; i < bullets.size(); i++)
	{
		Bullet* bullet = bullets.at(i);
		bullet->bulletLife+=deltaTimeF;
		bullet->obj->position += bullet->direction*deltaTimeF*50;
		PxRaycastBuffer hit = physics->RaycastSingle(PxVec3(bullet->obj->position.x,bullet->obj->position.y,bullet->obj->position.z),PxVec3(bullet->direction.x,bullet->direction.y,bullet->direction.z),2);
		if(hit.block.actor != NULL)
		{
			if(hit.block.actor == physics->player->getActor())
			{
				int damageToReceive = 15;
				if(gui->armour > 0)
				{
					gui->armour -= damageToReceive;
					if(gui->armour < 0)
					{
						damageToReceive = abs(gui->armour);
						gui->health -= damageToReceive;
						gui->armour = 0;
					}
				}
				else
				{
					gui->health -= damageToReceive;
				}
				if(gui->health > 0)
				{
					
					std::vector<std::string> names;
					names.push_back("SAMHIT1");
					names.push_back("SAMHIT2");
					names.push_back("SAMHIT3");
					soundHandler->PlayRandom(names,50);
				}
				else
				{
					soundHandler->PlayWaveFile("SAMDIE1");
				}
				bullets.erase(bullets.begin()+i);
				bullet->obj->ReleaseResources();
				delete bullet->obj;
				delete bullet;
				return;
			}

			bullets.erase(bullets.begin()+i);
			bullet->obj->ReleaseResources();
			delete bullet->obj;
			delete bullet;
			return;
		}
		if(Vector3Distance(cam->GetPosition(),bullet->obj->position) > 300 && bullet->bulletLife > 2)
		{
			bullets.erase(bullets.begin()+i);
			bullet->obj->ReleaseResources();
			delete bullet->obj;
			delete bullet;
			return;
		}
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
	PxVec3 moveDir = DoInput(deltaTimeF);
	player->Move(moveDir,deltaTimeF);
	HandlePlayerCollisions(moveDir);
	cam->Update(deltaTimeF,inputHandler->GetMousePosX(),inputHandler->GetMousePosY());
	physx::PxReal elapsed = 1.0f/60.0f;
	physics->gScene->simulate(elapsed);
	while(!physics->gScene->fetchResults())
	{
		
	}
	gui->Update(deltaTimeF);
}
void Game::HandlePlayerCollisions(PxVec3 moveDir)
{
	
	/*if(physics->player->getPosition().y > 20 || physics->player->getPosition().y < -20)
	{
		physics->player->setPosition(PxExtendedVec3(10,-5,0));
	}*/
	PxControllerState state;
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
	}
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
	for(unsigned int i = 0; i < spriteObjs.size();i++)
	{
		spriteObjs.at(i)->obj->cameraView = camView;
		spriteObjs.at(i)->obj->Draw();
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
	physics->DrawBoxes();
	
	p_Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	p_Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	for(unsigned int i = 0; i < modelObjs.size();i++)
	{
		modelObjs.at(i)->obj->DrawModel();	
	}
	for(unsigned int i = 0; i < debugCubes.size();i++)
	{
		debugCubes.at(i)->Draw();
	}
	for(unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets.at(i)->obj->cameraView = camView;
		bullets.at(i)->obj->Draw();
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
		physics->DeleteLastStatic();
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
	//std::cout << "   x: "<<physics->player->getFootPosition().x <<"   y: " <<physics->player->getFootPosition().y << "   z: "<<physics->player->getFootPosition().z << std::endl;
	if(KeyPressed(DIK_LCONTROL) && KeyPressed(DIK_Z) == 2)
	{
		UndoEditorAction();
	}
	
	if(KeyPressed(DIK_CAPSLOCK) == 2)
	{
		data->lockCursor = !data->lockCursor;
	}
	if(KeyPressed(DIK_COMMA) == 2)
	{
		data->changeDisplay = true;
		data->windowed = false;
		data->devmodeIndex = resources->GetDevMode(800,600);
		return PxVec3(0,0,0);
	}
	if(KeyPressed(DIK_M) == 2)
	{
		data->changeDisplay = true;
		data->windowed = true;
		data->renderSizeX = 800;
		data->renderSizeY = 600;
		return PxVec3(0,0,0);
	}
	
	int speed = 60;
	if(KeyPressed(DIK_LSHIFT))
	{
		speed = 90;
	}
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
			if(currentEditorObj->obj2D != NULL)
			{
				currentEditorObj->obj2D->PlayAnimation("WalkForward");
			}
		}
		if(KeyPressed(DIK_9) == 2)
		{
			currentEditorObjIndex++;
			cout << currentEditorObjIndex << endl;
			cout << "pressed 9 " << endl;
			if(currentEditorObjIndex >= editorObjs.size())
			{
				currentEditorObjIndex = 0;
			}
			currentEditorObj = &editorObjs.at(currentEditorObjIndex);
		
			if(currentEditorObj->obj2D != NULL && currentEditorObj->obj2D->hasAnimation)
			{
				currentEditorObj->obj2D->PlayAnimation("WalkForward");
			}
		}
		editorObjectDistance+=inputHandler->GetMouseScroll()*dT;
	}
	if(KeyPressed(DIK_F))
	{
		D3DXVECTOR3 camPos = cam->GetPosition()+cam->GetLookDir()*2;
		physics->ThrowCube(PxVec3(camPos.x,camPos.y,camPos.z),PxVec3(cam->GetLookDir().x*60,cam->GetLookDir().y*60,cam->GetLookDir().z*60));
		lastAction.push_back(ThrowCube);
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
	if(KeyPressed(DIK_R) == 2)
	{
		DestroyLevel();	 //This will destroy the current level 
		LoadLevel();	//load the level in level.txt (if edited, the changes will reflect on the world)
	}
	if(inputHandler->KeyPressed(DIK_ESCAPE))
	{
		data->loop = false;
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
			model->obj = obj;
			PxRigidActor* actor = NULL;
			if(data->physics.cType == PhysicsData::PhysicsType::Mesh)
			{
				physics->BakeMesh(obj->model.mesh,PxVec3(obj->scaling.x,obj->scaling.y,obj->scaling.z),true); //3rd arg "true" must be flipnormals, that is linked data from lvl.txt
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
bool Game::CreateAnimated2DObject(bool hasPhysics, Object2DData* data)
{
	Object2D* obj;
	Enemy* enemy = new Enemy();
	enemy->Health = 60;
	enemy->Damage = 10;
	if(hasPhysics)
	{
		if(data->hasAnimation)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10, data->physics.isStatic, data->physics.isKinematic);
			enemy->actor = actor;
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
	enemy->obj = obj;
	spriteObjs.push_back(enemy);
	return true;
}
bool Game::CreateStatic2DObject(bool hasPhysics, Object2DData* data)
{
	if(data != NULL)
	{
		Object2D* obj;
		Enemy* enemy = new Enemy();
		enemy->Health = 60;
		enemy->Damage = 10;

		if(hasPhysics)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10,data->physics.isStatic,data->physics.isKinematic);
			enemy->actor = actor;
			PhysicsUserData* uData = new PhysicsUserData();
			uData->Nullify();
			uData->related2D= obj;
			uData->enemy = enemy;
			actor->userData = uData;
		}
		else
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			//physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10, false, true);
		}
		enemy->obj = obj;
		spriteObjs.push_back(enemy);
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
				door->activated = true;
				
			}
		}
	}
}
void Game::PlaceEnemy(std::string textureString, D3DXVECTOR3 position, D3DXVECTOR3 scaling, float xRows,float yRows, float colRadius, float colHeight)
{
	CreateAnimated2DObject(true,&CreateObject2DData(_strdup(textureString.c_str()),true,position,scaling,D3DXVECTOR2(xRows,yRows),CreatePhysicsData(colRadius,colHeight)));
	Enemy* e = spriteObjs.at(spriteObjs.size()-1);
	Object2D* obj = spriteObjs.at(spriteObjs.size()-1)->obj;
	obj->objName = "Enemy";
	obj->PlayAnimation("WalkForward");
	PxRaycastBuffer hit = physics->RaycastSingle(PxVec3(obj->position.x,obj->position.y-(colHeight+0.5f),obj->position.z),PxVec3(0,-1,0),100);
	obj->position = D3DXVECTOR3(obj->position.x,hit.block.position.y+colHeight+colHeight/2,obj->position.z);
	e->actor->setGlobalPose(PxTransform(PxVec3(obj->position.x,obj->position.y,obj->position.z)));

}
void Game::LeftMouseClick()
{
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			if(!currentEditorObj->obj2D->hasAnimation)
			{
				CreateStatic2DObject(true,&CreateObject2DData(currentEditorObj->obj2D->quad.textureName,true,cam->GetPosition()+cam->GetLookDir()*editorObjectDistance,currentEditorObj->obj2D->scaling,D3DXVECTOR2(0,0),CreatePhysicsData(1,3)));
				Object2D* obj = spriteObjs.at(spriteObjs.size()-1)->obj;
				obj->objName = currentEditorObj->obj2D->objName;
			}
			else
			{
				PlaceEnemy(currentEditorObj->obj2D->quad.textureName,cam->GetPosition()+cam->GetLookDir()*editorObjectDistance,currentEditorObj->obj2D->scaling,currentEditorObj->obj2D->GetXRows(),currentEditorObj->obj2D->GetYRows(),1,3);
			}
		}
		else if(currentEditorObj->obj3D != NULL)
		{
			Door* door = new Door(&Game::PlaySound,this);
			
			soundHandler->PlayWaveFile("deur");
			D3DXVECTOR3 pos = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			Create3DObject(true,&CreateObject3DData(currentEditorObj->obj3D->model.meshPath,pos,currentEditorObj->obj3D->scaling,currentEditorObj->obj3D->rotation,CreatePhysicsData(PxVec3(currentEditorObj->obj3D->scaling.x/2,currentEditorObj->obj3D->scaling.y/2,currentEditorObj->obj3D->scaling.z/2),false,true,0)));
			//Object3D* obj = new Object3D(resources,currentEditorObj->obj3D->model.meshPath);
			Object3D* obj = GetLastObject3D();
			obj->objName = currentEditorObj->obj3D->objName;
			door->obj = obj;
			door->position = pos;
			door->actor = physics->GetLastDynamic();
			door->actor->userData = door;
			door->actor->setName(_strdup(currentEditorObj->obj3D->tag.c_str()));
			doors.push_back(door);
			lastAction.push_back(ModelWithCollision);
		}
		else if(currentEditorObj->col != NULL)
		{
			D3DXVECTOR3 pos = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			physics->CreateCube(PxVec3(pos.x,pos.y,pos.z),PxVec3(currentEditorObj->col->rotation.x,currentEditorObj->col->rotation.y,currentEditorObj->col->rotation.z),PxVec3(currentEditorObj->col->scaling.x,currentEditorObj->col->scaling.y,currentEditorObj->col->scaling.z),50,true);
			lastAction.push_back(Static);
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
		soundHandler->PlayRandom(names,50);

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
					data->enemy->cState = Enemy::Damaged;
					if(data->related2D != NULL && !data->enemy->IsDead)
					{
						std::cout << data->related2D->quad.textureName << std::endl;
						data->enemy->TakeDamage(10);
						soundHandler->PlayWaveFile("SWORDHIT",100);
						if(data->enemy->Health <= 0)
						{
							std::vector<std::string> names;
							names.push_back("MAN1DIE1");
							names.push_back("MAN1DIE2");
							soundHandler->PlayRandom(names,50);
						}
						else
						{
							std::vector<std::string> names;
							names.push_back("MAN1HIT1");
							names.push_back("MAN1HIT2");
							names.push_back("MAN1HIT3");
							soundHandler->PlayRandom(names,50);
						}
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
	PxRaycastBuffer hit=  physics->RaycastSingle(pCamPos,pCamDir,50);
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
void Game::ReleaseAll()
{
	DestroyLevel();
	for(unsigned int i=0;i<modelObjs.size();i++)
	{
		delete modelObjs.at(i)->obj;
	}
	for(unsigned int i=0;i<spriteObjs.size();i++)
	{
		spriteObjs.at(i)->obj->ReleaseResources();  
		delete spriteObjs.at(i)->obj;
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
		delete modelObjs.at(i)->obj;
	}
	modelObjs.clear(); 
	for(unsigned int i=0;i<spriteObjs.size();i++)
	{
		spriteObjs.at(i)->obj->ReleaseResources();
		delete spriteObjs.at(i)->obj;
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
		obj.col = new DebugCube(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(-1,-1,-1),D3DXVECTOR3(1,1,1),resources);
		obj.col->doRender = true;
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
			float xRows,yRows;
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
		level << "name" << "\t"<< "objName" << "\t"<< "posx" << "\t"<< "posy" << "\t"<< "posz" << "\t"<< "scalex" << "\t"<< "scaley" << "\t"<< "scalez" << "\t"<< "rotx" << "\t"<< "roty" << "\t"<< "rotz" << "\t"<< "XAnimRows" << "\t"<< "YAnimRows" << "Tag" << "\n";
		for(unsigned int i = 0; i < modelObjs.size();i++)
		{
			Object3D* obj = modelObjs.at(i)->obj;
			level << obj->model.meshPath << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" <<0 <<obj->tag << "\n";
		}
		for(unsigned int i = 0; i < spriteObjs.size();i++)
		{
			Object2D* obj = spriteObjs.at(i)->obj;
			level << obj->quad.textureName << "\t" << obj->objName << "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << obj->GetXRows() << "\t" << obj->GetYRows() <<obj->tag << "\n";
		}
		for(unsigned int i = 0; i < debugCubes.size();i++)
		{
			DebugCube* obj = debugCubes.at(i);
			level << "Collision"  << "\t" << "Collision"<< "\t" << obj->position.x << "\t" << obj->position.y << "\t" << obj->position.z << "\t" << obj->scaling.x << "\t" << obj->scaling.y << "\t" << obj->scaling.z << "\t" << obj->rotation.x << "\t" << obj->rotation.y << "\t" << obj->rotation.z << "\t" << 0 << "\t" << 0 << obj->tag <<"\n";
		}
		level.close();
	}
	else
	{
		std::cout << "Unable to create the new level file" << std::endl;
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
			float posx,posy,posz,scalex,scaley,scalez,rotx,roty,rotz,hitBoxX,hitBoxY,hitBoxZ,radius,capHeight;
			bool hasPhysics,isStatic,doDraw,meshCollision;
			fin >> name >> objName >>posx >> posy >> posz >> scalex >> scaley >> scalez >> rotx >> roty >> rotz >> XAnimRows >> YAnimRows >> tag >> hasPhysics >> isStatic >> meshCollision >> doDraw >>hitBoxX >> hitBoxY >> hitBoxZ >> radius >> capHeight;

			if(name.at(name.size()-1) == check[0])
			{
				char* mName = _strdup(name.c_str());
				PhysicsData pData;
				pData.Nullify();
				Object3DData data = CreateObject3DData(mName,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,scalez),D3DXVECTOR3(rotx,roty,rotz),pData);
				//SPEEngine::RigidData pData = CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(scalex,scaley,scalez),SPEVector(0,0,0),SPEVector(0,0,0));
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
						data.physics.cType = PhysicsData::PhysicsType::Mesh;
					}
					else if (radius != 0)
					{
						data.physics = CreatePhysicsData(radius,capHeight);
					}
					else
					{
						data.physics = CreatePhysicsData(PxVec3(hitBoxX,hitBoxY,hitBoxZ));
					}
					Create3DObject(hasPhysics,&data);
					Object3D* obj = modelObjs.at(modelObjs.size()-1)->obj;
					obj->objName = objName;
					obj->tag = tag;
				}
			}
			else
			{
				Object2D* obj;
				if(XAnimRows == 0 && YAnimRows == 0)
				{
					CreateStatic2DObject(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),false,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2(0,0),CreatePhysicsData(radius,capHeight)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
					obj = spriteObjs.at(spriteObjs.size()-1)->obj;
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->tag = tag;
				}
				else if(tag.compare("Enemy") == 0)
				{
					PlaceEnemy(_strdup(name.c_str()),D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),XAnimRows,YAnimRows,radius,capHeight);
				}
				else
				{
					CreateAnimated2DObject(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),true,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2(XAnimRows,YAnimRows),CreatePhysicsData(radius,capHeight)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
					obj = spriteObjs.at(spriteObjs.size()-1)->obj;
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