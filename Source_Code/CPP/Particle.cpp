#include "../Headers/Particle.h"

Particle::Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* texture,D3DXMATRIXA16* camView, D3DXVECTOR3 pos,int minParticles,int maxParticles, float minLife, float maxLife)
{
	resources = res;
	p_Device = device;
	texturePath = texture;
	cameraView = camView;
	position = pos;

	SetEllipsoid(D3DXVECTOR3(5,1,5));
	SetLifeTime(minLife,maxLife);
	SetParticleCount(minParticles,maxParticles);
	SetCreationSpeed(0.005f);
	updateTimer = 0;
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
	for(unsigned int i = 0; i < GetSize();i++)
	{
		Object2D* obj = particles.at(i).obj;
		obj->Draw();
	}
}
void Particle::CreateParticle()
{
	Part p;
	p.currentLife = 0;
	p.maxLife = RandomFloat(minLifeTime,maxLifeTime);
	p.obj = new Object2D(resources,p_Device,texturePath,cameraView,D3DXVECTOR2(-0.5f,-0.5f),D3DXVECTOR2(0.5f,0.5f));
	
	int i = rand();
	p.obj->position.x = (i%2==TRUE ? RandomFloat(0,ellipsoid.x) : -RandomFloat(0,ellipsoid.x)) + position.x;
	i = rand();
	p.obj->position.y = (i%2==TRUE ? RandomFloat(0,ellipsoid.y) : -RandomFloat(0,ellipsoid.y)) + position.y;
	i = rand();
	p.obj->position.z = (i%2==TRUE ? RandomFloat(0,ellipsoid.z) : -RandomFloat(0,ellipsoid.z)) + position.z;

	particles.push_back(p);
}
void Particle::DestroyParticle(int index)
{
	Part* p = &particles.at(index);
	p->obj->ReleaseResources();
	p = NULL;
	particles.erase (particles.begin()+index);
	//std::cout << particles.size() << std::endl;
}
void Particle::MoveParticle(D3DXVECTOR3 min, D3DXVECTOR3 max,float deltaTime)											 
{
	for(unsigned int i = 0; i < GetSize();i++)
	{
		Object2D* obj = particles.at(i).obj;
		obj->position.x += RandomFloat(min.x,max.x)*deltaTime;
		obj->position.y += RandomFloat(min.y,max.y)*deltaTime;
		obj->position.z += RandomFloat(min.z,max.z)*deltaTime;
			
	}
}
void Particle::CheckLifeTime()
{
	for(unsigned int i = 0; i < GetSize();i++)
	{								  
		if(particles.at(i).currentLife > particles.at(i).maxLife)
		{
			DestroyParticle(i);
		}
	}
}
void Particle::AddLifeTime(float deltaTime)
{
	for(unsigned int i = 0; i < GetSize();i++)
	{
		Part* obj = &particles.at(i);
		obj->currentLife += deltaTime;
	}
}