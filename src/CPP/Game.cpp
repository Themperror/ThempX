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

	//particles.push_back(new Particle(resources,p_Device,"Resources/Particles/Lightning.png",&camera.m_View,D3DXVECTOR3(0,10,10),200,500,1,3));
	//particles.at(0)->SetMovement(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,3,0));
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
	d.position = pos;
	d.scale = scale;
	d.rotation = rot;
	return d;
}
void Game::Initialize()
{
	currentEditorObjIndex = 0;
	qualityLevel = 0;
	editorObjectDistance = 10;
	wireframe = false;
	scaleMultiplier = 3;
	EditorMode = false;
	soundHandler->LoadWaveFile("test.wav","test",11025,8,1);
	
	keys.resize(256,0);
	resources->CreateTextObject("Arial","\n    THIS IS TESTING TEXT",12, 0, 0, 30, 20, 0xFFFF0000);
}
void Game::ReleaseEnemy(Enemy* enemy)
{
	physics->RemoveActor(enemy->actor->isRigidDynamic());
	physics->RemoveActor(enemy->actor->isRigidStatic());
	PxShape* shapes;
	enemy->actor->getShapes(&shapes,sizeof(PxShape));
	for(unsigned int x = 0; x < enemy->actor->getNbShapes(); x++)
	{
		enemy->actor->detachShape(shapes[x]);
	}
	enemy->actor->release();
	enemy->obj->ReleaseResources();
}
void Game::Update(double deltaTime)
{
	float deltaTimeF = (float)deltaTime;
	for(unsigned int i =0 ;i<spriteObjs.size(); i++)
	{
		spriteObjs.at(i)->obj->Update(deltaTimeF);
		if(spriteObjs.at(i)->obj->hasAnimation)
		{
			if(spriteObjs.at(i)->IsDead && spriteObjs.at(i)->obj->GetCurrentAnim()->isFinished)
			{
				ReleaseEnemy(spriteObjs.at(i));
				spriteObjs.erase(spriteObjs.begin()+i);
			}
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
	HandlePlayerCollisions(DoInput(deltaTimeF));
	PxExtendedVec3 PxPlayerPos = physics->player->getPosition()+PxExtendedVec3(0,2,0);
	cam->SetPosition(PxPlayerPos.x,PxPlayerPos.y,PxPlayerPos.z);
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
		spriteObjs.at(i)->obj->cameraView = &cam->GetView();
		spriteObjs.at(i)->obj->Draw();
		if(spriteObjs.at(i)->PlayAnimAfterCurrent)
		{
			spriteObjs.at(i)->obj->CheckPlayingAnimation("Test");
		}
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
			currentEditorObj->obj2D->position = cam->GetPosition()+cam->GetLookDir()*editorObjectDistance;
			currentEditorObj->obj2D->Update(0.0166f);
			currentEditorObj->obj2D->cameraView = &cam->GetView();
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
	gui->Render();
	cam->RenderCamera(THEMPX_CAM_PERSPECTIVE);
	resources->DrawAllText();
}
void Game::ReloadGUI()
{
	gui->ReloadGUI();
}
//Input Handling (for testing, this needs to be in game.cpp when a game is created)
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
	if(KeyPressed(DIK_Q))
	{
		moveDir+= PxVec3(0,dT*speed,0);
	}
	if(KeyPressed(DIK_E))
	{
		moveDir+= PxVec3(0,-dT*speed,0);
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
	if(KeyPressed(DIK_SPACE) == 2)
	{
	
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
				currentEditorObj->obj2D->PlayAnimation("Test");
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
				currentEditorObj->obj2D->PlayAnimation("Test");
			}
		}
		editorObjectDistance+=inputHandler->GetMouseScroll()*dT;
	}
	if(KeyPressed(DIK_F))
	{
		D3DXVECTOR3 camPos = cam->GetPosition()+cam->GetLookDir()*2;
		physics->ThrowCube(PxVec3(camPos.x,camPos.y,camPos.z),PxVec3(cam->GetLookDir().x*80,cam->GetLookDir().y*80,cam->GetLookDir().z*80));
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
	physics->player->move(moveDir,0.001f,dT,NULL,NULL);
	return moveDir;
}
bool Game::Create3DObject(bool hasPhysics, Object3DData* data)
{
	if(data != NULL)
	{
		if(hasPhysics)
		{
			Object3D* obj = new Object3D(resources,data->filePath);
			if(data->physics.cType == PhysicsData::PhysicsType::Mesh)
			{
				physics->BakeMesh(obj->model.mesh,PxVec3(obj->scaling.x,obj->scaling.y,obj->scaling.z),true); //3e arg "true" moet flipnormals worden, die gelinkt is aan de data uit lvl.txt
			}
			else
			{
				//box collision toevoegen.
			}
			obj->position = data->position;
			obj->scaling = data->scale;
			obj->rotation = data->rotation;
			PhysicsUserData uData;
			uData.Nullify();
			uData.related3D= obj;
			modelObjs.push_back(obj);
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
	Enemy* enemy = new Enemy();
	enemy->Health = 20;
	enemy->Damage = 10;
	if(hasPhysics)
	{
		if(data->hasAnimation)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
			obj->position = data->position;
			obj->hasAnimation = true;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10, false, true);
			enemy->actor = actor;
			PhysicsUserData* uData = new PhysicsUserData();
			uData->Nullify();
			uData->related2D = obj;
			uData->enemy = enemy;
			actor->userData = uData;
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
			obj = new Object2D(resources,data->filePath,GetCameraView(),data->xRowsAnim,data->yRowsAnim);
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
		enemy->Health = 20;
		enemy->Damage = 10;

		if(hasPhysics)
		{
			obj = new Object2D(resources,data->filePath,GetCameraView());
			obj->position = data->position;
			obj->scaling = data->scale;
			PxRigidActor* actor = physics->CreateSphereCapsule(data->physics.radius, data->physics.capsuleHeight ,PxVec3(data->position.x,data->position.y,data->position.z), 10, false, true);
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
//Left Mouse click function (to test things with), will be removed for engine use, as this is a gameplay feature
void Game::LeftMouseClick()
{
	if(EditorMode)
	{
		if(currentEditorObj->obj2D != NULL)
		{
			if(!currentEditorObj->obj2D->hasAnimation)
			{
				CreateStatic2DObject(true,&CreateObject2DData(currentEditorObj->obj2D->quad.textureName,true,cam->GetPosition()+cam->GetLookDir()*editorObjectDistance,currentEditorObj->obj2D->scaling,D3DXVECTOR2(0,0),CreatePhysicsData(1,5)));
				Object2D* obj = spriteObjs.at(spriteObjs.size()-1)->obj;
				obj->objName = currentEditorObj->obj2D->objName;
			}
			else
			{
				CreateAnimated2DObject(true,&CreateObject2DData(currentEditorObj->obj2D->quad.textureName,true,cam->GetPosition()+cam->GetLookDir()*editorObjectDistance,currentEditorObj->obj2D->scaling,D3DXVECTOR2(currentEditorObj->obj2D->GetXRows(),currentEditorObj->obj2D->GetYRows()),CreatePhysicsData(1,5)));
				Object2D* obj = spriteObjs.at(spriteObjs.size()-1)->obj;
				obj->objName = currentEditorObj->obj2D->objName;
				obj->PlayAnimation("Test");
			}
		}
		else if(currentEditorObj->obj3D != NULL)
		{
			Object3D* obj = new Object3D(resources,currentEditorObj->obj3D->model.meshPath);
			obj->objName = currentEditorObj->obj3D->objName;
			obj->SetPosition(cam->GetPosition()+cam->GetLookDir()*editorObjectDistance);
			obj->SetScale(currentEditorObj->obj2D->scaling);
			modelObjs.push_back(obj);
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
			gui->guiObjs.at(0).mirrored = !gui->guiObjs.at(0).mirrored;
			if(!gui->guiObjs.at(0).mirrored)
			{
				gui->PlayAnimation(&gui->guiObjs.at(0),"Attack");
			}
			else
			{
				gui->PlayAnimation(&gui->guiObjs.at(0),"ReversedAttack");
			}
		}
		PxRaycastBuffer* hit = RayFromPlayer();
		if(hit->block.actor != NULL)
		{
			//std::cout << "Raycast Single hit" << std::endl;
			PxRigidDynamic* p = hit->block.actor->isRigidDynamic();
			if(p != NULL && !p->getRigidDynamicFlags().isSet(PxRigidBodyFlag::eKINEMATIC))
			{
				//p->addForce(PxVec3(0,50,0),PxForceMode::eVELOCITY_CHANGE);
			}
			if(p != NULL && p->userData != NULL)
			{
				PhysicsUserData* data = (PhysicsUserData*)p->userData;
				if(data != NULL)
				{
					if(data->related2D != NULL && !data->enemy->IsDead)
					{
						std::cout << data->related2D->quad.textureName << std::endl;
						data->enemy->Health -= 10;
						data->related2D->PlayAnimation("Damage");
						data->enemy->PlayAnimAfterCurrent = true;
						if(data->enemy->Health <= 0)
						{
							data->enemy->obj->PlayAnimation("Death");
							data->enemy->PlayAnimAfterCurrent = false;
							data->enemy->IsDead = true;
						}
					}
				}

				//std::cout << "Number of the model =" << (unsigned int)p->userData << std::endl;
			}
		}
	}
}
PxRaycastBuffer* Game::RayFromPlayer()
{
	D3DXVECTOR3 camPos = cam->GetPosition();
	D3DXVECTOR3 camAt = cam->GetLookAt();
	D3DXVECTOR3 camDir =  cam->GetLookDir();
	PxVec3 pCamAt = PxVec3(camAt.x,camAt.y,camAt.z);
	PxVec3 pCamDir = PxVec3(camDir.x,camDir.y,camDir.z);
	PxVec3 pCamPos = PxVec3(camPos.x,camPos.y,camPos.z)+pCamDir*2;
	PxRaycastBuffer hit= physics->RaycastSingle(pCamPos,pCamDir,50);
	return &hit;
	
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
		delete modelObjs.at(i);
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
		delete modelObjs.at(i);
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
			Object3D* obj = modelObjs.at(i);
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
					Object3D* obj = modelObjs.at(modelObjs.size()-1);
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
					CreateStatic2DObject(hasPhysics,&CreateObject2DData(_strdup(name.c_str()),false,D3DXVECTOR3(posx,posy,posz),D3DXVECTOR3(scalex,scaley,1),D3DXVECTOR2(0,0),CreatePhysicsData(radius,capHeight)));//,&CreatePhysicsData(doDraw,isStatic,1,1,SPEVector(posx,posy,posz),SPEVector(hitBoxX,hitBoxY,hitBoxZ),SPEVector(0,0,0),SPEVector(0,0,0)));
					obj = spriteObjs.at(spriteObjs.size()-1)->obj;
					obj->handleWindow = handleWindow;
					obj->objName = objName;
					obj->tag = tag;
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