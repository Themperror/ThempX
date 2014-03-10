#ifndef _PHYSXENGINE_H_
#define _PHYSXENGINE_H_

#include <PxPhysicsAPI.h>
#include <pvd\PxVisualDebugger.h>
#include "ResourceManager.h"
#include "DebugCube.h"

#ifdef DEBUG
#pragma comment(lib, "PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")
#endif
#ifndef DEBUG
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#endif


using namespace physx;

class PhysXEngine
{
public:

	PhysXEngine(ResourceManager* res);
	void ReleaseAll();
	ResourceManager* resources;

	physx::PxFoundation* mFoundation;
	physx::PxPhysics* gPhysicsSDK;
	physx::PxCooking* mCooking;
	physx::PxScene* gScene;
	physx::PxShape* shape;
	physx::PxProfileZoneManager* profiler;
	
	void CreateCube();
	//PxProfileZoneManager* profiler;
	void DrawBoxes();
private:

	physx::PxDefaultAllocator defaultAlloc;
	physx::PxDefaultErrorCallback defaultError;
	PxMaterial* defaultMaterial;
	std::vector<PxRigidStatic*> statics;
	std::vector<PxRigidDynamic*> dynamics;
	std::vector<DebugCube*> visualCubes;
	PxTransform positionTest;
	/*
	struct RigidData
	{
		SPEVector scaleModel;
		SPEVector velocity;
		SPEVector angularVelocity;
		SPEVector position;
		bool isStatic;
		bool doRender;
		float mass;
		float density;
		void Nullify()
		{
			scaleModel =		SPEVector(1.0f,1.0f,1.0f);
			velocity =			SPEVector(0.0f,0.0f,0.0f);
			angularVelocity =	SPEVector(0.0f,0.0f,0.0f);
			position =			SPEVector(0.0f,0.0f,0.0f);
			isStatic = false;
			doRender = true;
			mass = 1000.0f;
			density = 1000.0f;
		}
	};
	struct RigidModel
	{
		bool doRender;
		ResourceManager::Model* model;
		int modelNr;
		LPSPERIGIDBODY rigidbody;
	};
	SPEEngine(ResourceManager* res);

	void Create3DPhysicsObject(char* pathToMesh, SPEEngine::RigidData* data);
	void Create2DPhysicsObject(SPEEngine::RigidData* data);

	void Release();
	void InitShape(ISPEShape* pShape, ID3DXMesh* pMesh);

	void InitApp();
	void OnFrameMove(float fElapsedTime);
	void OnFrameRender( IDirect3DDevice9* m_pd3dDevice );


	inline LPSPERIGIDBODY GetLastRigidBody()
	{
		return rigidbodies.at(rigidbodies.size()-1).rigidbody;
	}
	inline LPSPERIGIDBODY GetLastStaticBody()
	{
		return staticbodies.at(staticbodies.size()-1).rigidbody;
	}

private:
	HRESULT ScaleMesh(ID3DXMesh *pMesh, float scaleX,float scaleY,float scaleZ, D3DXVECTOR3* offset);
	ResourceManager* resources;
	
	LPSPEWORLD         pWorld; // interface of physics world

	//for cleanup
	std::vector<RigidModel> rigidbodies;
	std::vector<RigidModel> staticbodies;
	std::vector<LPSPESHAPE> shapes;

	*/
};
#endif