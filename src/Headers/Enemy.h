#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Object2D.h"
#include "ResourceManager.h"
#include "PhysXEngine.h"
#include "GUI.h"
#include "DebugCube.h"
class Enemy
{
public:
	enum EnemyState{Moving,Shooting,Dead,Idle,Damaged}; //remove later
	enum MovementState{Forward,Left,Right,Back};
	struct Bullet
	{
		Bullet(Object2D* texture, D3DXVECTOR3 dir)
		{
			obj = texture;
			direction = dir;
			hasHit = false;
			bulletLife = 0;
		}
		Object2D* obj;
		D3DXVECTOR3 direction;
		float bulletLife;
		bool hasHit;
	};

	Enemy(ResourceManager* res, PhysXEngine* phys, GUI* gu);
	void SetCState(EnemyState s);
	void TakeDamage(float damage);
	void SetInfo(PxVec3 playerPosition, PxVec3 pRight);
	void Update(float dT);
	void Nullify();
	void SetPos(D3DXVECTOR3 pos);
	void RenderBullets(D3DXMATRIX* camView);
	inline void Release()
	{
		if(obj != NULL)
		{
			obj->ReleaseResources();
			delete obj;
			obj = NULL;
		}
	}
	inline bool IsDead()
	{
		return isDead;
	}
	inline std::string LowCaseString(std::string string)
	{
		std::string newString = string;
		for(DWORD i = 0; i < string.size(); i++)
		{
			newString[i] = (char)tolower(string[i]);
		}
		return newString;
	}
	inline void SetHealth(float amount)
	{
		enemyHP = amount;
	}
	inline float GetDamage()
	{
		return enemyDamage;
	}
	inline float GetMoveSpeed()
	{
		return movementSpeed;
	}
	inline void SetAttackDelay(float amount)
	{
		shootDelay = amount;
	}
	inline void SetDamage(float amount)
	{
		enemyDamage = amount;
	}
	inline Object2D* GetObj()
	{
		return obj;
	}
	inline PxRigidActor* GetActor()
	{
		return actor;
	}
	inline void SetActorNull() //only use after cleaning up actor
	{
		actor = NULL;
	}
	inline void SetSpeed(float amount)
	{
		movementSpeed  = amount;
	}
	inline float GetCollisionRadius()
	{
		return colRadius;
	}
	inline float GetCollisionHeight()
	{
		return colCapsuleHeight;
	}
	inline float GetHealth()
	{
		return enemyHP;
	}
	inline void SetObj(Object2D* o)
	{
		obj = o;
	}
	inline void SetActor(PxRigidActor* a)
	{
		actor = a;
	}
	inline void SetColRadius(float rad)
	{
		colRadius = rad;
	}
	inline void SetColHeight(float height)
	{
		colCapsuleHeight = height;
	}




private:
	
	int* health; //pointer to GUI.health
	int* armour; //pointer to GUI.armour
	ResourceManager* resources;
	PhysXEngine* physics;
	PxVec3 playerRight;
	PxVec3 playerPos;
	std::vector<Bullet*> bullets;
	void CheckShooting(float dist, float deltaTime);
	void CheckFutureCollision();
	void Move(D3DXVECTOR3 dir, float dT);
	void UpdateBullets(float deltaTimeF);
	void CreateBullet(PxVec3 origin, PxVec3 dir);
	
	//Behaviour
	float enemyHP;
	float enemyDamage;
	bool isDead;
	bool sawPlayer;
	float shootDelay;
	float movementSpeed;

	//cooldown timers
	float movementSwitchTime;
	float currentMoveTime;
	float lastTimeShot;

	float colRadius;
	float colCapsuleHeight;

	//misc
	GUI* gui;
	Object2D* obj;
	PxRigidActor* actor;
	EnemyState cState;
	EnemyState prevState;
	MovementState cMState;
	D3DXVECTOR3 originalPos;
	D3DXVECTOR3 moveDir;
	D3DXVECTOR3 lookDirection;

	//temporary
	DebugCube* dCube;

	inline float Vector3Distance(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
	inline float Vector3Distance(PxVec3* a, PxVec3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
};

#endif