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
		int devmodeIndex;
		D3DPRESENT_PARAMETERS d3dxpresentationparams;
		bool applicationActive;
	};
	struct Item
	{
		Object2D* obj2D;
		Object3D* obj3D;
		std::string name;
		TriggerBox* trigger;

		void (Game::*MemberPointerType)(Item* item); //function to execute (LIMITATION: CANNOT RETURN ANYTHING OR ACCEPT ARGUMENTS UNLESS HARDCODED)
		Game* context; //class to use (Has to be of type Game)

		Item(std::string itemName,void (Game::*MemberFunction)(Item* item) ,Game* gContext, D3DXVECTOR3 pos, D3DXVECTOR3 halfWidthSize)
		{
			trigger = new TriggerBox(pos,halfWidthSize);
			name = itemName;
			MemberPointerType = MemberFunction;
			context = gContext;
			obj2D = NULL;
			obj3D = NULL;
			tBoxVisualized = NULL;
		}
		void CheckTrigger(D3DXVECTOR3* pos)
		{
			if(trigger->CheckInBox(pos) && MemberPointerType !=NULL)
			{
				(context->*MemberPointerType)(this);
			}
		}
		
		DebugCube* tBoxVisualized;
		void Draw()
		{
			if(obj2D != NULL)
			{
				obj2D->SetPosition(trigger->GetPosition());
				obj2D->Draw();
			}
			else if(obj3D != NULL)
			{
				obj3D->SetPosition(trigger->GetPosition());
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

		Player(Camera* camera, PxController* controller)
		{
			cam = camera;
			physicsPlayer = controller;
			hasRedKey = false;
			hasGreenKey = false;
		}
		D3DXVECTOR3 GetD3DXPosition()
		{
			PxExtendedVec3 p = physicsPlayer->getPosition();
			return D3DXVECTOR3(p.x,p.y,p.z);
		}
		PxExtendedVec3 GetPxExtendedVec3Position()
		{
			return physicsPlayer->getPosition();
		}
		PxVec3 GetPxVec3Position()
		{
			PxExtendedVec3 p = physicsPlayer->getPosition();
			return PxVec3(p.x,p.y,p.z);
		}
		void Move(PxVec3 moveDir, float dT)
		{
			physicsPlayer->move(moveDir,0.001f,dT,NULL,NULL);
			cam->SetPosition(GetD3DXPosition()+D3DXVECTOR3(0,2,0));
		}
	};
	struct Enemy
	{
		Enemy()
		{
			Nullify();
		}
		Object2D* obj;
		PxRigidActor* actor;
		float Health;
		float Damage;
		bool IsDead;
		bool PlayAnimAfterCurrent;
		float lastTimeShot;
		float shootDelay;
		float movementSwitchTime;
		float currentMoveTime;
		enum EnemyState{Moving,Shooting,Dead,Idle,Damaged};
		enum MovementState{Forward,Left,Right,Back};
		EnemyState cState;
		EnemyState prevState;
		MovementState cMState;
		D3DXVECTOR3 originalPos;
		D3DXVECTOR3 moveDir;
		D3DXVECTOR3 lookDirection;
		void Move(D3DXVECTOR3 dir, float dT)
		{
			obj->position += dir*dT;
			if(actor->isRigidDynamic() != NULL) actor->isRigidDynamic()->setKinematicTarget(PxTransform(PxVec3(obj->position.x,obj->position.y,obj->position.z)));
		}
		void SetCState(EnemyState s)
		{
			prevState = cState;
			cState = s;
		}
		void TakeDamage(float damage)
		{
			Health -= damage;
			if(Health <= 0)
			{
				obj->PlayAnimation("Death");
				PlayAnimAfterCurrent = false;
				IsDead = true;
			}
			else
			{
				prevState = cState;
				cState = Damaged;
				//obj->PlayAnimation("Damage");
				PlayAnimAfterCurrent = true;
			}
		}
		void Nullify()
		{
			Health = 0;
			Damage = 0;
			cState = EnemyState::Moving;
			cMState = MovementState::Forward;
			lastTimeShot = 2.5f;
			movementSwitchTime = 0.25f;
			moveDir = D3DXVECTOR3(0,0,0);
			currentMoveTime = 0;
			originalPos = D3DXVECTOR3(0,0,0);
			shootDelay = 5.0f;
			obj = NULL;
			actor = NULL;
			IsDead = false;
			PlayAnimAfterCurrent = true;
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
	struct Bullet
	{
		Bullet(Object2D* texture, D3DXVECTOR3 dir)
		{
			obj = texture;
			direction = dir;
			hasHit = false;
			bulletLife = 0;
		}
		Object2D* obj;
		D3DXVECTOR3 direction;
		float bulletLife;
		bool hasHit;
	};
	struct Door
	{
		void (Game::*sound)(std::string name); //function to execute (LIMITATION: CANNOT RETURN ANYTHING OR ACCEPT ARGUMENTS UNLESS HARDCODED)
		Game* context; //class to use (Has to be of type Game)
		bool playedSound;
		Door(void (Game::*SoundFunction)(std::string name) ,Game* gContext)
		{
			sound = SoundFunction;
			context = gContext;
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
				if(obj->position.y >= position.y +15)
				{
					obj->position.y = position.y+15;
					actor->isRigidDynamic()->setKinematicTarget(PxTransform(obj->position.x,obj->position.y,obj->position.z));
					isMoving = false;
					isUp = true;
					activated= false;
					playedSound = false;
				}
			}
		}
		void MoveDown(float dT)
		{
			if(activated && isUp)
			{
				if(!playedSound) 
				{
					(context->*sound)("DoorDown");
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
		enum PhysicsType{Box,Capsule,Sphere,Mesh,Uninitialized};
		PhysicsType cType;
		DWORD flags; //unused
		PxVec3 boxHalfWidth;
		PxReal radius;
		PxReal capsuleHeight;
		bool isStatic;
		bool isKinematic;
		void Nullify()
		{
			isStatic = false;
			isKinematic = false;
			flags = NULL;
			cType = PhysicsType::Uninitialized;
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
	Game(DataStruct* b,HWND windowHandle,ResourceManager* resMan,InputHandler* inputHand,SoundHandler* soundHand, LPDIRECT3DDEVICE9 d3dDev);
	void Update(double deltaTime);
	void FixedUpdate(double deltaTime);
	void Render();
	void Initialize();
	void ReleaseAll();
	void ReloadGUI();
	void ReleaseEnemy(Enemy* enemy);
	void RemoveEnemyCollision(Enemy* enemy);
	void DoAI(float dT);
	void OpenDoor();
	void PlaceEnemy(std::string textureString, D3DXVECTOR3 position, D3DXVECTOR3 scaling, float xRows,float yRows, float colRadius, float colHeight);
	bool Create3DObject(bool hasPhysics,Object3DData* data);
	bool CreateAnimated2DObject(bool hasPhysics, Object2DData* data);
	bool CreateStatic2DObject(bool hasPhysics, Object2DData* data);
	//SPEEngine::RigidData CreatePhysicsData(bool draw,bool isStatic, float mass, float density, SPEVector pos,SPEVector scale, SPEVector vel, SPEVector aVel);
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
		return spriteObjs.at(spriteObjs.size()-1)->obj;
	}
	inline PhysicsData CreatePhysicsData(PxVec3 boxHalf, bool isStatic = true, bool isKinematic = false, DWORD flags = 0)
	{
		PhysicsData data;
		data.Nullify();
		data.isStatic = isStatic;
		data.isKinematic = isKinematic;
		data.cType = PhysicsData::PhysicsType::Box;
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
			data.cType = PhysicsData::PhysicsType::Sphere;
		}
		else
		{
			data.cType = PhysicsData::PhysicsType::Capsule;
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
	DataStruct* data;
	int qualityLevel;
	std::vector<Model*> modelObjs;
	std::vector<Enemy*> spriteObjs;
	std::vector<DebugCube*> debugCubes;
	std::vector<Particle*> particles;
	std::vector<D3DLIGHT9*> lights;
	std::vector<Door*> doors;
	std::vector<Bullet*> bullets;
	std::vector<Item*> items;
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

	//////game functions and variables
	Player* player;
	
	//input stuff
	PxVec3 DoInput(float deltaTime);
	void HandlePlayerCollisions(PxVec3 moveDir);
	void LeftMouseClick();
	void RightMouseClick();
	std::vector<unsigned int> keys;
	int KeyPressed(int key);
	bool mouseLeftJustDown;
	bool mouseRightJustDown;

	//editor mode
	void UndoEditorAction();
	enum EditorAction{ThrowCube,Dynamic,Static,ModelWithCollision,None};
	std::vector<EditorAction> lastAction;
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

};

#endif