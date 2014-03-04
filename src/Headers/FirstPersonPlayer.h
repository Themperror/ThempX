#ifndef _FIRSTPERSONPLAYER_H_
#define _FIRSTPERSONPLAYER_H_

#include <d3dx9.h>
#include <d3d9.h>
#include <vector>

#include "InputHandler.h"

class FirstPersonPlayer
{
public:
	FirstPersonPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 rot,InputHandler* input,std::vector<unsigned int>* keyArray);
	
	inline void SetGrounded(bool val)
	{
		isGrounded = val;
	}
	
	void Update(float deltaTime);
	void FixedUpdate(float deltaTime);
	void Render();
private:

	InputHandler* inputHandler;
	std::vector<unsigned int>* keys;

	//player related
	D3DXVECTOR3 Input(float deltaTime);
	int KeyPressed(int key);
	bool isGrounded;
	bool checkCollision;
	float gravity;
	float fixedDeltaTime;
	float normalDeltaTime;
	D3DXVECTOR3 moveDir;
};

#endif