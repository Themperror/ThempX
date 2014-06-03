#include "Enemy.h"

Enemy::Enemy(ResourceManager* res, PhysXEngine* phys, int* hp, int* armor)
{
	Nullify();
	health = hp;
	armour = armor;
	resources = res;
	physics = phys;
	srand(timeGetTime());
}
void Enemy::Move(D3DXVECTOR3 dir, float dT)
{
	if(!physics->RaycastAny(actor->getGlobalPose().p-PxVec3(0,colCapsuleHeight/1.5f,0),PxVec3(0,-1,0).getNormalized(),3))
	{
		dir.y = -9.8f*dT*4;
		if(obj->position.y < -15)
		{
			obj->position.y = -9;
		}
	}
	obj->position += dir*dT;
	if(actor->isRigidDynamic() != NULL) actor->isRigidDynamic()->setKinematicTarget(PxTransform(PxVec3(obj->position.x,obj->position.y,obj->position.z)));
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
		PxRaycastBuffer hit = physics->RaycastSingle(PxVec3(bullet->obj->position.x,bullet->obj->position.y,bullet->obj->position.z),PxVec3(bullet->direction.x,bullet->direction.y,bullet->direction.z).getNormalized(),2);
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
				}
				else
				{
					resources->GetSoundHandler()->PlayWaveFile("SAMDIE1");
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
		pDir = pDir.getNormalized();
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
			CheckShooting(40,dT);
			cMState = MovementState::Forward; 
			anim = obj->GetAnimation("WalkForward");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkForward");
			}	
		}
		if(angle > -45 && angle < 0 && direction < 0)
		{
			CheckShooting(40,dT);
			cMState = MovementState::Left;
			anim = obj->GetAnimation("WalkLeft");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkLeft");
			}
		}
		else if(angle > 0 && angle < 45 && direction > 0)
		{
			CheckShooting(40,dT);
			cMState = MovementState::Right; 
			anim = obj->GetAnimation("WalkRight");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkRight");
			}
		}
		if(angle > -45 && angle < 0 && direction > 0)
		{
			CheckShooting(40,dT);
			cMState = MovementState::Right; 
			anim = obj->GetAnimation("WalkRight");
			if(anim.isFinished || cAnim->AnimationName.compare(anim.AnimationName) != 0)
			{
				obj->PlayAnimation("WalkRight");
			}
		}
		else if(angle > 0 && angle < 45 && direction < 0)
		{
			CheckShooting(40,dT);
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
		cState = Enemy::Moving;
		CheckFutureCollision();
		Move(moveDir,dT*movementSpeed);
	}
}
void Enemy::CheckShooting(float dist, float deltaTime)
{
	PxExtendedVec3 pPos = physics->player->getPosition();
	PxVec3 ori = actor->getGlobalPose().p;
	PxVec3 dir = -(ori - PxVec3(pPos.x,pPos.y,pPos.z));
	dir.normalize();

	PxRaycastHit* hit = physics->RaycastMultiple(ori+dir*2.8f,dir,dist,PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);
	if(hit[0].actor != NULL && hit[0].actor == physics->player->getActor())
	{
		lastTimeShot+=deltaTime;
		if(lastTimeShot > shootDelay)
		{
			obj->PlayAnimation("Shoot",true);
			moveDir *= 0.1f;
			if(obj->GetCurrentAnim()->doAction)
			{
				CreateBullet(ori+dir*2.5f,dir);
				currentMoveTime = 9999.0f;
				lastTimeShot = 0;
				resources->GetSoundHandler()->PlayWaveFile("piew");
				obj->GetCurrentAnim()->doAction = false;
			}
		}
	}
}
void Enemy::CheckFutureCollision()
{
	PxVec3 pos = actor->getGlobalPose().p;
	PxVec3 dirMove = PxVec3(moveDir.x,0,moveDir.z);
	dirMove.normalize();
	PxRaycastHit* hit = physics->RaycastMultiple(pos-PxVec3(0,1.5f,0),dirMove,3.5f,PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);
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
}