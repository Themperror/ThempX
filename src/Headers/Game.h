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
#include "../Headers/InputHandler.h"
#include "../Headers/SoundHandler.h"
#include "../Headers/Particle.h"
#include "../Headers/FirstPersonPlayer.h"
#include "../Headers/GUI.h"
#include "../Headers/PhysXEngine.h"
#include "../Headers/Camera.h"

#include <iostream>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <Windows.h>

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
		bool changeDisplay;
		bool windowed;
		int windowSizeX;
		int windowSizeY;
		int renderSizeX;
		int renderSizeY;
		bool applicationActive;
	};
	struct Object3DData
	{
		char* filePath;
		D3DXVECTOR3 position;
		D3DXVECTOR3 rotation;
		D3DXVECTOR3 scale;
		void Nullify()
		{
			filePath = "";
			position = VECTOR3ZERO;
			rotation = VECTOR3ZERO;
			scale = VECTOR3ONE;
		}
	};
	struct Object2DData
	{
		char* filePath;
		bool hasAnimation;
		Camera* cam;
		D3DXVECTOR2 URVertexPos;
		D3DXVECTOR2 LLVertexPos;
		D3DXVECTOR3 position;
		D3DXVECTOR3 scale;
		float textureSizeX, textureSizeY, xRowsAnim,yRowsAnim;
		void Nullify()
		{
			textureSizeX=0;textureSizeY =0;xRowsAnim=0;yRowsAnim = 0;
			filePath = "";
			hasAnimation = false;
			URVertexPos = D3DXVECTOR2(0,0);
			LLVertexPos = D3DXVECTOR2(0,0);
			position = VECTOR3ZERO;
			scale = VECTOR3ONE;
		}
	};
	Game(DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev);
	void Update(double deltaTime);
	void FixedUpdate(double deltaTime);
	void Render();
	void Initialize();
	void ReleaseAll();
	void ReloadGUI();

	bool Create3DObject(bool hasPhysics,Object3DData* data);
	bool CreateAnimated2DObject(bool hasPhysics, Object2DData* data);
	bool CreateStatic2DObject(bool hasPhysics, Object2DData* data);
	//SPEEngine::RigidData CreatePhysicsData(bool draw,bool isStatic, float mass, float density, SPEVector pos,SPEVector scale, SPEVector vel, SPEVector aVel);
	Object2DData CreateObject2DData(char* filePath,bool hasAnim, D3DXVECTOR3 pos,D3DXVECTOR3 scale,D3DXVECTOR2 tSize, D3DXVECTOR2 rows);
	Object3DData CreateObject3DData(char* filePath,D3DXVECTOR3 pos,D3DXVECTOR3 scale,D3DXVECTOR3 rot);
	inline D3DXMATRIX* GetCameraView()
	{
		if(cam != NULL)
		{
			return &cam->GetView();
		}
		else
		{
			return NULL;
		}
	}
private:
	//object holders
	DataStruct* data;
	int qualityLevel;
	std::vector<Object3D*> modelObjs;
	std::vector<Object2D*> spriteObjs;
	std::vector<DebugCube*> debugCubes;
	std::vector<Particle*> particles;
	std::vector<D3DLIGHT9*> lights;

	D3DLIGHT9* CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff);


	//core engine
	HWND handleWindow;
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	InputHandler* inputHandler;
	SoundHandler* soundHandler;
	PhysXEngine* physics;
	Camera* cam;
	GUI* gui;
	void LoadLevel();
	void DestroyLevel();
	bool wireframe;
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
	unsigned int currentEditorObjIndex;
	bool EditorMode;
	bool pressedEditorKey;
	float scaleMultiplier;

	//CollisionThread
	void CollisionThread();


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