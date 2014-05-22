#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Object2D.h"
#include "ResourceManager.h"
#include "PhysXEngine.h"

class Enemy
{
public:
	enum EnemyState{Moving,Shooting,Dead,Idle,Damaged};
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

	Enemy(ResourceManager* res, PhysXEngine* phys, int* hp, int* armor);
	void SetCState(EnemyState s);
	void TakeDamage(float damage);
	void SetInfo(PxVec3 playerPosition, PxVec3 pRight);
	void Update(float dT);
	void Nullify();
	Object2D* obj;
	PxRigidActor* actor;
	float Health;
	float Damage;
	bool IsDead;
	bool PlayAnimAfterCurrent;
	float lastTimeShot;
	float shootDelay;
	float movementSwitchTime;
	float currentMoveTime;
	float movementSpeed;
	EnemyState cState;
	EnemyState prevState;
	MovementState cMState;
	D3DXVECTOR3 originalPos;
	D3DXVECTOR3 moveDir;
	D3DXVECTOR3 lookDirection;
	float colRadius;
	float colCapsuleHeight;
	void SetPos(D3DXVECTOR3 pos);
	void RenderBullets(D3DXMATRIX* camView);
	inline std::string LowCaseString(std::string string)
	{
		std::string newString = string;
		for(DWORD i = 0; i < string.size(); i++)
		{
			newString[i] = (char)tolower(string[i]);
		}
		return newString;
	}
private:
	
	int* health;
	int* armour;
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

	inline float Vector3Distance(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
};

#endif