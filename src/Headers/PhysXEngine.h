#ifndef _PHYSXENGINE_H_
#define _PHYSXENGINE_H_

#include <PxPhysicsAPI.h>
#include <pvd\PxVisualDebugger.h>
#include <PxToolkit.h>
#include "ResourceManager.h"
#include "DebugCube.h"

#define DEBUG   //remove line for release


#ifdef DEBUG
#pragma comment(lib, "PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PhysX3CookingCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicCHECKED_x86.lib")

#pragma comment(lib, "PhysXProfileSDKCHECKED.lib")

#pragma comment(lib, "PhysXVisualDebuggerSDKCHECKED.lib")

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
	physx::PxControllerManager* cManager;
	physx::PxProfileZoneManager* profiler;
	PxCooking* cooking;
	physx::PxScene* gScene;
	physx::PxShape* shape;
	physx::PxController* player;
	void CreateCube(PxVec3 position, PxVec3 rotation, PxVec3 scaling,float mass, bool isStatic = false);
	void ThrowCube(PxVec3 position,PxVec3 force);
	void Update(float deltaTime);
	bool BakeMesh(LPD3DXMESH mesh,PxVec3 scale);
	//PxProfileZoneManager* profiler;
	void DrawBoxes();
	inline PxRigidStatic* GetLastStatic()
	{
		if(statics.size() > 0)
		{
			return statics.at(statics.size()-1);
		}
		else return 0;
	}
	inline PxRigidDynamic* GetLastDynamic()
	{
		if(statics.size() > 0)
		{
			return dynamics.at(dynamics.size()-1);
		}
		else return 0;
	}
	inline void DeleteLastDynamic()
	{
		if(dynamics.size() > 0)
		{
			dynamics.at(dynamics.size()-1)->release();
			dynamics.erase(dynamics.end()-1);
			dynamicVisualCubes.at(dynamicVisualCubes.size()-1)->Release();
			dynamicVisualCubes.erase(dynamicVisualCubes.end()-1);
		}
	}
	inline void DeleteLastStatic()
	{
		if(statics.size() > 0)
		{
			statics.at(statics.size()-1)->release();
			statics.erase(statics.end()-1);
			staticVisualCubes.at(staticVisualCubes.size()-1)->Release();
			staticVisualCubes.erase(staticVisualCubes.end()-1);
		}
	}
private:

	physx::PxDefaultAllocator defaultAlloc;
	physx::PxDefaultErrorCallback defaultError;
	PxMaterial* defaultMaterial;
	std::vector<PxRigidStatic*> statics;
	std::vector<PxRigidDynamic*> dynamics;
	std::vector<DebugCube*> staticVisualCubes;
	std::vector<DebugCube*> dynamicVisualCubes;

	
};
#endif