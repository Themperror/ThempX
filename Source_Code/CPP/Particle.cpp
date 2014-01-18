#include "../Headers/Particle.h"

Particle::Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* texture,D3DXMATRIXA16* camView, D3DXVECTOR3 pos,int minParticles,int maxParticles, float minLife, float maxLife)
{
	resources = res;
	p_Device = device;
	texturePath = texture;
	cameraView = camView;
	position = pos;

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
	//CheckLifeTime();

}

void Particle::Draw()
{
	RenderParticle();
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
	p.obj = new Object2D(resources,p_Device,texturePath,cameraView,D3DXVECTOR2(-0.5f,-0.5f),D3DXVECTOR2(0.5f,0.5f));
	particles.push_back(p);
}
void Particle::DestroyParticle(int index)
{
	Part* p = &particles.at(index);
	p->obj->ReleaseResources();
	delete p;
	particles.erase (particles.begin()+index);
}
void Particle::MoveParticle(D3DXVECTOR3 min, D3DXVECTOR3 max,float deltaTime)											 
{
	for(unsigned int i = 0; i < GetSize();i++)
	{
		Object2D* obj = particles.at(i).obj;
		obj->position = AddVector3(&D3DXVECTOR3(RandomFloat(min.x,max.x)*deltaTime,RandomFloat(min.y,max.y)*deltaTime,RandomFloat(min.z,max.z)*deltaTime),&obj->position);
	}
}
void Particle::CheckLifeTime()
{
	for(unsigned int i = 0; i < GetSize();i++)
	{
		Part* obj = &particles.at(i);
		if(obj->currentLife > maxLifeTime)
		{
			obj->obj->ReleaseResources();
			delete obj;
			particles.erase(particles.begin()+i);
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