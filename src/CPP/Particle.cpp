#include "../Headers/Particle.h"

Particle::Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* textureP,D3DXMATRIX* camView, D3DXVECTOR3 pos,unsigned int minParticles,unsigned int maxParticles, float minLife, float maxLife)
{
	resources = res;
	p_Device = device;
	minParticlesAmount = 0;
	maxParticlesAmount = 0;
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
Particle::Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* textureP,D3DXMATRIX* camView, D3DXVECTOR3 pos, bool burst, float life, D3DXVECTOR3 dirMin,D3DXVECTOR3 dirMax)
{
	resources = res;
	p_Device = device;
	minParticlesAmount = 0;
	maxParticlesAmount = 0;
	texturePath = textureP;
	cameraView = camView;
	position = pos;
	texture = res->GetTexture(texturePath);
	SetEllipsoid(D3DXVECTOR3(5,1,5));
	SetLifeTime(1,3);
	SetParticleCount(200,500);
	SetCreationSpeed(0.005f);
	updateTimer = 0;
	D3DMATERIAL9 m;
	ZeroMemory(&m,sizeof(m));
	m.Ambient = D3DXCOLOR(128,128,128,128);
	m.Diffuse = D3DXCOLOR(128,128,128,128);
	mat = m;
	vBuffer = FillCustomVertices(D3DXVECTOR2(-0.5f,-0.5f),D3DXVECTOR2(0.5f,0.5f));
	iBuffer = FillIndices();
	currentWholeLife = 0;
	burstFinished = false;
	burstMode = true;
	wholeLife = life;
	SetMovement(dirMin,dirMax);
}
void Particle::Release()
{
	for(unsigned int i = 0; i < particles.size();i++)
	{
		delete particles.at(i);
	}
	particles.clear();

	(vBuffer!= NULL ? vBuffer->Release() : NULL);
	(iBuffer != NULL ? iBuffer->Release() : NULL);
}
LPDIRECT3DVERTEXBUFFER9 Particle::FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos)
{
	Object2D::VertexPosNorTex triangleVerts[] = 
	{
		{D3DXVECTOR3(LLPos.x,URPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,0)},
		{D3DXVECTOR3(URPos.x,URPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,0)},
		{D3DXVECTOR3(LLPos.x,LLPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,1)},
		{D3DXVECTOR3(URPos.x,LLPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,1)},
	};
	LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL;

	HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(Object2D::VertexPosNorTex), 0, D3DFVF_XYZ |D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
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
	result = p_dx_VertexBuffer->Lock(0, 4*sizeof(Object2D::VertexPosNorTex), (void**)&p_Vertices, 0);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Error trying to lock","FillVertices()",MB_OK);
		return NULL;
		break;
	}
	memcpy(p_Vertices, triangleVerts, 4*sizeof(Object2D::VertexPosNorTex));
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
	if(burstMode)
	{
		currentWholeLife+=deltaTime;
		if(wholeLife < currentWholeLife)
		{
			burstFinished = true;
		}
	}


	unsigned int pSize = GetSize();
	if(pSize < minParticlesAmount)
	{
		for (unsigned int i = pSize; i < minParticlesAmount; i++)
		{
			CreateParticle();
		}
		pSize = GetSize();
	}
	if(pSize > minParticlesAmount && pSize < maxParticlesAmount && updateTimer > creationSpeed && !burstMode)
	{
		updateTimer = 0;
		CreateParticle();
	}
	MoveParticle(minMovement,maxMovement,deltaTime);
	AddLifeTime(deltaTime);
	CheckLifeTime();
}

void Particle::Draw(D3DXMATRIX* camView)
{
	RenderParticle(camView); //Render memory can be reduced by making 1 MATRIX for each necessary and pointing em to each particle for re-use
}
void Particle::RenderParticle(D3DXMATRIX* camView)
{
	D3DXMATRIX m_ViewScale;
	D3DXMATRIX m_ViewWorld;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX m_Scale;
	D3DXMATRIX m_Translation;
	for(unsigned int i = 0; i < GetSize();i++)
	{
		particles.at(i)->Draw(m_ViewScale,m_ViewWorld,worldMatrix,m_Scale,m_Translation,camView);
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
	p->SetMoveDir(D3DXVECTOR3(RandomFloat(minMovement.x,maxMovement.x), RandomFloat(minMovement.y,maxMovement.y),RandomFloat(minMovement.z,maxMovement.z)));
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
		obj->AddPosition(obj->GetMoveDir().x*deltaTime,obj->GetMoveDir().y*deltaTime,obj->GetMoveDir().z*deltaTime);
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