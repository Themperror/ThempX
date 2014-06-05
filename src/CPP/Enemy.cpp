#include "Enemy.h"

Enemy::Enemy(ResourceManager* res, PhysXEngine* phys, GUI* gu)
{
	Nullify();
	health = &gu->health;
	armour = &gu->armour;
	resources = res;
	physics = phys;
	gui = gu;
	srand(timeGetTime());
	srand(timeGetTime() + rand() + *health + *armour);
	dCube = new DebugCube(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),-D3DXVECTOR3(0.5f,0.5f,0.5f),D3DXVECTOR3(0.5f,0.5f,0.5f),res);
}
void Enemy::Move(D3DXVECTOR3 dir, float dT)
{
	PxRaycastHit* hit = physics->RaycastMultiple(actor->getGlobalPose().p,PxVec3(0,-1,0).getNormalized(),colCapsuleHeight+0.1f,NULL,PxQueryFlag::eSTATIC);
	if(hit[0].actor == NULL)
	{
		dir.y = -9.8f*dT*2.5f;
		if(obj->position.y-colCapsuleHeight < -13.6f)
		{
			obj->position.y = -13.6f+colCapsuleHeight;
		}
	}
	else if(hit[1].actor == NULL && hit[0].actor == actor)
	{
		dir.y = -9.8f*dT*2.5f;
		if(obj->position.y-colCapsuleHeight < -13.6f)
		{
			obj->position.y = -13.6f+colCapsuleHeight;
		}
	}
	obj->position += dir*dT;

	D3DXQUATERNION rotQuat;
	D3DXQuaternionRotationYawPitchRoll(&rotQuat,0,0,90*ToRadian);

	if(actor->isRigidDynamic() != NULL) actor->isRigidDynamic()->setKinematicTarget(PxTransform(PxVec3(obj->position.x,obj->position.y,obj->position.z),PxQuat(rotQuat.x,rotQuat.y,rotQuat.z,rotQuat.w)));
}
void Enemy::SetInfo(PxVec3 playerPosition, PxVec3 pRight)
{
	playerRight = pRight;
	playerPos = playerPosition;
}

