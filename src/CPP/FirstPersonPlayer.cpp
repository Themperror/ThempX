#include "../Headers/FirstPersonPlayer.h"

FirstPersonPlayer::FirstPersonPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot,InputHandler* input,std::vector<unsigned int>* keyArray)
{
	inputHandler = NULL;
	keys = NULL;

	checkCollision = true;
	moveDir= D3DXVECTOR3(0,0,0);
	
	inputHandler = input;
	keys = keyArray;

	isGrounded = false;
	gravity = 0;
}
void FirstPersonPlayer::Update(float deltaTime)
{
	normalDeltaTime = deltaTime;
}
void FirstPersonPlayer::FixedUpdate(float deltaTime)
{
	fixedDeltaTime = deltaTime;

	moveDir += Input(deltaTime);
	moveDir.y -= gravity*deltaTime;

	moveDir = D3DXVECTOR3(0,0,0);
}
void FirstPersonPlayer::Render()
{

}
D3DXVECTOR3 FirstPersonPlayer::Input(float deltaTime)
{
	D3DXVECTOR3 moveDir = D3DXVECTOR3(0,0,0);
	int speed = 20;
	if(KeyPressed(DIK_LSHIFT))
	{
		speed = 40;
	}
	else
	{
		speed = 20;
	}
	
	if(KeyPressed(DIK_R) == 2)
	{
		checkCollision = true;
		isGrounded= false;
	}
	return moveDir;
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
}