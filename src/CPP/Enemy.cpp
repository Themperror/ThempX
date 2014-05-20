#include "Enemy.h"

Enemy::Enemy(ResourceManager* res, PhysXEngine* phys)
{
	Nullify();
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
void Enemy::Update(float dT)
{
	if(!IsDead)
	{
		currentMoveTime += dT;
		if(currentMoveTime > movementSwitchTime)
		{
			currentMoveTime = 0;
			float xspeed = rand()%(int)movementSpeed; //getal van 0 tot 10
			float xfSpeed = xspeed - movementSpeed/2; // getal van -5 tot 5
			float zspeed = rand()%(int)movementSpeed; //getal van 0 tot 10
			float zfSpeed = zspeed - movementSpeed/2; // getal van -5 tot 5
			
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
		
		/*
		PxVec3 currentEPos = PxVec3(obj->position.x,obj->position.y,obj->position.z);
		PxExtendedVec3 currentPPos = physics->player->getPosition();
		PxVec3 lookDir = -PxVec3(currentEPos-PxVec3(currentPPos.x,currentPPos.y,currentPPos.z)).getNormalized();
		lookDirection = D3DXVECTOR3(lookDir.x,lookDir.y,lookDir.z);
		switch(cState)
		{
		case Enemy::Shooting:
				cState = Enemy::Shooting;
				if(cAnim->AnimationName.compare("Shoot") != 0)
				{
					obj->PlayAnimation("Shoot");
				}
				if(cAnim->AnimationName.compare("Shoot") == 0 && cAnim->doAction)
				{
					cAnim->doAction = false;
					cState = Enemy::Moving;
					cMState = Enemy::Forward;
					obj->PlayAnimation("WalkForward");
					soundHandler->PlayWaveFile("Piew");
					bullets.push_back(new Bullet(new Object2D(resources,"resources/sprites/bullet.png",&cam->GetView(),-D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.5f,0.5f)),lookDirection));
					Bullet* bullet = bullets.at(bullets.size()-1);
					bullet->obj->scaling = D3DXVECTOR3(1,1,1);
					bullet->obj->position = obj->position+lookDirection*3;
					lastTimeShot = 0;
				}
			break;
		case Enemy::Moving:
			if(hit.block.actor != NULL && hit.block.actor == physics->player->getActor())
			{
				lastTimeShot+=dT;
				if(lastTimeShot > shootDelay)
				{
					cState = Enemy::Shooting;
				}
			}
		case Enemy::Damaged:
			if(cAnim->doAction)
			{
				cState = prevState;
			}
			break;
		}*/
		Move(moveDir,dT*movementSpeed);
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
		float xspeed = rand()%(int)movementSpeed;
		float xfSpeed = xspeed - movementSpeed/2;
		float zspeed = rand()%(int)movementSpeed;
		float zfSpeed = zspeed - movementSpeed/2;
		moveDir = D3DXVECTOR3(xfSpeed, 0, zfSpeed);
		CheckFutureCollision();
	}
	else if(hit[1].actor != NULL && hit[1].distance > 0.1f)
	{
		currentMoveTime = 0;
		float xspeed = rand()%(int)movementSpeed;
		float xfSpeed = xspeed - movementSpeed/2;
		float zspeed = rand()%(int)movementSpeed;
		float zfSpeed = zspeed - movementSpeed/2;
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
		obj->PlayAnimation("Death");
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