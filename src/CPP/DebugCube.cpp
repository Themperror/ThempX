#include "..\Headers\DebugCube.h"

DebugCube::DebugCube(D3DXVECTOR3 cubePosition,D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LLFPosition, D3DXVECTOR3 URBPosition, ResourceManager* resources)
{
	doRender = false;
	didCollide = false;
	vBuffer = NULL;
	iBuffer = NULL;
	resource = NULL;

	D3DXMatrixIdentity(&eWorldMatrix);
	hasExternalWorldMatrix = false;
	p_Device = resources->GetDevice();
	position = cubePosition;
	rotation = cubeRotation;
	scaling = D3DXVECTOR3(1,1,1);
	LLFPos = D3DXVECTOR3(LLFPosition.x,LLFPosition.y,LLFPosition.z);
	URBPos = D3DXVECTOR3(URBPosition.x,URBPosition.y,URBPosition.z);
	resource = resources;
	texture = resources->GetTexture("Resources/Models/Box.png");
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
	if(doRender)
	{
		HRESULT result;
		D3DXMATRIX worldMatrix;
		D3DXMATRIX scalingMatrix;
		D3DXMATRIX rotationMatrix;
		D3DXMATRIX translationMatrix;
		D3DXMATRIX RotScaleMatrix;

		
		p_Device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

		result = p_Device->SetIndices(iBuffer);
		result = p_Device->SetStreamSource(0, vBuffer, 0, sizeof(VertexPNT));

		if(!hasExternalWorldMatrix)
		{
			D3DXMatrixTranslation(&translationMatrix,position.x,position.y,position.z);
			D3DXMatrixRotationYawPitchRoll(&rotationMatrix,rotation.x*ToRadian,rotation.y*ToRadian,rotation.z*ToRadian);
			D3DXMatrixScaling(&scalingMatrix,scaling.x,scaling.y,scaling.z);

			D3DXMatrixMultiply(&RotScaleMatrix,&scalingMatrix,&rotationMatrix);
			D3DXMatrixMultiply(&worldMatrix,&RotScaleMatrix,&translationMatrix);
			
			result = p_Device->SetTransform(D3DTS_WORLD, &worldMatrix);
		}
		else
		{
			result = p_Device->SetTransform(D3DTS_WORLD, &eWorldMatrix);	
		}
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

LPDIRECT3DINDEXBUFFER9 DebugCube::FillIndices()
{
	cubeIndices[0]	 = 0;
	cubeIndices[1]	 = 1;
	cubeIndices[2]	 = 2;
	cubeIndices[3]	 = 2;
	cubeIndices[4]	 = 1;
	cubeIndices[5]	 = 3;
	cubeIndices[6]	 = 4;
	cubeIndices[7]	 = 5;
	cubeIndices[8]	 = 6;
	cubeIndices[9]	 = 6;
	cubeIndices[10]	 = 5;
	cubeIndices[11]	 = 7;
	cubeIndices[12]	 = 8;
	cubeIndices[13]	 = 9;
	cubeIndices[14]	 = 10;
	cubeIndices[15]	 = 10;
	cubeIndices[16]	 = 9;
	cubeIndices[17]	 = 11;
	cubeIndices[18]	 = 12;
	cubeIndices[19]	 = 13;
	cubeIndices[20]	 = 14;
	cubeIndices[21]	 = 14;
	cubeIndices[22]	 = 13;
	cubeIndices[23]	 = 15;
	cubeIndices[24]	 = 16;
	cubeIndices[25]	 = 17;
	cubeIndices[26]	 = 18;
	cubeIndices[27]	 = 18;
	cubeIndices[28]	 = 17;
	cubeIndices[29]	 = 19;
	cubeIndices[30]	 = 20;
	cubeIndices[31]	 = 21;
	cubeIndices[32]	 = 22;
	cubeIndices[33]	 = 22;
	cubeIndices[34]	 = 21;
	cubeIndices[35]	 = 23;

	VOID* pVoid = NULL;

	LPDIRECT3DINDEXBUFFER9 p_dx_IndexBuffer = NULL;
	HRESULT result = p_Device->CreateIndexBuffer(sizeof(cubeIndices), 0, D3DFMT_INDEX16,D3DPOOL_MANAGED,  &p_dx_IndexBuffer, NULL);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Invalid Call while creating IndexBuffer","DEBUGCUBE FillIndices()",MB_OK);
		return NULL;
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(handleWindow,"Out of Video Memory while creating IndexBuffer","DEBUGCUBE FillIndices()",MB_OK);
		return NULL;
		break;
	case E_OUTOFMEMORY:
		MessageBox(handleWindow,"Out of Memory while creating IndexBuffer","DEBUGCUBE FillIndices()",MB_OK);
		return NULL;
		break;
	}
	result = p_dx_IndexBuffer->Lock(0, 0, (void**)&pVoid, 0);

	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Error trying to lock","DEBUGCUBE FillIndices()",MB_OK);
		return NULL;
		break;
	}

	memcpy(pVoid, cubeIndices, sizeof(cubeIndices));
	p_dx_IndexBuffer->Unlock();

	return p_dx_IndexBuffer;
}
