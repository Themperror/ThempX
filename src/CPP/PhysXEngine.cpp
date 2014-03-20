#include "../Headers/PHYSXENGINE.h"


PhysXEngine::PhysXEngine(ResourceManager* res)
{
	resources = res;
	playerGravity = 0;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAlloc, defaultError);

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale());
	PxInitExtensions(*gPhysicsSDK);

	// Create the scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -18.6f, 0.0f);

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if(!sceneDesc.filterShader)
	{
		sceneDesc.filterShader  = PxDefaultSimulationFilterShader;
	}

	gScene = gPhysicsSDK->createScene(sceneDesc);
	cooking = PxCreateCooking(PX_PHYSICS_VERSION,gPhysicsSDK->getFoundation(),PxCookingParams(PxTolerancesScale()));
	

#ifdef DEBUG

	if (!gPhysicsSDK->getPvdConnectionManager())
	{
		std::cout << "Warning: PhysX Visual Debugger not found running!\n";
		return;
	}

	const char* pvdHostIP = "127.0.0.1";
	int port = 5425;
	unsigned int timeout = 100;
	physx::PxVisualDebuggerConnectionFlags flags =
		physx::PxVisualDebuggerConnectionFlag::eDEBUG
		| physx::PxVisualDebuggerConnectionFlag::ePROFILE
		| physx::PxVisualDebuggerConnectionFlag::eMEMORY;
 
	// Create connection with PhysX Visual Debugger
	physx::debugger::comm::PvdConnection* conn = physx::PxVisualDebuggerExt::createConnection(
		gPhysicsSDK->getPvdConnectionManager(),
		pvdHostIP,
		port,
		timeout,
		flags);
  
	if (conn)
	{
		std::cout << "Connected to PhysX Visual Debugger!\n";
 
		gPhysicsSDK->getVisualDebugger()->setVisualizeConstraints(true);
		gPhysicsSDK->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
	}
