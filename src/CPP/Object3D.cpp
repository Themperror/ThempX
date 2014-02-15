#include "../Headers/Object3D.h"

void Object3D::InitVars()
{
	scaling.x = 1;
	scaling.y = 1;
	scaling.z = 1;
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;
}
Object3D::Object3D(ResourceManager* resources,char* path)
{
	InitVars();

	p_Device = resources->GetDevice();

	model.d3dxMaterials = NULL;
	model.materialBuffer = NULL;
    model.numMaterials = 0;
	model.meshPath = path;
	switch(LoadModel(path,resources))
	{
		case true: std::cout << "Model Loaded Succesfully" << std::endl; break;
		case false: std::cout << "Something went wrong with loading the model" << std::endl; break;
	}
}
bool Object3D::LoadModel(LPSTR pathName, ResourceManager* res)
{
	ResourceManager::Model* m = res->GetModelStructFromVector(res->GetMeshData(pathName));
	model.d3dxMaterials = m->d3dxMaterials;
	model.meshMaterials = m->meshMaterials;
	model.meshTextures = m->meshTextures;
	model.numMaterials = m->numMaterials;
	model.mesh = m->mesh;

	return true;
}
void Object3D::DrawModel()
{
	D3DXMatrixIdentity(&worldMatrix);

	D3DXMATRIX m_Scaling;
	D3DXMatrixScaling(&m_Scaling,scaling.x,scaling.y,scaling.z);

	D3DXMATRIX m_Rotation;
	D3DXMatrixRotationYawPitchRoll(&m_Rotation,rotation.x,rotation.y,rotation.z);

	D3DXMATRIX m_Translation;
	D3DXMatrixTranslation(&m_Translation,position.x,position.y,position.z);
	
	D3DXMATRIX m_rotScale;
	D3DXMatrixMultiply(&m_rotScale,&m_Rotation,&m_Scaling);

	D3DXMatrixMultiply(&worldMatrix,&m_rotScale,&m_Translation);
	p_Device->SetTransform(D3DTS_WORLD, &worldMatrix);
	if(model.mesh != NULL)
	{
		p_Device->SetFVF(model.mesh->GetFVF());
		for (DWORD i=0; i<model.numMaterials; i++)
		{
			if(model.meshTextures != NULL)
			{
				p_Device->SetMaterial(&model.meshMaterials[i]);
				p_Device->SetTexture(0,model.meshTextures[i]);
			}
			result = model.mesh->DrawSubset(i);
			if(result == D3DERR_INVALIDCALL)
			{
				std::cout << "Invalid Call (DrawSubset)" << std::endl;
			}
		}
	}
	
}
HRESULT Object3D::CalcBoundingBox(ID3DXMesh *pMesh,D3DXVECTOR3* inner,D3DXVECTOR3 *outer)
{
	void* ptr = NULL;

	if (!pMesh){return D3DERR_INVALIDCALL;}

	DWORD numVerts=pMesh->GetNumVertices();
	DWORD fvfSize=D3DXGetFVFVertexSize(pMesh->GetFVF());
	result = pMesh->LockVertexBuffer(0,&ptr);

	if(result != D3D_OK){return result;}

	result = D3DXComputeBoundingBox((D3DXVECTOR3 *) ptr,numVerts,fvfSize,inner,outer);
	result = pMesh->UnlockVertexBuffer();

	if (result != D3D_OK){return result;}

	delete ptr;
	return S_OK;
}
HRESULT Object3D::CalcBounds(ID3DXMesh *pMesh, D3DXVECTOR3 *vCenter, float *radius)
{
	void* ptr = NULL;
	if (!pMesh)
	{
		return D3DERR_INVALIDCALL;
	}

	DWORD numVerts=pMesh->GetNumVertices();

	DWORD fvfSize=D3DXGetFVFVertexSize(pMesh->GetFVF());

	result = pMesh->LockVertexBuffer(0,&ptr);
	if(result != D3D_OK)
	{
		return result;
	}
	result = D3DXComputeBoundingSphere((D3DXVECTOR3 *) ptr, numVerts, fvfSize, vCenter, radius);
	
	centerPos.x = vCenter->x;
	centerPos.y = vCenter->y;
	centerPos.z = vCenter->z;
	if (result != D3D_OK)
	{
		return result;
	}
	result = pMesh->UnlockVertexBuffer();
	if (result != D3D_OK)
	{
		return result;
	}
	delete ptr;
	return S_OK;
}


void Object3D::SetPosition(float x,float y,float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}
void Object3D::SetScale(float x,float y,float z)
{
	scaling.x = x;
	scaling.y = y;
	scaling.z = z;
}
void Object3D::SetRotation(float x,float y,float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}
HRESULT Object3D::NormalizeMesh(float scaleToX, float scaleToY, float scaleToZ, bool bCenter)
{
	D3DXVECTOR3 vCenter;
	float radius;
	result = CalcBounds(model.mesh,&vCenter,&radius);
	// calculate bounds of mesh
	if (result != D3D_OK)
	{
		return result;
	}

	// calculate scaling factor
	float scaleX=scaleToX/radius;
	float scaleY=scaleToY/radius;
	float scaleZ=scaleToZ/radius;

	// calculate offset if centering requested
	D3DXVECTOR3 vOff;
	if (bCenter) 
	{
		vOff=-vCenter;
	}
	else
	{
		vOff=D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	// scale and offset mesh
	return ScaleMesh(model.mesh,scaleX,scaleY,scaleZ,&vOff);
}
HRESULT Object3D::ScaleMesh(ID3DXMesh *pMesh, float scaleX,float scaleY,float scaleZ, D3DXVECTOR3* offset)
{
	BYTE* ptr = NULL;
	D3DXVECTOR3 vOff;

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

	// unlock the vertex buffer
	result = pMesh->UnlockVertexBuffer();
	if (result != D3D_OK)
	{
		return result;
	}
	// return success to caller
	delete ptr;	
	return S_OK;
}