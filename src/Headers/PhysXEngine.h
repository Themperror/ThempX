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
	//player related
	float playerGravity;

//engine

	physx::PxFoundation* mFoundation;
	physx::PxPhysics* gPhysicsSDK;
	physx::PxControllerManager* cManager;
	physx::PxProfileZoneManager* profiler;
	PxCooking* cooking;
	physx::PxScene* gScene;
	physx::PxShape* shape;
	physx::PxController* player;
	PxRigidActor* CreateCube(PxVec3 position, PxVec3 rotation, PxVec3 scaling,float mass, bool isStatic = false);
	PxRigidActor* CreateSphereCapsule(PxReal radius, PxReal capHeight,PxVec3 position,float mass, bool isStatic, bool isKinematic);
	PxRigidActor* ThrowCube(PxVec3 position,PxVec3 force);
	void Update(float deltaTime);
	bool BakeMesh(LPD3DXMESH mesh,PxVec3 scale, bool flipNormals);
	bool RaycastAny(PxVec3 origin, PxVec3 nDir, float distance);
	PxRaycastBuffer RaycastSingle(PxVec3 origin, PxVec3 nDir, float distance);
	//PxProfileZoneManager* profiler;
	void DrawBoxes();
	void RemoveActor(PxRigidDynamic* actor);
	void RemoveActor(PxRigidStatic* actor);
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
			PxShape* ptr;
			dynamics.at(dynamics.size()-1)->getShapes(&ptr,256);
			PxShape* shapes;
			dynamics.at(dynamics.size()-1)->getShapes(&shapes,sizeof(PxShape));
			for(unsigned int i = 0; i < dynamics.at(i)->getNbShapes(); i++)
			{
				shapes[i].release();
			}
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
			PxShape* ptr;
			statics.at(statics.size()-1)->getShapes(&ptr,256);
			PxShape* shapes;
			statics.at(statics.size()-1)->getShapes(&shapes,sizeof(PxShape));
			for(unsigned int i = 0; i < statics.at(i)->getNbShapes(); i++)
			{
				shapes[i].release();
			}
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