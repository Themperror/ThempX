#ifndef _THEMPX_H_
#define _THEMPX_H_

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
#include "InputHandler.h"
#include "SoundHandler.h"
#include "Particle.h"
#include "Game.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>

#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
using namespace std;
class ThempX
{
public:
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager::DataStruct* data;
	MSG input;
	HWND handleWindow;
	ThempX(HINSTANCE hInstance,LPSTR lpCmdLine);
	int wSizeX,wSizeY,oldWSizeX,oldWSizeY;
	ResourceManager* resources;
	void SetIsDone(void);
private:
	//DirectX Init
	void CreateLoadingScreen();
	void SetDisplayModeFullScreen(int devmodeIndex);
	void SetDisplayModeWindowed(int sizeX, int sizeY, int renderSizeX,int renderSizeY);
	void SetDefaultRenderStateSettings();
	LPDIRECT3DDEVICE9 InitializeDevice(HWND han_WindowToBindTo);
	InputHandler* inputHandler;
	SoundHandler* soundHandler;

	void DrawScene();

	void Initialize();
	void Update(); // runs every frame (61fps max)
	void FixedUpdate(); //runs every iterations of the main loop
	void CheckDevice();
	void GetListofDisplayModes();
	Game* g;

	int SRenderSizeX;
	int SRenderSizeY;

	LARGE_INTEGER fixedFrequency;        // ticks per second
	LARGE_INTEGER newFixedDelta, oldFixedDelta;           // ticks

	LARGE_INTEGER currentTicks;
	LARGE_INTEGER currentFrequency;
	LARGE_INTEGER oldTicks;

	LARGE_INTEGER frequency;
	LARGE_INTEGER oldDelta,newDelta;

	HWND NewWindow(LPCSTR className, LPCTSTR windowName,int posX,int posY, int sizeX,int sizeY,bool isWindowed);
	void PreCreateWindow();
	void GetDesktopResolution(int& horizontal, int& vertical);
};
#endif