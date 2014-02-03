#include "../Headers/Particle.h"

Particle::Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* textureP,D3DXMATRIXA16* camView, D3DXVECTOR3 pos,int minParticles,int maxParticles, float minLife, float maxLife)
{
	resources = res;
	p_Device = device;
	texturePath = textureP;
	cameraView = camView;
	position = pos;
	texture = res->GetTexture(texturePath);
	SetEllipsoid(D3DXVECTOR3(5,1,5));
	SetLifeTime(minLife,maxLife);
	SetParticleCount(minParticles,maxParticles);
	SetCreationSpeed(0.005f);
	updateTimer = 0;
	D3DMATERIAL9 m;
	ZeroMemory(&m,sizeof(m));
	m.Ambient = D3DXCOLOR(128,128,128,128);
	m.Diffuse = D3DXCOLOR(128,128,128,128);
	mat = m;
	vBuffer = FillCustomVertices(D3DXVECTOR2(-0.5f,-0.5f),D3DXVECTOR2(0.5f,0.5f));
	iBuffer = FillIndices();

}
LPDIRECT3DVERTEXBUFFER9 Particle::FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos)
{
	VertexPosNorTex triangleVerts[] = 
	{
		{D3DXVECTOR3(LLPos.x,URPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,0)},
		{D3DXVECTOR3(URPos.x,URPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,0)},
		{D3DXVECTOR3(LLPos.x,LLPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,1)},
		{D3DXVECTOR3(URPos.x,LLPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,1)},
	};
	LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL;

	HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(VertexPosNorTex), 0, D3DFVF_XYZ |D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
	switch(result) 
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Invalid Call while creating VertexBuffer","FillVertices()",MB_OK);
		return NULL;
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(handleWindow,"Out of Video Memory while creating VertexBuffer","FillVertices()",MB_OK);
		return NULL;
		break;
	case E_OUTOFMEMORY:
		MessageBox(handleWindow,"Out of Memory while creating VertexBuffer","FillVertices()",MB_OK);
		return NULL;
		break;
	}
	void* p_Vertices;
	result = p_dx_VertexBuffer->Lock(0, 4*sizeof(VertexPosNorTex), (void**)&p_Vertices, 0);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Error trying to lock","FillVertices()",MB_OK);
		return NULL;
		break;
	}
	memcpy(p_Vertices, triangleVerts, 4*sizeof(VertexPosNorTex));
	p_dx_VertexBuffer->Unlock();

	return p_dx_VertexBuffer;
	
	
}

LPDIRECT3DINDEXBUFFER9 Particle::FillIndices()
{
	short s_Indices[4];
	s_Indices[0]=0;
	s_Indices[1]=1;
	s_Indices[2]=2;
	s_Indices[3]=3;


	LPDIRECT3DINDEXBUFFER9 p_dx_IndexBuffer = NULL;
	HRESULT result = p_Device->CreateIndexBuffer(4*sizeof(short), D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&p_dx_IndexBuffer,NULL);
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

	void* p_Indices;
	result = p_dx_IndexBuffer->Lock(0, 4*sizeof(short), (void**)&p_Indices, 0);

	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Error trying to lock","FillIndices()",MB_OK);
		return NULL;
		break;
	}
	memcpy(p_Indices, s_Indices, 4*sizeof(short));
	p_dx_IndexBuffer->Unlock();

	return p_dx_IndexBuffer;
}
void Particle::Update(float deltaTime)
{
	updateTimer+=deltaTime;
	unsigned int pSize = GetSize();
	if(pSize < minParticlesAmount)
	{
		for (unsigned int i = pSize; i < minParticlesAmount; i++)
		{
			CreateParticle();
		}
		pSize = GetSize();
	}
	if(pSize > minParticlesAmount && pSize < maxParticlesAmount && updateTimer > creationSpeed)
	{
		updateTimer = 0;
		CreateParticle();
	}
	MoveParticle(minMovement,maxMovement,deltaTime);
	AddLifeTime(deltaTime);
	CheckLifeTime();
}

void Particle::Draw()
{
	RenderParticle(); //Render memory can be reduced by making 1 MATRIX for each necessary and pointing em to each particle for re-use
}
void Particle::RenderParticle()
{
	D3DXMATRIX m_ViewScale;
	D3DXMATRIX m_ViewWorld;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX m_Scale;
	D3DXMATRIX m_Translation;
	for(unsigned int i = 0; i < GetSize();i++)
	{
		particles.at(i)->Draw(m_ViewScale,m_ViewWorld,worldMatrix,m_Scale,m_Translation);
	}
}
void Particle::CreateParticle()
{
	Particle2D* p = new Particle2D(p_Device,cameraView,vBuffer,iBuffer);
	p->SetTexture(texture);
	p->SetMaterial(&mat);
	p->SetLife(0);
	p->SetMaxLife(RandomFloat(minLifeTime,maxLifeTime));
	p->SetPosition((rand()%2==TRUE ? RandomFloat(0,ellipsoid.x) : -RandomFloat(0,ellipsoid.x)) + position.x,(rand()%2==TRUE ? RandomFloat(0,ellipsoid.y) : -RandomFloat(0,ellipsoid.y)) + position.y,(rand()%2==TRUE ? RandomFloat(0,ellipsoid.z) : -RandomFloat(0,ellipsoid.z)) + position.z);
	particles.push_back(p);
}
void Particle::DestroyParticle(int index)
{
	Particle2D* p = particles.at(index);
	delete p;
	particles.erase (particles.begin()+index);
}
void Particle::MoveParticle(D3DXVECTOR3 min, D3DXVECTOR3 max,float deltaTime)											 
{
	for(unsigned int i = 0; i < GetSize();i++)
	{
		Particle2D* obj = particles.at(i);
		obj->AddPosition((float)RandomFloat(min.x,max.x)*deltaTime, (float)RandomFloat(min.y,max.y)*deltaTime, (float)RandomFloat(min.z,max.z)*deltaTime);
	}
}
void Particle::CheckLifeTime()
{
	for(unsigned int i = 0; i < GetSize();i++)
	{								  
		if(particles.at(i)->GetLife() > particles.at(i)->GetMaxLife())
		{
			DestroyParticle(i);
		}
	}
}
void Particle::AddLifeTime(float deltaTime)
{
	for(unsigned int i = 0; i < GetSize();i++)
	{
		particles.at(i)->AddLife(deltaTime);
	}
}