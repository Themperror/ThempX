#ifndef _GAME_H_
#define _GAME_H_

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
class Game
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
	struct DataStruct
	{
		bool loop;
		bool lockCursor;
	};
	Game(DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev);
	void Update(double deltaTime);
	void FixedUpdate(double deltaTime);
	void Render();
	void Initialize();
	void ReleaseAll();
private:
	//object holders
	DataStruct* data;
	std::vector<Object3D*> modelObjs;
	std::vector<Object2D*> spriteObjs;
	std::vector<DebugCube*> debugCubes;
	std::vector<Particle*> particles;

	std::vector<D3DLIGHT9*> lights;
	D3DLIGHT9* CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff);



	//core engine
	HWND handleWindow;
	LPDIRECT3DDEVICE9 p_Device;
	Camera camera;
	ResourceManager* resources;
	InputHandler* inputHandler;
	SoundHandler* soundHandler;
	void LoadLevel();
	void DestroyLevel();

	//////game functions

	//camera functions
	void DoCameraStuff(float deltaTime);
	void SetUpCamera();
	D3DXVECTOR3 ReturnDirection(float anglesX,float anglesY);
	void SetCameraLook(float anglesX,float anglesY);


	//input stuff
	void DoInput(float deltaTime);
	void LeftMouseClick();
	std::vector<unsigned int> keys;
	int KeyPressed(int key);
	bool mouseLeftJustDown;
	bool mouseRightJustDown;
	float sensitivity;
	float angleX;
	float angleY;

	//editor mode
	void CreateLevelFile();
	void SetUpEditorMode();
	struct EditorObj
	{
		Object2D* obj2D;
		Object3D* obj3D;
		DebugCube* col;
	};
	std::vector<EditorObj> editorObjs;
	EditorObj* currentEditorObj;
	int currentEditorObjIndex;
	bool EditorMode;
	bool pressedEditorKey;

	//CollisionThread
	void CollisionThread();
	boost::mutex collisionLock;


	//math functions
	inline float Vector3Distance(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
	inline D3DXVECTOR3 AddVector3(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return D3DXVECTOR3(a->x+b->x,a->y+b->y,a->z+b->z);
	}
	inline D3DXVECTOR3 SubstractVector3(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return D3DXVECTOR3(a->x-b->x,a->y-b->y,a->z-b->z);
	}
	inline D3DXVECTOR3 MultiplyVector3(D3DXVECTOR3* a,float val)
	{
		return D3DXVECTOR3(a->x*val,a->y*val, a->z*val);
	}

};

#endif