#ifndef _FIRSTPERSONPLAYER_H_
#define _FIRSTPERSONPLAYER_H_

#include "ResourceManager.h"
#include "InputHandler.h"
#include "CollisionGeo.h"

class FirstPersonPlayer
{
public:
	FirstPersonPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot,LPDIRECT3DDEVICE9 d3dDev,ResourceManager* res,InputHandler* input,std::vector<unsigned int>* keyArray);
	inline D3DXMATRIX* GetCameraView()
	{
		return &cam.m_View;
	}
	inline D3DXVECTOR3 GetPosition()
	{
		return position;
	}
	inline D3DXVECTOR3 GetLookAt()
	{
		return cam.lookAt;
	}
	inline D3DXVECTOR3 GetLookDir()
	{
		return cam.lookDir;
	}
	inline void SetGrounded(bool val)
	{
		isGrounded = true;
	}
	inline CollisionGeo::CollisionInfo DoesCollideWith(CollisionGeo* other)
	{
		return collision->DidAABBCollideWithAABB(other);
	}
	inline CollisionGeo* GetCollision()
	{
		return collision;
	}
	void Collision(CollisionGeo::CollisionInfo* info);
	void Update(float deltaTime);
	void FixedUpdate(float deltaTime);
	void Render();
private:
	//camera vars
	struct Camera
	{
		D3DXVECTOR3 lookAt;
		D3DXMATRIXA16 m_View;
		D3DXMATRIX m_Projection;
		D3DXVECTOR3 lookDir;
	};
	Camera cam;
	D3DXVECTOR3 position;
	float sensitivity;
	float angleX;
	float angleY;

	//base vars
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	InputHandler* inputHandler;
	std::vector<unsigned int>* keys;

	//player related
	D3DXVECTOR3 Input(float deltaTime);
	int KeyPressed(int key);
	CollisionGeo* collision;
	bool isGrounded;
	float gravity;
	float fixedDeltaTime;
	float normalDeltaTime;
	D3DXVECTOR3 moveDir;

	//camera functions
	void DoCameraStuff(float deltaTime);
	void SetUpCamera();
	D3DXVECTOR3 ReturnDirection(float anglesX,float anglesY);
	void SetCameraLook(float anglesX,float anglesY);
};

#endif