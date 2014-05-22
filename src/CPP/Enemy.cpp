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
		//physics->player->setPosition(PxExtendedVec3(bullet->obj->position.x,bullet->obj->position.y,bullet->obj->position.z));
		bullet->obj->position += bullet->direction*deltaTimeF*30;
		PxRaycastBuffer hit = physics->RaycastSingle(PxVec3(bullet->obj->position.x,bullet->obj->position.y,bullet->obj->position.z),PxVec3(bullet->direction.x,bullet->direction.y,bullet->direction.z),2);
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
	if(!IsDead)
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
		cState = Enemy::Moving;
		CheckFutureCollision();
		CheckShooting(40,dT);
		Move(moveDir,dT*movementSpeed);
	}
}
void Enemy::CheckShooting(float dist, float deltaTime)
{
	PxExtendedVec3 pPos = physics->player->getPosition();
	PxVec3 ori = actor->getGlobalPose().p;
	PxVec3 dir = (ori - PxVec3(pPos.x,pPos.y,pPos.z)).getNormalized();
	PxRaycastBuffer hit = physics->RaycastSingle(ori-dir*2,-dir,dist);
	if(hit.block.actor != NULL && hit.block.actor == physics->player->getActor())
	{
		lastTimeShot+=deltaTime;
		if(lastTimeShot > shootDelay)
		{
			obj->PlayAnimation("Shoot",true);
			if(obj->GetCurrentAnim()->doAction)
			{
				CreateBullet(ori-dir*2,-dir);
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
	PxRaycastHit* hit = physics->RaycastMultiple(pos-PxVec3(0,1,0),dirMove.getNormalized(),3.5f,PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);
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
	Health -= damage;
	if(Health <= 0)
	{
		obj->PlayAnimation("Death",true);
		PlayAnimAfterCurrent = false;
		IsDead = true;
	}
	else
	{
		prevState = cState;
		cState = Damaged;
		//obj->PlayAnimation("Damage");
		PlayAnimAfterCurrent = true;
	}
}
void Enemy::SetPos(D3DXVECTOR3 pos)
{
	obj->position = D3DXVECTOR3(pos.x,pos.y,pos.z);
	actor->setGlobalPose(PxTransform(PxVec3(pos.x,pos.y,pos.z)));
}
void Enemy::Nullify()
{
	Health = 0;
	Damage = 0;
	cState = EnemyState::Moving;
	cMState = MovementState::Forward;
	lastTimeShot = 2.0f;
	movementSpeed = 4;
	movementSwitchTime = 2;
	moveDir = D3DXVECTOR3(0,0,0);
	currentMoveTime = 0;
	originalPos = D3DXVECTOR3(0,0,0);
	shootDelay = 4.0f;
	obj = NULL;
	actor = NULL;
	IsDead = false;
	colRadius= 0;
	colCapsuleHeight = 0;
	PlayAnimAfterCurrent = true;
}