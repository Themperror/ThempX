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
#include "../Headers/TriggerBox.h"
#include "../Headers/Menu.h"
#include "../Headers/Enemy.h"

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
	struct Item
	{
		Object2D* obj2D;
		Object3D* obj3D;
		std::string name;
		TriggerBox* trigger;

		void (Game::*MemberPointerType1)(Item* item); //function to execute (LIMITATION: CANNOT RETURN ANYTHING OR ACCEPT ARGUMENTS UNLESS HARDCODED)
		void (Game::*MemberPointerType2)(void);
		Game* context; //reference to class ("this")

		Item(std::string itemName,void (Game::*MemberFunction)(Item* item) ,Game* gContext, D3DXVECTOR3 pos, D3DXVECTOR3 halfWidthSize)
		{
			trigger = new TriggerBox(pos,halfWidthSize);
			name = itemName;
			MemberPointerType1 = MemberFunction;
			MemberPointerType2 = NULL;
			context = gContext;
			obj2D = NULL;
			obj3D = NULL;
			tBoxVisualized = NULL;
		}
		Item(void (Game::*MemberFunction)(void) ,Game* gContext, D3DXVECTOR3 pos, D3DXVECTOR3 halfWidthSize)
		{
			trigger = new TriggerBox(pos,halfWidthSize);
			name = "Trigger";
			MemberPointerType1 = NULL;
			MemberPointerType2 = MemberFunction;
			context = gContext;
			obj2D = NULL;
			obj3D = NULL;
			tBoxVisualized = NULL;
		}
		void CheckTrigger(D3DXVECTOR3* pos)
		{
			if(trigger->CheckInBox(pos) && MemberPointerType1 !=NULL)
			{
				(context->*MemberPointerType1)(this);
			}
			else if(trigger->CheckInBox(pos) && MemberPointerType2 !=NULL)
			{
				(context->*MemberPointerType2)();
			}
		}
		
		DebugCube* tBoxVisualized;
		void Draw()
		{
			if(obj2D != NULL)
			{
				obj2D->SetPosition(&trigger->GetPosition());
				obj2D->Draw();
			}
			else if(obj3D != NULL)
			{
				obj3D->SetPosition(&trigger->GetPosition());
				obj3D->DrawModel();
			}
			if(tBoxVisualized != NULL)
			{
				DrawTriggerBox();
			}
		}
		void DrawTriggerBox()
		{
			tBoxVisualized->doRender = true;
			tBoxVisualized->position = trigger->GetPosition();
			tBoxVisualized->scaling = trigger->GetHalfSize();
			tBoxVisualized->rotation = D3DXVECTOR3(0,0,0);
			tBoxVisualized->Draw();
		}
		void Release()
		{
			if(obj2D != NULL)
			{
				obj2D->ReleaseResources();
				delete obj2D;
			}
			else if(obj3D != NULL)
			{
				delete obj3D;
			}
			delete trigger;
		}
	};
	struct Player
	{
		Camera* cam;
		PxController* physicsPlayer;

		bool hasRedKey;
		bool hasGreenKey;
		bool isDead;
		bool lockCam;
		int* health;
		int* armor;
		GUI* gui;

		Player(Camera* camera, PxController* controller, GUI* g, int* hp, int* am)
		{
			cam = camera;
			physicsPlayer = controller;
			hasRedKey = false;
			hasGreenKey = false;
			isDead = false;
			lockCam = true;
			health = hp;
			armor = am;
			gui = g;
		}
		void TakeDamage(float damage)
		{
			if(*armor > 0)
			{
				*armor -= damage;
				if(*armor < 0)
				{
					damage = abs(*armor);
					*health -= damage;
					*armor = 0;
				}
			}
			else
			{
				*health -= damage;
			}
			if(*health > 0)
			{
				if(*health >= 75)
				{
					GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
					if(g->currentlyPlayingAnimation.compare("100") != 0)
					{
						gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"100");
					}
				}
				else if(*health < 75 && *health >= 50)
				{
					GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
					if(g->currentlyPlayingAnimation.compare("75") != 0)
					{
						gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"75");
					}
				}
				else if(*health < 50 && *health >= 25)
				{
					GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
					if(g->currentlyPlayingAnimation.compare("50") != 0)
					{
						gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"50");
					}
				}
				else if(*health < 25 && *health > 0)
				{
					GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
					if(g->currentlyPlayingAnimation.compare("25") != 0)
					{
						gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"25");
					}
				}
			}
			else
			{
				GUI::GUITexture* g = gui->GetGUIObj("Resources/GUI/Heart.png");
				if(g->currentlyPlayingAnimation.compare("0") != 0)
				{
					gui->PlayAnimation(gui->GetGUIObj("Resources/GUI/Heart.png"),"0");
				}
			}
		}
		D3DXVECTOR3 GetD3DXPosition()
		{
			PxExtendedVec3 p = physicsPlayer->getPosition();
			return D3DXVECTOR3((float)p.x,(float)p.y,(float)p.z);
		}
		PxExtendedVec3 GetPxExtendedVec3Position()
		{
			return physicsPlayer->getPosition();
		}
		PxVec3 GetPxVec3Position()
		{
			PxExtendedVec3 p = physicsPlayer->getPosition();
			return PxVec3((PxReal)p.x,(PxReal)p.y,(PxReal)p.z);
		}
		void Move(PxVec3 moveDir, float dT)
		{
			physicsPlayer->move(moveDir,0.001f,dT,NULL,NULL);
			if(lockCam)
			{
				cam->SetPosition(GetD3DXPosition()+D3DXVECTOR3(0,2,0));
			}
		}
	};
	struct Model
	{
		Model()
		{
			obj = NULL;
			actor = NULL;
		}
		Object3D* obj;
		PxRigidActor* actor;
	};
	struct Sprite
	{
		Sprite()
		{
			obj = NULL;
			actor = NULL;
		}
		Object2D* obj;
		PxRigidActor* actor;
	};
	
	struct Door
	{
		void (Game::*sound)(std::string name); //function to execute (LIMITATION: CANNOT RETURN ANYTHING OR ACCEPT ARGUMENTS UNLESS HARDCODED)
		Game* context; //class to use (Has to be of type Game)
		bool playedSound;
		bool requiresRedKey;
		bool requiresGreenKey;
		float timeOpen;
		Door(void (Game::*SoundFunction)(std::string name) ,Game* gContext)
		{
			sound = SoundFunction;
			context = gContext;
			timeOpen = 0;
			Nullify();
		}
		void MoveUp(float dT)
		{
			if(activated && !isUp)
			{
				if(!playedSound) 
				{
					(context->*sound)("DoorUp");
					playedSound = true;
				}
				isMoving = true;
				float amount = obj->position.y + 50;
				obj->position.y += amount*dT;
				actor->isRigidDynamic()->setKinematicTarget(PxTransform(obj->position.x,obj->position.y,obj->position.z));
				if(obj->position.y >= position.y +9)
				{
					obj->position.y = position.y+9;
					actor->isRigidDynamic()->setKinematicTarget(PxTransform(obj->position.x,obj->position.y,obj->position.z));
					isMoving = false;
					isUp = true;
					activated= false;
					playedSound = false;
				}
			}
		}
		void Update(float dT)
		{
			if(isUp)
			{
				timeOpen += dT;
				if(timeOpen > 3)
				{
					activated = true;
					timeOpen = 0;
				}
			}
			MoveUp(dT);
			MoveDown(dT);

		}
		void MoveDown(float dT)
		{
			if(activated && isUp)
			{
				if(!playedSound) 
				{
					PxExtendedVec3 pPos = context->physics->player->getPosition();
					D3DXVECTOR3 dPos = D3DXVECTOR3((float)pPos.x,(float)pPos.y,(float)pPos.z);
					//std::cout << "Player Pos X: " << pPos.x << " Y: " << pPos.y << " Z: " << pPos.z << "   Door Pos X: " << obj->position.x << " Y: " << obj->position.y << " Z: " << obj->position.z << std::endl;
					float DISTANCE = context->Vector3Distance(&dPos,&obj->position);
					std::cout << "distance was " << DISTANCE << std::endl;
					if(DISTANCE < 160)
					{
						(context->*sound)("DoorDown");
					}
					playedSound = true;
				}
				isMoving = true;
				float amount = position.y - obj->position.y +50;
				obj->position.y -= amount*dT;
				actor->isRigidDynamic()->setKinematicTarget(PxTransform(obj->position.x,obj->position.y,obj->position.z));
				if(obj->position.y <= position.y) 
				{
					obj->position.y = position.y;
					actor->isRigidDynamic()->setKinematicTarget(PxTransform(obj->position.x,obj->position.y,obj->position.z));
					isMoving = false;
					isUp = false;
					activated = false;
					playedSound = false;
					timeOpen = 0;
				}
			}
		}

		Object3D* obj;
		PxRigidActor* actor;
		D3DXVECTOR3 position;
		bool isUp;
		bool isMoving;
		bool activated;
		bool breakable;
		float health;
		void Nullify()
		{
			obj = NULL;
			actor = NULL;
			position = D3DXVECTOR3(0,0,0);
			isUp = false;
			playedSound = false;
			isMoving = false;
			activated = false;
			breakable = false;
			requiresGreenKey = false;
			requiresRedKey = false;
			health = 30;
		}
	};
	struct PhysicsUserData
	{
		PhysicsUserData()
		{
			Nullify();
		}
		Object3D* related3D; //backwards compatibility
		Object2D* related2D; //backwards compatibility
		//WorldModelObject* worldObj;
		Enemy* enemy;
		void Nullify()
		{
			related3D = NULL;
			related2D = NULL;
			enemy = NULL;
		}
	};
	struct PhysicsData
	{
		enum PhysicsType{Box,Capsule,Sphere,Mesh,BoundingBox,Uninitialized};
		PhysicsType cType;
		DWORD flags; //unused
		PxVec3 boxHalfWidth;
		PxReal radius;
		PxReal capsuleHeight;
		bool isStatic;
		bool isKinematic;
		bool flipNormals;
		void Nullify()
		{
			isStatic = false;
			isKinematic = false;
			flipNormals = true;
			flags = NULL;
			cType = Uninitialized;
			boxHalfWidth = PxVec3(0,0,0);
			radius = 0;
			capsuleHeight = 0;
		}
	};
	struct Object3DData
	{
		char* filePath;
		D3DXVECTOR3 position;
		D3DXVECTOR3 rotation;
		D3DXVECTOR3 scale;
		PhysicsData physics;
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
		PhysicsData physics;
		int textureSizeX, textureSizeY, xRowsAnim,yRowsAnim;
		void Nullify()
		{
			textureSizeX=0;textureSizeY =0;xRowsAnim=0;yRowsAnim = 0;
			filePath = "";
			hasAnimation = false;
			URVertexPos = D3DXVECTOR2(0,0);
			LLVertexPos = D3DXVECTOR2(0,0);
			position = VECTOR3ZERO;
			scale = VECTOR3ONE;
			physics.Nullify();
		}

	};

	Game(ResourceManager::DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev);
	void Update(double deltaTime);
	void FixedUpdate(double deltaTime);
	void Render();
	void Initialize();
	void Release();
	void ReloadGUI();
	void ReleaseEnemy(Enemy* enemy);
	void RemoveEnemyCollision(Enemy* enemy);
	void OpenDoor();
	void LevelComplete();
	void PlaceEnemy(std::string textureString, D3DXVECTOR3 position, D3DXVECTOR3 scaling, int xRows,int yRows, float colRadius, float colHeight);
	bool Create3DObject(bool hasPhysics,Object3DData* data);
	bool CreateAnimatedSprite(bool hasPhysics, Object2DData* data);
	bool CreateStaticSprite(bool hasPhysics, Object2DData* data);
	bool CreateEnemy(bool hasPhysics, Object2DData* data);
	Object2DData CreateObject2DData(char* filePath,bool hasAnim, D3DXVECTOR3 pos,D3DXVECTOR3 scale, D3DXVECTOR2 rows, PhysicsData pData);
	Object3DData CreateObject3DData(char* filePath,D3DXVECTOR3 pos,D3DXVECTOR3 scale,D3DXVECTOR3 rot, PhysicsData pData);

	PxRaycastBuffer RayFromPlayer();
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
	inline Object3D* GetLastObject3D()
	{
		return modelObjs.at(modelObjs.size()-1)->obj;
	}
	inline Object2D* GetLastObject2D()
	{
		return sprites.at(sprites.size()-1)->obj;
	}
	inline Enemy* GetLastEnemy()
	{
		return enemies.at(enemies.size()-1);
	}
	inline PhysicsData CreatePhysicsData(PxVec3 boxHalf, bool isStatic = true, bool isKinematic = false, DWORD flags = 0)
	{
		PhysicsData data;
		data.Nullify();
		data.isStatic = isStatic;
		data.isKinematic = isKinematic;
		if(boxHalf.x == 0 && boxHalf.y == 0 && boxHalf.z == 0)
		{
			data.cType = PhysicsData::BoundingBox;
		}
		else
		{
			data.cType = PhysicsData::Box;
		}
		data.boxHalfWidth = boxHalf;
		return data;
	}
	inline PhysicsData CreatePhysicsData(PxReal radius, PxReal capHeight = 0, bool isStatic = false, bool isKinematic = true, DWORD flags = 0)
	{
		PhysicsData data;
		data.Nullify();
		data.isStatic = isStatic;
		data.isKinematic = isKinematic;
		if(capHeight == 0)
		{
			data.cType = PhysicsData::Sphere;
		}
		else
		{
			data.cType = PhysicsData::Capsule;
		}
		data.radius = radius;
		data.capsuleHeight = capHeight;
		return data;
	}
	inline PhysicsData CreatePhysicsDataNull()
	{
		PhysicsData data;
		data.Nullify();
		return data;
	}
	void ItemTrigger(Item* item);
	void PlaySound(std::string name);
