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

	Enemy(ResourceManager* res, PhysXEngine* phys);
	void Move(D3DXVECTOR3 dir, float dT);
	void SetCState(EnemyState s);
	void TakeDamage(float damage);
	void SetInfo(PxVec3 playerPosition, PxVec3 pRight);
	void Update(float dT);
	void Nullify();
	void CheckFutureCollision();
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
	ResourceManager* resources;
	PhysXEngine* physics;
	PxVec3 playerRight;
	PxVec3 playerPos;
};

#endif