#include "../Headers/SPEENGINE.h"

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