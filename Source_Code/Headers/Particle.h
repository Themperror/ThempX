#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "Object2D.h"
//#include <boost/random.hpp>
//#include <boost/random/lagged_fibonacci.hpp>
class Particle
{
public:
	Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* texture,D3DXMATRIXA16* camView, D3DXVECTOR3 pos,int minParticles,int maxParticles, float minLife, float maxLife);
	
	
	void Update(float deltaTime);
	void Draw();
	
	D3DXVECTOR3 position;

	float maxLifeTime;
	float minLifeTime;
	inline void SetLifeTime(float min,float max)
	{
		maxLifeTime = max;
		minLifeTime = min;
	}

	unsigned int maxParticlesAmount;
	unsigned int minParticlesAmount;
	inline void SetParticleCount(unsigned int min,unsigned int max)
	{
		maxParticlesAmount = max;
		minParticlesAmount = min;
	}

	D3DXVECTOR3 minMovement;
	D3DXVECTOR3 maxMovement;
	inline void SetMovement(D3DXVECTOR3 min,D3DXVECTOR3 max)
	{
		minMovement = min;
		maxMovement = max;
	}

	float creationSpeed;
	inline void SetCreationSpeed(float speed)
	{
		creationSpeed = speed;
	}
	inline unsigned int GetSize()
	{
		return particles.size();
	}
private:						   

	struct Part
	{
		float currentLife;
		Object2D* obj;
	};

	float updateTimer;
	ResourceManager* resources;
	char* texturePath;
	std::vector<Part> particles;
	LPDIRECT3DDEVICE9 p_Device;
	D3DXMATRIXA16* cameraView;

	void RenderParticle();
	void CreateParticle();
	void DestroyParticle(int index);
	void MoveParticle(D3DXVECTOR3 min, D3DXVECTOR3 max,float deltaTime);
	void CheckLifeTime();
	void AddLifeTime(float deltaTime);

	float RandomFloat(float min, float max)
	{
		if(min < max)
		{
			float random = ((float) rand()) / (float) RAND_MAX;
			float range = max - min;  
			return (random*range) + min;
		}
		else
		{
			float random = ((float) rand()) / (float) RAND_MAX;
			float range = min - max;  
			return (random*range) + max;
		}
	}

	D3DXVECTOR3 AddVector3(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return D3DXVECTOR3(a->x+b->x,a->y+b->y,a->z+b->z);
	}	


};		
#endif