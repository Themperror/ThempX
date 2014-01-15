#include "..\Headers\DebugCube.h"

DebugCube::DebugCube(LPDIRECT3DDEVICE9 d3d_Device, D3DXVECTOR3 cubePosition,D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LLFPosition, D3DXVECTOR3 URBPosition, ResourceManager* resources)
{
	p_Device = d3d_Device;
	position = cubePosition;
	rotation = cubeRotation;
	LLFPos = D3DXVECTOR3(LLFPosition.x,LLFPosition.y,LLFPosition.z);
	URBPos = D3DXVECTOR3(URBPosition.x,URBPosition.y,URBPosition.z);

	collision = NULL;
	vBuffer = NULL;
	iBuffer = NULL;
	texture = resources->GetTexture("Resources/Models/CubeRed.png");
	resource = resources;
	ZeroMemory(&meshMaterial,sizeof(meshMaterial));

	meshMaterial.Diffuse.a = 255; meshMaterial.Diffuse.r = 128; meshMaterial.Diffuse.g = 128; meshMaterial.Diffuse.b = 128;
	meshMaterial.Ambient.a = 255; meshMaterial.Ambient.r = 128; meshMaterial.Ambient.g = 128; meshMaterial.Ambient.b = 128;

	vBuffer= FillVertices();
	iBuffer = FillIndices();
}
void DebugCube::ChangeTexture(char* path)
{
	texture = resource->GetTexture(path);
}
void DebugCube::Release()
{
	if(iBuffer != NULL)
	{
		iBuffer->Release();
	}
	if(vBuffer != NULL)
	{
		vBuffer->Release();
	}
}
void DebugCube::Draw()
{
	HRESULT result;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX scalingMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXMATRIX translationMatrix;
	D3DXMATRIX RotScaleMatrix;

	D3DXMatrixTranslation(&translationMatrix,position.x,position.y,position.z);
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix,rotation.x*ToRadian,rotation.y*ToRadian,rotation.z*ToRadian);
	D3DXMatrixScaling(&scalingMatrix,1,1,1);

	D3DXMatrixMultiply(&RotScaleMatrix,&scalingMatrix,&rotationMatrix);
	D3DXMatrixMultiply(&worldMatrix,&RotScaleMatrix,&translationMatrix);

	p_Device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

	result = p_Device->SetIndices(iBuffer);
	result = p_Device->SetStreamSource(0, vBuffer, 0, sizeof(VertexPNT));
	result = p_Device->SetTransform(D3DTS_WORLD, &worldMatrix);
	if(texture != NULL)
	{
		result = p_Device->SetTexture(0,texture);
	}
	result = p_Device->SetMaterial(&meshMaterial);

	

	// draw the cube
	result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
	switch(result)
	{	
		//case D3D_OK:std::cout << "D3D OK" << std::endl;
			//break;
		case D3DERR_INVALIDCALL: std::cout << "Invalid Call" << std::endl;
			break;
		case D3DERR_CONFLICTINGRENDERSTATE: std::cout << "Conflicting Renderstate" << std::endl;
			break ;
		case D3DERR_DRIVERINVALIDCALL: std::cout <<"Driver invalid call"<< std::endl;
			break;
		case D3DERR_TOOMANYOPERATIONS : std::cout << "too many operations" << std::endl; 
			break;
	}
}

