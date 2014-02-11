#ifndef _GAME_H_
#define _GAME_H_
#define IFCOL if(currentEditorObj->col != NULL)
#define IFOBJ3D if(currentEditorObj->obj3D != NULL)
#define IFOBJ2D if(currentEditorObj->obj2D != NULL)
#define VECTOR3ZERO D3DXVECTOR3(0,0,0)
#define VECTOR3ONE D3DXVECTOR3(1,1,1)
#include <Windows.h>
#include <windowsx.h>
#include <conio.h>
#include <vector>
#include <d3dx9.h>
#include <d3d9.h>
#include <D3DCommon.h>

#include "../Headers/ResourceManager.h"
#include "../Headers/Object3D.h"
#include "../Headers/Object2D.h"
#include "../Headers/DebugCube.h"
#include "../Headers/CollisionGeo.h"
#include "../Headers/InputHandler.h"
#include "../Headers/SoundHandler.h"
#include "../Headers/Particle.h"
#include "../Headers/FirstPersonPlayer.h"
#include "../Headers/GUI.h"
#include "../Headers/SPEEngine.h"

#include <SPE.h>
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
	inline D3DXMATRIX* GetCameraView()
	{
		return NULL;
	}
private:
	//object holders
	DataStruct* data;
	std::vector<Object3D*> modelObjs;
	std::vector<Object2D*> spriteObjs;
	std::vector<DebugCube*> debugCubes;
	std::vector<Particle*> particles;
	std::vector<D3DLIGHT9*> lights;

	D3DLIGHT9* CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff);

	FirstPersonPlayer* player;

	//core engine
	HWND handleWindow;
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	InputHandler* inputHandler;
	SoundHandler* soundHandler;
	SPEEngine* physics;
	GUI* gui;
	void LoadLevel();
	void DestroyLevel();

	//////game functions

	


	//input stuff
	void DoInput(float deltaTime);
	void LeftMouseClick();
	std::vector<unsigned int> keys;
	int KeyPressed(int key);
	bool mouseLeftJustDown;
	bool mouseRightJustDown;

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
	float scaleMultiplier;

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