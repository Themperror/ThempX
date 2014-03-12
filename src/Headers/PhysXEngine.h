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
	
	void CreateCube(PxVec3 position, PxVec3 rotation, PxVec3 scaling, bool isStatic = false);
	//PxProfileZoneManager* profiler;
	void DrawBoxes();
private:

	physx::PxDefaultAllocator defaultAlloc;
	physx::PxDefaultErrorCallback defaultError;
	PxMaterial* defaultMaterial;
	std::vector<PxRigidStatic*> statics;
	std::vector<PxRigidDynamic*> dynamics;
	std::vector<DebugCube*> visualCubes;

	
};
#endif