LPDIRECT3DVERTEXBUFFER9 DebugCube::FillVertices()
{	
	VertexPNT vertices[] =
	{
		{ LLFPos.x, LLFPos.y, URBPos.z,  0,0,1, 0.0f, 0.0f,},    // side 1
		{ URBPos.x, LLFPos.y, URBPos.z,  0,0,1, 0.0f, 1.0f,},
		{ LLFPos.x, URBPos.y, URBPos.z,  0,0,1, 1.0f, 0.0f,},
		{ URBPos.x, URBPos.y, URBPos.z,  0,0,1, 1.0f, 1.0f,},
										 
		{ LLFPos.x, LLFPos.y, LLFPos.z,  0,0,-1, 0.0f, 0.0f,},    // side 2
		{ LLFPos.x, URBPos.y, LLFPos.z,  0,0,-1, 0.0f, 1.0f,},
		{ URBPos.x, LLFPos.y, LLFPos.z,  0,0,-1, 1.0f, 0.0f,},
		{ URBPos.x, URBPos.y, LLFPos.z,  0,0,-1, 1.0f, 1.0f,},
									
		{ LLFPos.x, URBPos.y, LLFPos.z,  0,1,0, 0.0f, 0.0f,},    // side 3
		{ LLFPos.x, URBPos.y, URBPos.z,  0,1,0, 0.0f, 1.0f,},
		{ URBPos.x, URBPos.y, LLFPos.z,  0,1,0, 1.0f, 0.0f,},
		{ URBPos.x, URBPos.y, URBPos.z,  0,1,0, 1.0f, 1.0f,},
										 
		{ LLFPos.x, LLFPos.y, LLFPos.z,  0,-1,0, 0.0f, 0.0f,},    // side 4
		{ URBPos.x, LLFPos.y, LLFPos.z,  0,-1,0, 0.0f, 1.0f,},
		{ LLFPos.x, LLFPos.y, URBPos.z,  0,-1,0, 1.0f, 0.0f,},
		{ URBPos.x, LLFPos.y, URBPos.z,  0,-1,0, 1.0f, 1.0f,},
										  
		{ URBPos.x, LLFPos.y, LLFPos.z,  1,0,0, 0.0f, 0.0f,},    // side 5
		{ URBPos.x, URBPos.y, LLFPos.z,  1,0,0, 0.0f, 1.0f,},
		{ URBPos.x, LLFPos.y, URBPos.z,  1,0,0, 1.0f, 0.0f,},
		{ URBPos.x, URBPos.y, URBPos.z,  1,0,0, 1.0f, 1.0f,},
										
		{ LLFPos.x, LLFPos.y, LLFPos.z,  -1,0,0, 0.0f, 0.0f,},    // side 6
		{ LLFPos.x, LLFPos.y, URBPos.z,  -1,0,0, 0.0f, 1.0f,},
		{ LLFPos.x, URBPos.y, LLFPos.z,  -1,0,0, 1.0f, 0.0f,},
		{ LLFPos.x, URBPos.y, URBPos.z,  -1,0,0, 1.0f, 1.0f,},
	};
	LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; 

	HRESULT result = p_Device->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_XYZ  | D3DFVF_NORMAL |D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
	
	void* p_Vertices = NULL;
	result = p_dx_VertexBuffer->Lock(0, sizeof(vertices), (void**)&p_Vertices, 0);
	
	memcpy(p_Vertices, vertices, sizeof(vertices));
	p_dx_VertexBuffer->Unlock();

	return p_dx_VertexBuffer;
}

LPDIRECT3DINDEXBUFFER9 DebugCube::FillIndices() //zelfde als FillVertices, zie uitleg daar
{
	short indices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 5, 6,    // side 2
		6, 5, 7,
		8, 9, 10,    // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	VOID* pVoid = NULL;

	LPDIRECT3DINDEXBUFFER9 p_dx_IndexBuffer = NULL;
	HRESULT result = p_Device->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX16,D3DPOOL_MANAGED,  &p_dx_IndexBuffer, NULL);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Invalid Call while creating IndexBuffer","FillIndices()",MB_OK);
		return NULL;
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(handleWindow,"Out of Video Memory while creating IndexBuffer","FillIndices()",MB_OK);
		return NULL;
		break;
	case E_OUTOFMEMORY:
		MessageBox(handleWindow,"Out of Memory while creating IndexBuffer","FillIndices()",MB_OK);
		return NULL;
		break;
	}
	result = p_dx_IndexBuffer->Lock(0, 0, (void**)&pVoid, 0);

	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Error trying to lock","FillIndices()",MB_OK);
		return NULL;
		break;
	}

	memcpy(pVoid, indices, sizeof(indices));
	p_dx_IndexBuffer->Unlock();

	return p_dx_IndexBuffer;
}
