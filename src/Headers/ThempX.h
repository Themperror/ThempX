#ifndef _THEMPX_H_
#define _THEMPX_H_

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#include <Windows.h>
#include <windowsx.h>
#include <conio.h>
#include <vector>
#include <d3dx9.h>
#include <d3d9.h>
#include <D3DCommon.h>

#include "ResourceManager.h"
#include "Object3D.h"
#include "Object2D.h"
#include "DebugCube.h"
#include "CollisionGeo.h"
#include "InputHandler.h"
#include "SoundHandler.h"
#include "Particle.h"
#include "Game.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <boost/thread.hpp>

#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
using namespace std;
class ThempX
{
public:


	LPDIRECT3DDEVICE9 p_Device;
	bool isDone;
	MSG input;
	HWND handleWindow;
	ThempX(HWND handle,HINSTANCE hInstance);

	ResourceManager* resources;
	void SetIsDone(void);
private:
	//DirectX Init
	LPDIRECT3DDEVICE9 InitializeDevice(HWND han_WindowToBindTo);
	InputHandler* inputHandler;
	SoundHandler* soundHandler;

	int windowSizeX;
	int windowSizeY;
	int windowPosX;
	int windowPosY;

	void DrawScene();

	void Initialize();
	void Update(); // runs every frame (61fps max)
	void FixedUpdate(); //runs every iterations of the main loop

	Game* g;


	LARGE_INTEGER fixedFrequency;        // ticks per second
	LARGE_INTEGER newFixedDelta, oldFixedDelta;           // ticks

	DWORD currentTicks;
	DWORD oldTicks;

	LARGE_INTEGER frequency;
	LARGE_INTEGER oldDelta,newDelta;


};
#endif