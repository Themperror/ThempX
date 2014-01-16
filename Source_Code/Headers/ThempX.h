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

#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
//#include <boost/thread.hpp>

#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
using namespace std;
class ThempX
{
public:

	struct Camera
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 lookAt;
		D3DXMATRIXA16 m_View;
		D3DXMATRIX m_Projection;
		D3DXVECTOR3 lookDir;
	};
	struct objData
	{
		float x,y,z;
		char* path;
	};


	LPDIRECT3DDEVICE9 p_Device;
	bool isDone;
	MSG input;
	HWND handleWindow;
	ThempX(HWND handle,HINSTANCE hInstance);

	Camera camera;
	ResourceManager* resources;

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

	bool CheckCamBoxCollision(D3DXVECTOR3 pos,Object3D* obj);

	void DoCameraStuff();
	void DoInput();
	void LeftMouseClick();
	void SetUpCamera();
	void Initialize();
	void Update();
	void LoadLevel();
	void DestroyLevel();
	void CollisionThread();

	D3DXVECTOR3 AddVector3(D3DXVECTOR3* a, D3DXVECTOR3* b);
	D3DXVECTOR3 SubstractVector3(D3DXVECTOR3* a, D3DXVECTOR3* b);
	D3DXVECTOR3 ReturnDirection(float anglesX,float anglesY);

	std::vector<Object3D*> modelObjs;
	std::vector<Object2D*> spriteObjs;
	std::vector<DebugCube*> debugCubes;
	std::vector<D3DLIGHT9*> lights;

	DWORD oldTicks;
	DWORD newTicks;
	float deltaTime;

	bool snapCursor;

	float sensitivity;
	float angleX;
	float angleY;

	D3DXVECTOR3 lookDir;

	vector<unsigned int> keys;
	int KeyPressed(int key);
	bool mouseLeftJustDown;
	bool mouseRightJustDown;
	bool lockCursor;
	D3DXVECTOR3 oldPos;
	D3DXVECTOR3 newPos;

	void SetCameraLookX(float anglesX,float anglesY);
	D3DLIGHT9* ThempX::CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff);
	inline float Vector3Distance(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
};
#endif