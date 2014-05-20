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
		case false: std::cout << "Something went wrong with loading the model" << std::endl; break;
	}
}
bool Object3D::LoadModel(char* pathName, ResourceManager* res)
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
	if(D3DXMatrixIsIdentity(&worldMatrix))
	{
		D3DXMATRIX m_Scaling;
		D3DXMatrixScaling(&m_Scaling,scaling.x,scaling.y,scaling.z);

		D3DXMATRIX m_Rotation;
		D3DXMatrixRotationYawPitchRoll(&m_Rotation,rotation.x,rotation.y,rotation.z);

		D3DXMATRIX m_Translation;
		D3DXMatrixTranslation(&m_Translation,position.x,position.y,position.z);
	
		D3DXMATRIX m_rotScale;
		D3DXMatrixMultiply(&m_rotScale,&m_Rotation,&m_Scaling);

		D3DXMatrixMultiply(&worldMatrix,&m_rotScale,&m_Translation);
	}
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
	D3DXMatrixIdentity(&worldMatrix);
	
}
D3DXVECTOR3 Object3D::GetBoundingBox()
{
	BYTE* vPtr = NULL;
	HRESULT result;
	D3DXVECTOR3 inner = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 outer = D3DXVECTOR3(0,0,0);
	//model.mesh->OptimizeInplace(NULL,NULL,NULL,NULL,NULL);
	if(model.mesh == NULL)
	{
		return D3DXVECTOR3(0,0,0);
	}
	DWORD numVerts=model.mesh->GetNumVertices();
	DWORD fvf=model.mesh->GetFVF();

	result = model.mesh->LockVertexBuffer(D3DLOCK_READONLY,(LPVOID*)&vPtr);
	if(result != D3D_OK)
	{
		return D3DXVECTOR3(0,0,0);
	}

	float* vertices = new float[model.mesh->GetNumVertices()*3];
	ZeroMemory(vertices,sizeof(vertices));
	UINT vertSize=D3DXGetFVFVertexSize(fvf);
	for (DWORD i=0;i<numVerts;i++) 
	{	
		D3DXVECTOR3 *ptr=(D3DXVECTOR3 *) vPtr;  //vertices are retrieved correctly
        vertices[i*3] =		ptr->x * scaling.x;
        vertices[i*3+1] =	ptr->y * scaling.y;
        vertices[i*3+2] =	ptr->z * scaling.z;
		//std::cout << "vertex was: X: " << vertices[i*3] << " Y: " << vertices[i*3+1] << " Z: " << vertices[i*3+2] << std::endl;
		if(vertices[i*3] < inner.x)
		{
			inner.x = vertices[i*3];
		}
		if(vertices[i*3] > outer.x)
		{
			outer.x = vertices[i*3];
		}
		if(vertices[i*3+1] < inner.y)
		{
			inner.y = vertices[i*3+1];
		}
		if(vertices[i*3+1] > outer.y)
		{
			outer.y = vertices[i*3+1];
		}
		if(vertices[i*3+2] < inner.z)
		{
			inner.z = vertices[i*3+2];
		}
		if(vertices[i*3+2] > outer.z)
		{
			outer.z = vertices[i*3+2];
		}

		vPtr+=vertSize;
	}
	float xDistance = (abs(inner.x) + abs(outer.x))/2;
	float yDistance = (abs(inner.y) + abs(outer.y))/2;
	float zDistance = (abs(inner.z) + abs(outer.z))/2;

	result = model.mesh->UnlockVertexBuffer();
	//result = D3DXComputeBoundingBox((D3DXVECTOR3*)vertices,numVerts,sizeof(D3DXVECTOR3),&inner,&outer);

	if (result != D3D_OK)
	{
		return D3DXVECTOR3(0,0,0);
	}

	return D3DXVECTOR3(xDistance,yDistance,zDistance);
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