#endif





	/*if(!BakeMesh(resources->GetMeshFromVector(resources->GetMeshData("resources/models/cube2d.x"))))
	{
		std::cout << "failed baking mesh" << std::endl;

	}
	
	*/

	defaultMaterial = gPhysicsSDK->createMaterial(0.5f,0.5f,0.5f);

	LPD3DXMESH mesh = resources->GetMeshFromVector(resources->GetMeshData("resources/collision/Level1CollisionMesh.x"));
	BakeMesh(mesh,PxVec3(2,2,-2),true);
	//if bakemesh succeeds, the level should automatically get it's collision
	//an check if it succeeds is currently in BakeMesh itself

	PxReal d = 0.0f;  
	PxTransform pose = PxTransform(PxVec3(0.0f, -13.6f, 0.0f),PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(pose);
	PxShape* shape = plane->createShape(PxPlaneGeometry(), *defaultMaterial);
	gScene->addActor(*plane);
	statics.push_back(plane);
	DebugCube* vCube = new DebugCube(D3DXVECTOR3(0.0f, -14, 0.0f),D3DXVECTOR3(0,0,0),-D3DXVECTOR3(500,0.1f,500),D3DXVECTOR3(500,0.1f,500),resources);
	vCube->ChangeTexture("Resources/Particles/Grid.png");
	staticVisualCubes.push_back(vCube);

	cManager = PxCreateControllerManager(*gScene);
	PxCapsuleControllerDesc playerDesc;
	playerDesc.setToDefault();
	playerDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
	playerDesc.contactOffset = 0.04f;
	playerDesc.density = 10;
	playerDesc.height = 4; // adjust this to roof height below;
	playerDesc.maxJumpHeight = 2;
	playerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	playerDesc.position = PxExtendedVec3(10,20,0);
	playerDesc.radius = 1.5f;
	playerDesc.scaleCoeff = 0.8f;
	playerDesc.stepOffset = 0.4f;
	playerDesc.upDirection = PxVec3(0,1,0);
	playerDesc.material = defaultMaterial;
	player = cManager->createController(playerDesc);

	/*physx::PxMaterial* mat;
	mat = gPhysicsSDK->createMaterial(1.0f,1.0f,1.0f);

	shape = gPhysicsSDK->createShape(physx::PxSphereGeometry(1.0f),&mat,1);
	PxRigidDynamic* sphereactor = gPhysicsSDK->createRigidDynamic(PxTransform(PxVec3(0,10,0)));
	sphereactor->attachShape(*shape);
	sphereactor->setLinearVelocity(PxVec3(0,5,0));

	PxRigidStatic* staticFloor = gPhysicsSDK->createRigidStatic(PxTransform(PxVec3(0,0,0)));
	physx::PxShape* plane;
	plane = gPhysicsSDK->createShape(PxPlaneGeometry(),&mat,1);
	staticFloor->attachShape(*plane);*/
	
	//actor aanmaken
	//actor.attachShape(shape);
	//profiler = &physx::PxProfileZoneManager::createProfileZoneManager(mFoundation);
	//profiler->createProfileZone("Test",PxProfileNames());
	//if(!mProfileZoneManager)
	for(unsigned int i = 0; i < 20 ; i++)
	{
		CreateCube(PxVec3(std::sin((float)i),i*3,0),PxVec3(0,0,0),PxVec3(3,3,3),50000);
	}
	//CreateTriangleMesh()
}
bool PhysXEngine::RaycastAny(PxVec3 origin, PxVec3 nDir, float distance)
{	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call is the boolean 'status'
	PxQueryFilterData fd; 
	fd.flags |= PxQueryFlag::eANY_HIT;
	PxRaycastBuffer hit;
	return gScene->raycast(origin, nDir, distance, hit, PxHitFlags(PxHitFlag::eDEFAULT), fd);
}
PxRaycastBuffer PhysXEngine::RaycastSingle(PxVec3 origin, PxVec3 nDir, float distance)
{	
	PxRaycastBuffer hit;
	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call is the closest hit, stored in the 'hit.block' structure
	bool status = gScene->raycast(origin, nDir, distance, hit);

	return hit;
}
void PhysXEngine::Update(float deltaTime)
{

}
bool PhysXEngine::BakeMesh(LPD3DXMESH mesh,PxVec3 scale, bool flipNormals)
{
	mesh->Optimize(0,NULL,NULL,NULL,NULL,&mesh);
	
	PxTriangleMeshDesc* description=new PxTriangleMeshDesc();
	
	description->setToDefault();

	float* vertices = new float[mesh->GetNumVertices()*3];
	ZeroMemory(vertices,sizeof(vertices));
	BYTE* vPtr = NULL;
	DWORD numVerts=mesh->GetNumVertices();
	DWORD fvf=mesh->GetFVF();
	UINT vertSize=D3DXGetFVFVertexSize(fvf);
	mesh->LockVertexBuffer(D3DLOCK_READONLY,(LPVOID*)&vPtr);
	for (DWORD i=0;i<numVerts;i++) 
	{	
		D3DXVECTOR3 *ptr=(D3DXVECTOR3 *) vPtr;  //vertices are retrieved correctly
        vertices[i*3] =		ptr->x * scale.x;
        vertices[i*3+1] =	ptr->y * scale.y;
        vertices[i*3+2] =	 ptr->z * scale.z;

		vPtr+=vertSize;
	}
/*	for(unsigned int i = 0 ; i < numVerts; i++)
	{
		std::cout<< "Vertex " <<i<<": "<<"  x: "<< vertices[i*3] << "  y: " <<vertices[i*3+1] << "  z: " << vertices[i*3+2] << std::endl;
	}*/
	mesh->UnlockVertexBuffer();
	
	description->points.data = vertices;
	description->points.count = numVerts;
	description->points.stride = sizeof(D3DXVECTOR3);
		
	DWORD numFaces=mesh->GetNumFaces();
	DWORD numIndices= numFaces*3;
	BYTE* indexBufferData;
	mesh->LockIndexBuffer(D3DLOCK_READONLY,(LPVOID*)&indexBufferData);

	PxU16* indices = new PxU16[numIndices];
	ZeroMemory(indices,sizeof(indices));
	for(unsigned int i=0; i < numIndices; i++)  
	{
		//((PxU16*)indexBufferData)[i]
		indices[i] = *(PxU16*)indexBufferData; //raw float* data 
		indexBufferData+=sizeof(PxU16); //increment pointer to next address
	}
	mesh->UnlockIndexBuffer();
	description->triangles.data = indices;
	description->triangles.count = numFaces;
	description->triangles.stride = 3*sizeof(PxU16);
	if(flipNormals)
	{
		description->flags = PxMeshFlag::e16_BIT_INDICES | PxMeshFlag::eFLIPNORMALS;
	}
	else
	{
		description->flags = PxMeshFlag::e16_BIT_INDICES;
	}

	PxDefaultMemoryOutputStream stream;
	if(cooking->cookTriangleMesh(*description,stream))
	{
		std::cout << "Mesh has been baked successfully" << std::endl;
	}
	else
	{
		std::cout << "Mesh failed baking." << std::endl;
		delete vertices;
		delete indices;
		return false;
	}
	
	PxTransform pose;
	pose.p = PxVec3(0,-10,0);
	pose.q= PxQuat(0,PxVec3(0,0,0));

	PxDefaultMemoryInputData istream(stream.getData(),stream.getSize());
	PxTriangleMesh *triangleMesh = gPhysicsSDK->createTriangleMesh(istream);

	PxRigidStatic* triangleMeshActor = gPhysicsSDK->createRigidStatic(pose);

	PxMeshScale meshScale(PxVec3(1,1,1), PxQuat(1.0f));
	PxTriangleMeshGeometry geom = PxTriangleMeshGeometry(triangleMesh,meshScale);

	PxShape *triangleMeshShape = triangleMeshActor->createShape(geom,*defaultMaterial,pose); // mesh is NULL

	gScene->addActor(*triangleMeshActor);
	statics.push_back(triangleMeshActor);

	delete vertices;
	delete indices;
	return true;
}
void PhysXEngine::ReleaseAll()
{
	for(unsigned int i = 0; i < statics.size(); i++)
	{
		PxShape* ptr;
		statics.at(i)->getShapes(&ptr,256);
		PxShape* shapes;
		statics.at(i)->getShapes(&shapes,sizeof(PxShape));
		for(unsigned int x = 0; x < statics.at(i)->getNbShapes(); x++)
		{
			statics.at(i)->detachShape(shapes[x]);
		}
		statics.at(i)->release();
	}
	for(unsigned int i = 0; i < dynamics.size(); i++)
	{
		PxShape* ptr;
		dynamics.at(i)->getShapes(&ptr,256);
		PxShape* shapes;
		dynamics.at(i)->getShapes(&shapes,sizeof(PxShape));
		for(unsigned int x = 0; x < dynamics.at(i)->getNbShapes(); x++)
		{
			dynamics.at(i)->detachShape(shapes[x]);
		}
		dynamics.at(i)->release();
	}
	for(unsigned int i = 0; i < dynamicVisualCubes.size(); i++)
	{
		dynamicVisualCubes.at(i)->Release();
	}
	for(unsigned int i = 0; i < staticVisualCubes.size(); i++)
	{
		staticVisualCubes.at(i)->Release();
	}
	//profiler->release();
	player->release();
	cManager->release();
	gScene->release();
	cooking->release();
	mFoundation->release();
}
void PhysXEngine::RemoveActor(PxRigidDynamic* actor)
{
	for(unsigned int i =0; i < dynamics.size(); i++)
	{
		if(actor == dynamics.at(i))
		{
			dynamics.erase(dynamics.begin()+i);
		}
	}
}
void PhysXEngine::RemoveActor(PxRigidStatic* actor)
{
	for(unsigned int i =0; i < statics.size(); i++)
	{
		if(actor == statics.at(i))
		{
			statics.erase(statics.begin()+i);
		}
	}
}
void PhysXEngine::DrawBoxes()
{
	std::vector<PxTransform> dynamicData;
	for(unsigned int i = 0;i < dynamics.size(); i++)
	{
		if(dynamics.at(i)->getName() != NULL)
		{
			if(std::strcmp(dynamics.at(i)->getName(),"Kinematic") != 0)
			{
				PxTransform t = dynamics.at(i)->getGlobalPose();
				dynamicData.push_back(t);
			}
		}
	}
	for(unsigned int i = 0; i< dynamicData.size();i++)
	{
		if(i < dynamicVisualCubes.size())
		{
			PxTransform t = dynamicData.at(i);
			PxVec3 pos = t.p;
			PxQuat rot = t.q;
			D3DXMATRIX rotation;
			D3DXMATRIX position;
			D3DXMATRIX world;
			D3DXMatrixRotationQuaternion(&rotation,&D3DXQUATERNION(t.q.x,t.q.y,t.q.z,t.q.w));
			D3DXMatrixTranslation(&position,t.p.x,t.p.y,t.p.z);
			D3DXMatrixMultiply(&world,&rotation,&position);
		
			dynamicVisualCubes.at(i)->hasExternalWorldMatrix = true;
			dynamicVisualCubes.at(i)->eWorldMatrix = world;
			dynamicVisualCubes.at(i)->position = D3DXVECTOR3(pos.x,pos.y,pos.z);
			dynamicVisualCubes.at(i)->doRender =true;
			dynamicVisualCubes.at(i)->Draw();
		}
	}/*
	for(unsigned int i = 0; i < statics.size(); i++)
	{
		if(i>0)
		{
			PxTransform t = statics.at(i)->getGlobalPose();
			PxVec3 pos = t.p;
			PxQuat rot = t.q;
			D3DXMATRIX rotation;
			D3DXMATRIX position;
			D3DXMATRIX world;
			D3DXMatrixRotationQuaternion(&rotation,&D3DXQUATERNION(t.q.x,t.q.y,t.q.z,t.q.w));
			D3DXMatrixTranslation(&position,t.p.x,t.p.y,t.p.z);
			D3DXMatrixMultiply(&world,&rotation,&position);
			if(i < staticVisualCubes.size())
			{
				staticVisualCubes.at(i)->hasExternalWorldMatrix = true;
				staticVisualCubes.at(i)->eWorldMatrix = world;
				staticVisualCubes.at(i)->position = D3DXVECTOR3(pos.x,pos.y,pos.z);
		
				staticVisualCubes.at(i)->doRender =true;
				staticVisualCubes.at(i)->Draw();	
			}
		}
	}*/
}
PxRigidActor* PhysXEngine::CreateCube(PxVec3 position, PxVec3 rotation, PxVec3 scaling,float mass, bool isStatic)
{
	PxReal cubeDensity = 100.0f;
	D3DXQUATERNION rotQuat;
	D3DXQuaternionRotationYawPitchRoll(&rotQuat,rotation.x*0.0174532925f,rotation.y*0.0174532925f,rotation.z*0.0174532925f);

	PxTransform cubeTransform(position);
	PxBoxGeometry cubeGeometry(PxVec3(abs((float)scaling.x),abs((float)scaling.y),abs((float)scaling.z)));
	cubeTransform.p = position;
	cubeTransform.q = PxQuat(rotQuat.x,rotQuat.y,rotQuat.z,rotQuat.w);
	DebugCube* vCube = new DebugCube(D3DXVECTOR3(position.x,position.y,position.z),D3DXVECTOR3(rotation.x,rotation.y,rotation.z),D3DXVECTOR3(-scaling.x,-scaling.y,-scaling.z),D3DXVECTOR3(scaling.x,scaling.y,scaling.z),resources);
		
	if(!isStatic)
	{
		PxRigidDynamic *cubeActor = PxCreateDynamic(*gPhysicsSDK, cubeTransform, cubeGeometry, *defaultMaterial, cubeDensity);
		cubeActor->setName("Cube");
		cubeActor->setAngularDamping(0.2f);
		cubeActor->setLinearDamping(0.2f);
		cubeActor->setMass(mass);
		gScene->addActor(*cubeActor);
		dynamics.push_back(cubeActor);
		dynamicVisualCubes.push_back(vCube);
		return cubeActor;
	}
	else
	{
		PxRigidStatic *cubeActor = PxCreateStatic(*gPhysicsSDK,cubeTransform,cubeGeometry,*defaultMaterial);
		cubeActor->setName("StaticCube");
		gScene->addActor(*cubeActor);
		statics.push_back(cubeActor);
		staticVisualCubes.push_back(vCube);
		return cubeActor;
	}
}
PxRigidActor* PhysXEngine::CreateSphereCapsule(PxReal radius, PxReal capHeight,PxVec3 position,float mass, bool isStatic, bool isKinematic)
{
	PxReal density = 100.0f;

	PxTransform transform(position);
	PxSphereGeometry geometry;
	geometry.radius = radius;
	PxCapsuleGeometry cGeometry;
	cGeometry.halfHeight = capHeight/2;
	cGeometry.radius = radius;
	transform.p = position;
		
	if(!isStatic)
	{
		PxRigidDynamic *actor;
		
		if(capHeight == 0)
		{
			actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *defaultMaterial, density);
		}
		else
		{
			actor = PxCreateDynamic(*gPhysicsSDK, transform, cGeometry, *defaultMaterial, density);
		}
		actor->setName("Kinematic");
		actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		actor->setAngularDamping(0.2f);
		actor->setLinearDamping(0.2f);
		actor->setMass(mass);
		gScene->addActor(*actor);
		dynamics.push_back(actor);
		return actor;
	}
	else
	{
		PxRigidStatic *actor;
		if(capHeight == 0)
		{
			actor = PxCreateStatic(*gPhysicsSDK, transform, geometry, *defaultMaterial);
		}
		else
		{
			actor = PxCreateStatic(*gPhysicsSDK, transform, cGeometry, *defaultMaterial);
		}
		gScene->addActor(*actor);
		statics.push_back(actor);
		return actor;
	}
}
PxRigidActor* PhysXEngine::ThrowCube(PxVec3 position,PxVec3 force)
{
	PxReal cubeDensity = 10.0f;
	D3DXQUATERNION rotQuat;
	D3DXQuaternionRotationYawPitchRoll(&rotQuat,0,0,0);

	PxTransform cubeTransform(position);
	PxBoxGeometry cubeGeometry(PxVec3(0.5f,0.5f,0.5f));
	cubeTransform.p = position;
	cubeTransform.q = PxQuat(rotQuat.x,rotQuat.y,rotQuat.z,rotQuat.w);

	PxRigidDynamic *cubeActor = PxCreateDynamic(*gPhysicsSDK, cubeTransform, cubeGeometry, *defaultMaterial, cubeDensity);
	cubeActor->setName("ThrownCube");
	cubeActor->setAngularDamping(0.1f);
	cubeActor->setLinearDamping(0.02f);
	cubeActor->setMass(100);
	gScene->addActor(*cubeActor);

	dynamics.push_back(cubeActor);
	DebugCube* vCube = new DebugCube(D3DXVECTOR3(position.x,position.y,position.z),D3DXVECTOR3(0,0,0),-D3DXVECTOR3(0.5f,0.5f,0.5f),D3DXVECTOR3(0.5f,0.5f,0.5f),resources);
	dynamicVisualCubes.push_back(vCube);
	cubeActor->addForce(force,PxForceMode::eVELOCITY_CHANGE);
	return cubeActor;
}