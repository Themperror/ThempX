#include "../Headers/FirstPersonPlayer.h"

FirstPersonPlayer::FirstPersonPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot,LPDIRECT3DDEVICE9 d3dDev,ResourceManager* res,InputHandler* input,std::vector<unsigned int>* keyArray)
{
	p_Device = NULL;
	resources = NULL;
	inputHandler = NULL;
	keys = NULL;

	cam.lookDir.x = 0;
	cam.lookDir.y = 1;
	cam.lookDir.z = 0;
	cam.lookAt.x = 0;
	cam.lookAt.y = 0;
	cam.lookAt.z = 0;
	
	position.x = pos.x;
	position.y = pos.y;
	position.z = 0;

	angleX = 0;
	angleY = 0;
	sensitivity = 50;

	keys = keyArray;
	p_Device = d3dDev;
	resources = res;
	inputHandler = input;
	if(p_Device == NULL || resources == NULL || inputHandler == NULL)
	{
		std::cout << "Player class not created correctly, Game will probably crash" << std::endl;
	}
}
void FirstPersonPlayer::Update(float deltaTime)
{

}
void FirstPersonPlayer::FixedUpdate(float deltaTime)
{
	Input(deltaTime);
	
	DoCameraStuff(deltaTime);
}
void FirstPersonPlayer::Render()
{
	SetUpCamera();
}
void FirstPersonPlayer::Input(float deltaTime)
{
	int speed = 60;
	if(KeyPressed(DIK_LSHIFT))
	{
		speed = 70;
	}
	else
	{
		speed = 40;
	}
	if(KeyPressed(DIK_Q))
	{
		position.y += deltaTime*speed;
	}
	if(KeyPressed(DIK_E))
	{
		position.y -= deltaTime*speed;
	}
	if(KeyPressed(DIK_W))
	{
		position.x += cam.lookDir.x *deltaTime*speed;
		position.z += cam.lookDir.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_S))
	{
		position.x -= cam.lookDir.x *deltaTime*speed;
		position.z -= cam.lookDir.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_D))
	{
		D3DXVECTOR3 temp = ReturnDirection(angleX-90,angleY);
		position.x += temp.x *deltaTime*speed;
		position.z += temp.z *deltaTime*speed;
	}
	if(KeyPressed(DIK_A))
	{
		D3DXVECTOR3 temp = ReturnDirection(angleX+90,angleY);
		position.x += temp.x *deltaTime*speed;
		position.z += temp.z *deltaTime*speed;
	}
}

//used in DoCameraStuff
void FirstPersonPlayer::SetCameraLook(float anglesX,float anglesY)
{
	float x = 1 * std::cos(anglesX * 3.141592f / 180);
	float z = 1 * std::sin(anglesX * 3.141592f / 180);
	float y = 1 * std::tan(anglesY * 3.141592f / 200);
	cam.lookAt.x = position.x+x;
	cam.lookAt.y = position.y+y;
	cam.lookAt.z = position.z+z;

	cam.lookDir.x = cam.lookAt.x - position.x;
	cam.lookDir.y = cam.lookAt.y - position.y;
	cam.lookDir.z = cam.lookAt.z - position.z;
}
//sets the camera for use
void FirstPersonPlayer::SetUpCamera()
{
	D3DXVECTOR3 m_UpVector(0, 1, 0);
	D3DXMatrixLookAtLH(&cam.m_View, &position, &cam.lookAt, &m_UpVector);
	p_Device->SetTransform(D3DTS_VIEW, &cam.m_View);
	D3DXMatrixPerspectiveFovLH(&cam.m_Projection, D3DX_PI/4, 500/500, 1, 2000);
	p_Device->SetTransform(D3DTS_PROJECTION, &cam.m_Projection);
}
//Camera rotation handling, also needs to be in game.cpp)
void FirstPersonPlayer::DoCameraStuff(float deltaTime)
{
	if(angleX < -360 || angleX > 720)
	{
		angleX = 0;
	}
	float xMod = inputHandler->GetMousePosX()*deltaTime*sensitivity;
	if(angleX-xMod < 0) 
	{
		float rest = xMod - angleX;
		angleX = 360-rest;
	}
	else if(angleX-xMod > 360)
	{
		float rest = abs(360 - angleX+xMod);
		angleX = rest;
	}
	else
	{
		angleX -= xMod;
	}
	angleY-=inputHandler->GetMousePosY()*deltaTime*sensitivity;
	(angleY < -50 ? angleY = -50 : (angleY> 50 ? angleY = 50 : 0));
	SetCameraLook(angleX,angleY);
}
D3DXVECTOR3 FirstPersonPlayer::ReturnDirection(float anglesX,float anglesY)
{    
	return D3DXVECTOR3((position.x+ 1 * std::cos(anglesX * 3.141592f / 180)) - position.x,(position.y+1 * std::tan(anglesY * 3.141592f / 200))-position.y,(position.z+1 * std::sin(anglesX * 3.141592f / 180))-position.z);
}
int FirstPersonPlayer::KeyPressed(int key)
{
	if(inputHandler->KeyPressed(key))
	{
		if(keys->at(key) == 0)
		{
			keys->at(key) = 1;
			return 2 ;
		}
		return 1;
	}
	else
	{
		keys->at(key) = 0;
		return 0;
	}
	return 0;
}