private:
	//object holders
	ResourceManager::DataStruct* data;
	int qualityLevel;
	std::vector<Model*> modelObjs;
	std::vector<Enemy*> enemies;
	std::vector<Sprite*> sprites;
	std::vector<DebugCube*> debugCubes;
	std::vector<Particle*> particles;
	std::vector<D3DLIGHT9*> lights;
	std::vector<Door*> doors;
	std::vector<Item*> items;
	D3DLIGHT9* CreateLight(D3DXVECTOR3 position,D3DXVECTOR3 direction, D3DLIGHTTYPE lightType,D3DXCOLOR lightColor,float range,float falloff);


	//core engine
	HWND handleWindow;
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	InputHandler* inputHandler;
	SoundHandler* soundHandler;
	PhysXEngine* physics;
	Menu* menu;
	Camera* cam;
	GUI* gui;
	void LoadLevel(char* txtPath);
	void LoadItems(char* txtPath);
	void LoadEnemies(char* txtPath);
	void DestroyLevel();
	bool wireframe;

	//////game functions and variables
	
	Player* player;
	bool levelComplete;
	//input stuff
	PxVec3 DoInput(float deltaTime);
	void HandlePlayerCollisions(PxVec3 moveDir);
	void LeftMouseClick();
	void RightMouseClick();
	void CreateBullet(PxVec3 origin, PxVec3 dir);
	std::vector<unsigned int> keys;
	int KeyPressed(int key);
	bool mouseLeftJustDown;
	bool mouseRightJustDown;
	bool camFall;
	//editor mode
	void UndoEditorAction();
	enum EditorAction{ThrowCube,Dynamic,Static,ModelWithCollision,PlacedDoor,None};
	std::vector<EditorAction> lastAction;
	bool doorReqRed;
	bool doorReqGreen;
	inline void RemoveLastAction()
	{
		if(lastAction.size() > 0)
		{
			lastAction.erase(lastAction.end()-1);
		}
	}
	inline EditorAction GetLastAction()
	{
		if(lastAction.size() > 0)
		{
			return lastAction.at(lastAction.size()-1);
		}	
		else return None;
	}
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
	float editorObjectDistance;
	float scaleMultiplier;
	DebugCube* dCube;


	//math functions
	inline float Vector3Distance(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
	inline float Vector3Distance(D3DXVECTOR3 a, D3DXVECTOR3 b)
	{
		return sqrt((pow(a.x,2) + pow(b.x,2))+(pow(a.y,2) + pow(b.y,2))+(pow(a.z,2) + pow(b.z,2)));
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
	inline D3DXVECTOR3 DivideVector3(D3DXVECTOR3* a, float val)
	{
		return D3DXVECTOR3(a->x/val,a->y/val, a->z/val);
	}
	inline PxVec3 ToVec3(D3DXVECTOR3* pos)
	{
		return PxVec3(pos->x,pos->y,pos->z);
	}
	inline D3DXVECTOR3 ToD3D(PxVec3* pos)
	{
		return D3DXVECTOR3(pos->x,pos->y,pos->z);
	}
};

#endif