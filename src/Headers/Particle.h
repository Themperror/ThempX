#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "Particle2D.h"
//#include <boost/random.hpp>
//#include <boost/random/lagged_fibonacci.hpp>
class Particle
{
public:
	Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* textureP,D3DXMATRIX* camView, D3DXVECTOR3 pos,unsigned int minParticles = 200,unsigned int maxParticles = 500, float minLife = 1, float maxLife = 3);
	Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* textureP,D3DXMATRIX* camView, D3DXVECTOR3 pos, bool burst = true, float life = 2, D3DXVECTOR3 dirMin = D3DXVECTOR3(0,0,0) ,D3DXVECTOR3 dirMax = D3DXVECTOR3(0,0,0));
	void Update(float deltaTime);
	void Draw(D3DXMATRIX* camView);
	void Release();

	inline void SetEllipsoid(D3DXVECTOR3 ellipse)
	{
		ellipsoid = ellipse;
	}
	inline void SetLifeTime(float min,float max)
	{
		maxLifeTime = max;
		minLifeTime = min;
	}
	inline bool GetBurstFinished()
	{
		return burstFinished;
	}
	inline void SetParticleCount(unsigned int min,unsigned int max)
	{
		maxParticlesAmount = max;
		minParticlesAmount = min;
	}
	inline void SetMovement(D3DXVECTOR3 min,D3DXVECTOR3 max)
	{
		minMovement = min;
		maxMovement = max;
	}
	inline void SetCreationSpeed(float speed)
	{
		creationSpeed = speed;
	}
	inline unsigned int GetSize()
	{
		return particles.size();
	}
private:	

	bool burstMode; //if the class only has a burst then dies;
	float currentWholeLife;
	float wholeLife; //the amount of time the particle class will live;
	bool burstFinished;

	double creationSpeed; //how fast particles will generate after reaching minimum value;
	D3DXVECTOR3 minMovement; //movement direction min
	D3DXVECTOR3 maxMovement;//movement direction min
	unsigned int maxParticlesAmount; //max amount of paticles;
	unsigned int minParticlesAmount; //min amount of particles;
	D3DXVECTOR3 position; //position;
	float maxLifeTime; //max lifetime of a particle;
	float minLifeTime; //min lifetime of a particle;
	D3DXVECTOR3 ellipsoid; //particle spawning radius

	HWND handleWindow;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	float maxLife;
	float updateTimer;
	ResourceManager* resources;
	char* texturePath;
	D3DMATERIAL9 mat;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<Particle2D*> particles;
	LPDIRECT3DDEVICE9 p_Device;
	D3DXMATRIX* cameraView;

	LPDIRECT3DVERTEXBUFFER9 FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos);
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	void RenderParticle(D3DXMATRIX* camView);
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