void Enemy::CreateBullet(PxVec3 origin, PxVec3 dir)
{
	Bullet* b = new Bullet(new Object2D(resources,"resources/sprites/bullet.png",resources->GetCameraView()),D3DXVECTOR3(dir.x,dir.y,dir.z));	
	b->obj->position = D3DXVECTOR3(origin.x,origin.y,origin.z);
	b->direction = D3DXVECTOR3(dir.x,dir.y,dir.z);
	bullets.push_back(b);
	//physics->ThrowCube(origin,dir*50+PxVec3(0,5,0));
}
void Enemy::RenderBullets(D3DXMATRIX* camView)
{
	for(unsigned int i =0; i < bullets.size(); i++)
	{
		bullets.at(i)->obj->cameraView = camView;
		bullets.at(i)->obj->SetScale(0.5f,0.5f,0.5f);
		bullets.at(i)->obj->Draw();
	}
}
void Enemy::UpdateBullets(float deltaTimeF)
{
	for(unsigned int i = 0; i < bullets.size(); i++)
	{
		Bullet* bullet = bullets.at(i);
		bullet->bulletLife+=deltaTimeF;
		bullet->obj->position += bullet->direction*deltaTimeF*55;
		PxVec3 finalDir = PxVec3(bullet->direction.x,bullet->direction.y,bullet->direction.z);
		finalDir.normalize();
		PxRaycastBuffer hit = physics->RaycastSingle(PxVec3(bullet->obj->position.x,bullet->obj->position.y,bullet->obj->position.z),finalDir,2);
		if(hit.block.actor != NULL)
		{
			if(hit.block.actor == physics->player->getActor())
			{
				int damageToReceive = 15;
				if(*armour > 0)
				{
					*armour -= damageToReceive;
					if(*armour < 0)
					{
						damageToReceive = abs(*armour);
						*health -= damageToReceive;
						*armour = 0;
					}
				}
				else
				{
					*health -= damageToReceive;
				}
				if(*health > 0)
				{
					std::vector<std::string> names;
					names.push_back("SAMHIT1");
					names.push_back("SAMHIT2");
					names.push_back("SAMHIT3");
					resources->GetSoundHandler()->PlayRandom(&names);
					if(*health >= 75)
					{
						GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
						if(g->currentlyPlayingAnimation.compare("100") != 0)
						{
							gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"100");
						}
					}
					else if(*health < 75 && *health >= 50)
					{
						GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
						if(g->currentlyPlayingAnimation.compare("75") != 0)
						{
							gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"75");
						}
					}
					else if(*health < 50 && *health >= 25)
					{
						GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
						if(g->currentlyPlayingAnimation.compare("50") != 0)
						{
							gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"50");
						}
					}
					else if(*health < 25 && *health > 0)
					{
						GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
						if(g->currentlyPlayingAnimation.compare("25") != 0)
						{
							gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"25");
						}
					}
				}
				else
				{
					GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
					if(g->currentlyPlayingAnimation.compare("0") != 0)
					{
						gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"0");
					}
					resources->GetSoundHandler()->PlayWaveFile("SAMDIE1");
				}
				bullets.erase(bullets.begin()+i);
				bullet->obj->ReleaseResources();
				delete bullet->obj;
				delete bullet;
				return;
			}
			else if(hit.block.actor->isRigidStatic() != NULL)
			{
				bullets.erase(bullets.begin()+i);
				bullet->obj->ReleaseResources();
				delete bullet->obj;
				delete bullet;
				return;
			}

			
		}
		if(Vector3Distance(&D3DXVECTOR3(playerPos.x,playerPos.y,playerPos.z),&bullet->obj->position) > 300 && bullet->bulletLife > 2)
		{
			bullets.erase(bullets.begin()+i);
			bullet->obj->ReleaseResources();
			delete bullet->obj;
			delete bullet;
			return;
		}
	}
}
void Enemy::Update(float dT)
{
	UpdateBullets(dT);
	obj->Update(dT);
	if(!isDead)
	{
		currentMoveTime += dT;
		if(currentMoveTime > movementSwitchTime)
		{
			currentMoveTime = 0;
			float xspeed = rand()% (int)ceil(movementSpeed); //getal van 0 tot 10
			float xfSpeed = xspeed - (int)ceil(movementSpeed)/2; // getal van -5 tot 5
			float zspeed = rand()%(int)ceil(movementSpeed); //getal van 0 tot 10
			float zfSpeed = zspeed -(int)ceil( movementSpeed)/2; // getal van -5 tot 5
			
			moveDir = D3DXVECTOR3(xfSpeed, 0, zfSpeed);
		}
		PxVec3 pos = actor->getGlobalPose().p;
		PxVec3 pDir = (playerPos - pos);
		pDir.y = 0;
		pDir.normalize();
		PxVec3 target = PxVec3(moveDir.x,0,moveDir.z);
		float angle = 90 - (90 * pDir.dot(target.getNormalized())) - 90; //mapping range 1 to -1 into 0-180 if desirable
		playerRight.y = 0;
		float direction = playerRight.dot(target.getNormalized());
		if(direction < 0 && angle > -45 && angle < 45)
		{
			angle = -angle;		
		}
		Object2D::Animation* cAnim = obj->GetCurrentAnim();
		Object2D::Animation anim;
		if(angle < -45)
		{
			cMState = MovementState::Forward; 
			anim = obj->GetAnimation("WalkForward");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkForward");
			}	
		}
		if(angle > -45 && angle < 0 && direction < 0)
		{
			cMState = MovementState::Left;
			anim = obj->GetAnimation("WalkLeft");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkLeft");
			}
		}
		else if(angle > 0 && angle < 45 && direction > 0)
		{
			cMState = MovementState::Right; 
			anim = obj->GetAnimation("WalkRight");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkRight");
			}
		}
		if(angle > -45 && angle < 0 && direction > 0)
		{
			cMState = MovementState::Right; 
			anim = obj->GetAnimation("WalkRight");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkRight");
			}
		}
		else if(angle > 0 && angle < 45 && direction < 0)
		{
			cMState = MovementState::Left;
			anim = obj->GetAnimation("WalkLeft");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkLeft");
			}
		}
		if(angle > 45)
		{
			cMState = MovementState::Back;
			anim = obj->GetAnimation("WalkBack");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkBack");
			}		
		}
		CheckShooting(30,dT*1.2f);
		if(sawPlayer)
		{
			cState = Enemy::Moving;
			CheckFutureCollision();
			Move(moveDir,dT*movementSpeed);
		}
	}
}
void Enemy::CheckShooting(float dist, float deltaTime)
{

	PxExtendedVec3 tpPos = physics->player->getPosition();
	PxVec3 pPos = playerPos;
	PxVec3 ori = actor->getGlobalPose().p;
	PxVec3 dir = pPos - ori;
	dir.normalize();
	int numHits = 0;
	PxRaycastHit* hit = physics->RaycastMultiple(ori,dir,dist,&numHits, PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);
	for(unsigned int i = 1; i < numHits; i++)
	{
		if(hit[i-1].actor == actor && hit[i].actor == physics->player->getActor())
		{
			sawPlayer = true;
			lastTimeShot+=deltaTime;
			if(lastTimeShot > shootDelay)
			{
				obj->PlayAnimation("Shoot",true);
				moveDir *= 0.1f;
				if(obj->GetCurrentAnim()->doAction)
				{
					CreateBullet(ori+dir*2,dir);
					currentMoveTime = 9999.0f;
					lastTimeShot = 0;
					resources->GetSoundHandler()->PlayWaveFile("piew");
					obj->GetCurrentAnim()->doAction = false;
				}
			}
		}
	}
	/*
	if(hit[0].actor == physics->player->getActor())
	{
		
		
		resources->ClearConsole();
		std::cout << "I can see you and the distance was: "<< hit[0].distance << " Real distance: " << Vector3Distance(&ori,&pPos) <<std::endl;
		std::cout << "You were at: X: " << pPos.x << " Y: " << pPos.y << " Z: " << pPos.z << std::endl;
		std::cout << "I was at: X: "<< ori.x << " Y: "<< ori.y << " Z: " << ori.z << std::endl;
		std::cout << "Is direction normalized? : " << (dir.isNormalized() ? "yes" : "no")  << "   \nThe direction was: X: " <<dir.x << " Y: " << dir.y << " Z: "<<dir.z << std::endl;
	}
	else if(hit[0].actor == actor)
	{
		if( hit[1].actor == physics->player->getActor())
		{
			sawPlayer = true;
			lastTimeShot+=deltaTime;
			if(lastTimeShot > shootDelay)
			{
				obj->PlayAnimation("Shoot",true);
				moveDir *= 0.1f;
				if(obj->GetCurrentAnim()->doAction)
				{
					CreateBullet(ori+dir*2,dir);
					currentMoveTime = 9999.0f;
					lastTimeShot = 0;
					resources->GetSoundHandler()->PlayWaveFile("piew");
					obj->GetCurrentAnim()->doAction = false;
				}
			}
		}
	}*/
}
void Enemy::CheckFutureCollision()
{
	PxVec3 pos = actor->getGlobalPose().p;
	PxVec3 dirMove = PxVec3(moveDir.x,0,moveDir.z);
	while(dirMove.isZero())
	{
		float xspeed = rand()% (int)ceil(movementSpeed); //getal van 0 tot 10
		float xfSpeed = xspeed - (int)ceil(movementSpeed)/2; // getal van -5 tot 5
		float zspeed = rand()%(int)ceil(movementSpeed); //getal van 0 tot 10
		float zfSpeed = zspeed -(int)ceil( movementSpeed)/2; // getal van -5 tot 5
		dirMove = PxVec3(xfSpeed, 0, zfSpeed);
	}
	dirMove.normalize();
	PxRaycastHit* hit = physics->RaycastMultiple(pos-PxVec3(0,1.5f,0),dirMove,3.5f,NULL,PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);
	if(hit[0].actor != NULL && hit[0].distance > 0.1f)
	{
		currentMoveTime = 0;
		float xspeed = rand()% (int)ceil(movementSpeed); //getal van 0 tot 10
		float xfSpeed = xspeed - (int)ceil(movementSpeed)/2; // getal van -5 tot 5
		float zspeed = rand()%(int)ceil(movementSpeed); //getal van 0 tot 10
		float zfSpeed = zspeed -(int)ceil( movementSpeed)/2; // getal van -5 tot 5
		moveDir = D3DXVECTOR3(xfSpeed, 0, zfSpeed);
		CheckFutureCollision();
	}
	else if(hit[1].actor != NULL && hit[1].distance > 0.1f)
	{
		currentMoveTime = 0;
		float xspeed = rand()% (int)ceil(movementSpeed); //getal van 0 tot 10
		float xfSpeed = xspeed - (int)ceil(movementSpeed)/2; // getal van -5 tot 5
		float zspeed = rand()%(int)ceil(movementSpeed); //getal van 0 tot 10
		float zfSpeed = zspeed -(int)ceil( movementSpeed)/2; // getal van -5 tot 5
		moveDir = D3DXVECTOR3(xfSpeed, 0, zfSpeed);
		CheckFutureCollision();
	}
}
void Enemy::SetCState(Enemy::EnemyState s)
{
	prevState = cState;
	cState = s;
}
void Enemy::TakeDamage(float damage)
{
	enemyHP -= damage;
	if(enemyHP <= 0)
	{
		std::vector<std::string> names;
		names.push_back("MAN1DIE1");
		names.push_back("MAN1DIE2");
		resources->GetSoundHandler()->PlayRandom(&names);
		obj->PlayAnimation("Death",true, true);
		isDead = true;
	}
	else
	{
		std::vector<std::string> names;
		names.push_back("MAN1HIT1");
		names.push_back("MAN1HIT2");
		names.push_back("MAN1HIT3");
		resources->GetSoundHandler()->PlayRandom(&names);
		prevState = cState;
		cState = Damaged;
		switch(cMState)
		{
			case Forward:  obj->PlayAnimation("DamageF",true); break;
			case Left:  obj->PlayAnimation("DamageL",true); break;
			case Right:  obj->PlayAnimation("DamageR",true); break;
			case Back:  obj->PlayAnimation("DamageB",true); break;
		}
	}
}
void Enemy::SetPos(D3DXVECTOR3 pos)
{
	obj->position = D3DXVECTOR3(pos.x,pos.y,pos.z);
	actor->setGlobalPose(PxTransform(PxVec3(pos.x,pos.y,pos.z)));
}
void Enemy::Nullify()
{
	enemyHP = 0;
	enemyDamage = 0;
	cState = Moving;
	cMState = Forward;
	lastTimeShot = 0.5f;
	movementSpeed = 4;
	currentMoveTime = 9999.0f;
	movementSwitchTime = 1.0f;
	moveDir = D3DXVECTOR3(0.1f,0,0.1f);
	currentMoveTime = 0;
	originalPos = D3DXVECTOR3(0,0,0);
	shootDelay = 1.0f;
	obj = NULL;
	actor = NULL;
	isDead = false;
	colRadius= 0;
	colCapsuleHeight = 0;
	sawPlayer = false;
}