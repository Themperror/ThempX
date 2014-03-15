#include "../Headers/PHYSXENGINE.h"


PhysXEngine::PhysXEngine(ResourceManager* res)
{
	resources = res;
	
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAlloc, defaultError);

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale());
	PxInitExtensions(*gPhysicsSDK);

	// Create the scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

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





	/*if(!BakeMesh(resources->GetMeshFromVector(resources->GetMeshData("resources/models/cube2d.x"))))
	{
		std::cout << "failed baking mesh" << std::endl;

	}
	
	*/

	defaultMaterial = gPhysicsSDK->createMaterial(0.5f,0.5f,0.5f);

	LPD3DXMESH mesh = resources->GetMeshFromVector(resources->GetMeshData("resources/collision/testcollision.x"));
	BakeMesh(mesh,PxVec3(2,2,2));
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
	playerDesc.radius = 1;
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
	profiler = &physx::PxProfileZoneManager::createProfileZoneManager(mFoundation);
	profiler->createProfileZone("Test",PxProfileNames());
	//if(!mProfileZoneManager)
	for(unsigned int i = 0; i < 20 ; i++)
	{
		CreateCube(PxVec3(std::sin((float)i),i*3,0),PxVec3(0,45,0),PxVec3(1,1,1),500);
	}
	//CreateTriangleMesh()
}
void PhysXEngine::Update(float deltaTime)
{

}
bool PhysXEngine::BakeMesh(LPD3DXMESH mesh,PxVec3 scale)
{
	PxTriangleMeshDesc* description=new PxTriangleMeshDesc();
	
	description->setToDefault();
	description->points.count= mesh->GetNumVertices();
	description->triangles.count=mesh->GetNumFaces()*3;

	PxVec3* vertices = new PxVec3[mesh->GetNumVertices()];

	BYTE* vPtr = NULL;
	DWORD numVerts=mesh->GetNumVertices();
	DWORD fvf=mesh->GetFVF();
	DWORD vertSize=D3DXGetFVFVertexSize(fvf);
	mesh->LockVertexBuffer(0,(LPVOID*)&vPtr);

	for (DWORD i=0;i<numVerts;i++) 
	{
		D3DXVECTOR3 *ptr=(D3DXVECTOR3 *) vPtr;
		vertices[i] =PxVec3(ptr->x*scale.x,ptr->y*scale.y,ptr->z*scale.z);
		vPtr+=vertSize;
	}
	mesh->UnlockVertexBuffer();

	description->points.count = mesh->GetNumVertices();
	description->points.stride = sizeof(PxVec3);
	description->points.data = vertices;
	
	LPDIRECT3DINDEXBUFFER9 pIndBuf;
	mesh->GetIndexBuffer(&pIndBuf);

	
	DWORD numFaces=mesh->GetNumFaces();
	DWORD numIndices= numFaces*3;
	PxU16* indices = new PxU16[numIndices];
	//void* indices;
	void* pInd = NULL;
	pIndBuf->Lock(0,numIndices*sizeof(PxU16),(void**) &pInd,D3DLOCK_READONLY);
	PxU16* iptr=(PxU16*) pInd;
	//indices= malloc(sizeof(pInd));
	//memcpy(indices,pInd,sizeof(pInd));
	for (int i=0;i<numIndices;i++)
	{
		PxU16 INDEX = iptr[i];
		std::cout << INDEX << std::endl;
		indices[i] = iptr[i];
	}
	pIndBuf->Unlock();
	pIndBuf->Release();



	description->triangles.data = indices;
	description->triangles.count = numIndices;
	description->triangles.stride = 3*sizeof(PxU16);

	PxDefaultMemoryOutputStream stream;
	if(cooking->cookTriangleMesh(*description,stream))
	{
		std::cout << "Mesh has been baked successfully" << std::endl;
	}
	else
	{
		std::cout << "Mesh failed baking." << std::endl;
		return false;
	}

	
	PxTransform pose;
	pose.p = PxVec3(0,-20,0);
	pose.q= PxQuat(0,PxVec3(0,0,0));

	PxDefaultMemoryInputData istream(stream.getData(),stream.getSize());
	PxTriangleMesh *triangleMesh = gPhysicsSDK->createTriangleMesh(istream);

	PxRigidStatic* triangleMeshActor = gPhysicsSDK->createRigidStatic(pose);

	PxTriangleMeshGeometry geom = PxTriangleMeshGeometry(triangleMesh,PxMeshScale());

	PxShape *triangleMeshShape = triangleMeshActor->createShape(geom,*defaultMaterial,pose);

	gScene->addActor(*triangleMeshActor);
	statics.push_back(triangleMeshActor);
	return true;
}
void PhysXEngine::ReleaseAll()
{
	for(unsigned int i = 0; i < statics.size(); i++)
	{
		PxShape* ptr;
		statics.at(i)->getShapes(&ptr,256);
		PxShape* vPtr=(PxShape*) ptr;
		while(vPtr <= ptr+sizeof(ptr))
		{
			if(vPtr != NULL && vPtr->getActor() != NULL)
			{
				PxRigidActor* actor = vPtr->getActor();
				actor->detachShape(*vPtr);
				//vPtr->release();
			}
			vPtr+=sizeof(PxShape);
		}
		statics.at(i)->release();
	}
	for(unsigned int i = 0; i < dynamics.size(); i++)
	{
		PxShape* ptr;
		dynamics.at(i)->getShapes(&ptr,256);
		PxShape* vPtr=(PxShape*) ptr;
		while(vPtr < ptr)
		{
			if(vPtr != NULL && vPtr->getActor() != NULL)
			{
				PxRigidActor* actor = vPtr->getActor();
				actor->detachShape(*vPtr);
			//	vPtr->release();
			}
			vPtr+=sizeof(PxShape);
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
void PhysXEngine::DrawBoxes()
{
	for(unsigned int i = 0;i < dynamics.size(); i++)
	{
		PxTransform t = dynamics.at(i)->getGlobalPose();
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
	for(unsigned int i = 0; i < statics.size(); i++)
	{
		/*if(i>1)
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
			staticVisualCubes.at(i)->hasExternalWorldMatrix = true;
			staticVisualCubes.at(i)->eWorldMatrix = world;
			staticVisualCubes.at(i)->position = D3DXVECTOR3(pos.x,pos.y,pos.z);
		}
		staticVisualCubes.at(i)->doRender =true;
		staticVisualCubes.at(i)->Draw();	*/
	}
}
void PhysXEngine::CreateCube(PxVec3 position, PxVec3 rotation, PxVec3 scaling,float mass, bool isStatic)
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
		cubeActor->setAngularDamping(0.5f);
		cubeActor->setLinearDamping(0.5f);
		cubeActor->setMass(mass);
		gScene->addActor(*cubeActor);
		dynamics.push_back(cubeActor);
		dynamicVisualCubes.push_back(vCube);
	}
	else
	{
		PxRigidStatic *cubeActor = PxCreateStatic(*gPhysicsSDK,cubeTransform,cubeGeometry,*defaultMaterial);
		gScene->addActor(*cubeActor);
		statics.push_back(cubeActor);
		staticVisualCubes.push_back(vCube);
	}
}
void PhysXEngine::ThrowCube(PxVec3 position,PxVec3 force)
{
	PxReal cubeDensity = 10.0f;
	D3DXQUATERNION rotQuat;
	D3DXQuaternionRotationYawPitchRoll(&rotQuat,0,0,0);

	PxTransform cubeTransform(position);
	PxBoxGeometry cubeGeometry(PxVec3(0.5f,0.5f,0.5f));
	cubeTransform.p = position;
	cubeTransform.q = PxQuat(rotQuat.x,rotQuat.y,rotQuat.z,rotQuat.w);

	PxRigidDynamic *cubeActor = PxCreateDynamic(*gPhysicsSDK, cubeTransform, cubeGeometry, *defaultMaterial, cubeDensity);

	cubeActor->setAngularDamping(0.1f);
	cubeActor->setLinearDamping(0.02f);
	cubeActor->setMass(100);
	gScene->addActor(*cubeActor);
	dynamics.push_back(cubeActor);
	DebugCube* vCube = new DebugCube(D3DXVECTOR3(position.x,position.y,position.z),D3DXVECTOR3(0,0,0),-D3DXVECTOR3(0.5f,0.5f,0.5f),D3DXVECTOR3(0.5f,0.5f,0.5f),resources);
	dynamicVisualCubes.push_back(vCube);
	cubeActor->addForce(force,PxForceMode::eVELOCITY_CHANGE);
}
/*
SPEEngine::SPEEngine(ResourceManager* res)
{
	resources = res;
	InitApp();
}
void SPEEngine::InitShape(ISPEShape* pShape, LPD3DXMESH pMesh)
{
    BYTE *pVB;
    void *pIB;
    pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&pVB);
    pMesh->LockIndexBuffer (D3DLOCK_NOSYSLOCK, (void**)&pIB);
    if(pMesh->GetOptions() & D3DXMESH_32BIT)  // 32BIT index
    {
        pShape->Initialize (pVB, pMesh->GetNumBytesPerVertex (),
            (int*)pIB, pMesh->GetNumFaces ());
    }
    else  // 16BIT index
    {
        pShape->Initialize (pVB, pMesh->GetNumBytesPerVertex (),
            (WORD*)pIB, pMesh->GetNumFaces ());
    }
    pMesh->UnlockVertexBuffer ();
    pMesh->UnlockIndexBuffer ();
}
void SPEEngine::Create3DPhysicsObject(char* pathToMesh, SPEEngine::RigidData* data)
{
	RigidModel r;
	r.doRender = data->doRender;
	LPSPERIGIDBODY pBody;
	LPSPESHAPE pShape = pWorld->CreateShape();
	r.modelNr = resources->GetMeshData(pathToMesh);
	r.model = resources->GetModelStructFromVector(r.modelNr);
	ScaleMesh(r.model->mesh,data->scaleModel.x,data->scaleModel.y,data->scaleModel.z,NULL);

	//D3DXLoadMeshFromX (pathToMesh,NULL,resources->GetDevice(),NULL,NULL,NULL,NULL, &mesh);
	InitShape(pShape, r.model->mesh);  // initialize the shape
	pBody = pWorld->AddRigidBody (pShape); // add a rigid body to SPEWorld with this shape
	pBody->SetPosition(data->position); // set position of rigid body
	pBody->SetVelocity(data->velocity); // set velocity
	pBody->SetAngularVelocity(data->angularVelocity);

	pBody->SetBeStatic(data->isStatic);
	r.rigidbody = pBody;
	if(data->isStatic)
	{
		staticbodies.push_back(r);
	}
	else
	{
		rigidbodies.push_back(r);
	}
	shapes.push_back(pShape);
}
void SPEEngine::Create2DPhysicsObject(SPEEngine::RigidData* data)
{
	RigidModel r;
	r.doRender = data->doRender;
	LPSPERIGIDBODY pBody;
	LPSPESHAPE pShape = pWorld->CreateShape();
	r.modelNr = resources->GetMeshData("resources/models/cube2d.x");
	r.model = resources->GetModelStructFromVector(r.modelNr);
	ScaleMesh(r.model->mesh,data->scaleModel.x,data->scaleModel.y,data->scaleModel.z,NULL);

	InitShape(pShape, r.model->mesh);  // initialize the shape

	pBody = pWorld->AddRigidBody(pShape); // add a rigid body to SPEWorld with this shape
	pBody->SetPosition(data->position); // set position of rigid body
	pBody->SetVelocity(data->velocity); // set velocity
	pBody->SetAngularVelocity(data->angularVelocity);
	
	pBody->SetBeStatic(data->isStatic);
	r.rigidbody = pBody;
	if(data->isStatic)
	{
		staticbodies.push_back(r);
	}
	else
	{
		rigidbodies.push_back(r);
	}
	shapes.push_back(pShape);
}
HRESULT SPEEngine::ScaleMesh(ID3DXMesh *pMesh, float scaleX,float scaleY,float scaleZ, D3DXVECTOR3* offset)
{
	BYTE* ptr = NULL;
	D3DXVECTOR3 vOff;
	HRESULT result;

	// return failure if no mesh pointer set
	if (!pMesh)
		return D3DERR_INVALIDCALL;

	// select default or specified offset vector
	if (offset)
	{
		vOff=*offset;
	}
	else
	{
		vOff=D3DXVECTOR3(0.0f,0.0f,0.0f);
	}
	// get the face count
	DWORD numVerts=pMesh->GetNumVertices();

	// get the FVF flags
	DWORD fvf=pMesh->GetFVF();

	// calculate vertex size
	DWORD vertSize=D3DXGetFVFVertexSize(fvf);

	// lock the vertex buffer
	result = pMesh->LockVertexBuffer(0,(LPVOID*)&ptr);
	if(result != D3D_OK)
	{
		return result;
	}
	// loop through the vertices
	for (DWORD i=0;i<numVerts;i++) 
	{
		// get pointer to location
		D3DXVECTOR3 *vPtr=(D3DXVECTOR3 *) ptr;

		// scale the vertex
		*vPtr+=vOff;
		vPtr->x*=scaleX;
		vPtr->y*=scaleY;
		vPtr->z*=scaleZ;

		// increment pointer to next vertex
		ptr+=vertSize;
	}
	result = pMesh->UnlockVertexBuffer();
	if (result != D3D_OK)
	{
		return result;
	}
	return S_OK;
}
void SPEEngine::Release()
{
	for(unsigned int i = 0; i < rigidbodies.size(); i++)
	{
		pWorld->ReleaseRigidBody(rigidbodies.at(i).rigidbody);
	}
	for(unsigned int i = 0; i < staticbodies.size(); i++)
	{
		pWorld->ReleaseRigidBody(staticbodies.at(i).rigidbody);
	}
	for(unsigned int i = 0; i < shapes.size(); i++)
	{
		pWorld->ReleaseShape(shapes.at(i));
	}
	ReleaseSPEWorld( pWorld ); 
}
void SPEEngine::InitApp()
{
	pWorld = CreateSPEWorld();  // create a instance of physics world
    pWorld->SetGravity (SPEVector(0, -4.9f, 0)); // set the gravity
	pWorld->SetStepTime(0.00833f);
	pWorld->SetSolverPrecision(3);
	pWorld->SetMaxStepPerUpdate(3);
	pWorld->SetSolverCacheFactor(0.5f);
} 
void SPEEngine::OnFrameMove(float fElapsedTime )
{
    pWorld->Update(fElapsedTime);
} 
void SPEEngine::OnFrameRender( IDirect3DDevice9* m_pd3dDevice )
{
    D3DXMATRIX matWorld;
	for(unsigned int i = 0; i < rigidbodies.size(); i++)
	{
		if(rigidbodies.at(i).doRender == true)
		{
			rigidbodies.at(i).rigidbody->GetTransformMesh (&matWorld); // get the world matrix of a rigid body
			m_pd3dDevice->SetTransform (D3DTS_WORLD, &matWorld);

			ResourceManager::Model* m = resources->GetModelStructFromVector(rigidbodies.at(i).modelNr);
			if(m->mesh != NULL)
			{
				HRESULT result;
				resources->GetDevice()->SetFVF(m->mesh->GetFVF());
				for (DWORD x=0; x<m->numMaterials; x++)
				{
					if(m->meshTextures != NULL)
					{
						resources->GetDevice()->SetMaterial(&m->meshMaterials[x]);
						resources->GetDevice()->SetTexture(0,m->meshTextures[x]);
					}
					result = m->mesh->DrawSubset(x);
					if(result == D3DERR_INVALIDCALL)
					{
						std::cout << "Invalid Call (DrawSubset)" << std::endl;
					}
				}
			}
		}
	}
	for(unsigned int i = 0; i < staticbodies.size(); i++)
	{
		if(staticbodies.at(i).doRender == true)
		{
			staticbodies.at(i).rigidbody->GetTransformMesh (&matWorld); // get the world matrix of a rigid body
			m_pd3dDevice->SetTransform (D3DTS_WORLD, &matWorld);

			ResourceManager::Model* m = resources->GetModelStructFromVector(staticbodies.at(i).modelNr);
			if(m->mesh != NULL)
			{
				HRESULT result;
				resources->GetDevice()->SetFVF(m->mesh->GetFVF());
				for (DWORD x=0; x<m->numMaterials; x++)
				{
					if(m->meshTextures != NULL)
					{
						resources->GetDevice()->SetMaterial(&m->meshMaterials[x]);
						resources->GetDevice()->SetTexture(0,m->meshTextures[x]);
					}
					result = m->mesh->DrawSubset(x);
					if(result == D3DERR_INVALIDCALL)
					{
						std::cout << "Invalid Call (DrawSubset)" << std::endl;
					}
				}
			}
		}
	}
} 

*/