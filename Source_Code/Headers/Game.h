#ifndef _GAME_H_
#define _GAME_H_

#include "ResourceManager.h"
class Game
{
public:
	Game();
	void Update(float deltaTime);
	void FixedUpdate(float deltaTime);
	void Render();
private:

};

#endif