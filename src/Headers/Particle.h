#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "Particle2D.h"
//#include <boost/random.hpp>
//#include <boost/random/lagged_fibonacci.hpp>
class Particle
{
public:
	Particle(ResourceManager* res,LPDIRECT3DDEVICE9 device,char* textureP,D3DXMATRIXA16* camView, D3DXVECTOR3 pos,int minParticles,int maxParticles, double minLife, double maxLife);
	
	void Update(double deltaTime);
	void Draw();

	inline void SetEllipsoid(D3DXVECTOR3 ellipse)
	{
		ellipsoid = ellipse;
	}
	inline void SetLifeTime(double min,double max)
	{
		maxLifeTime = max;
		minLifeTime = min;
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
	inline void SetCreationSpeed(double speed)
	{
		creationSpeed = speed;
	}
	inline unsigned int GetSize()
	{
		return particles.size();
	}
private:	


	double creationSpeed;
	D3DXVECTOR3 minMovement;
	D3DXVECTOR3 maxMovement;
	unsigned int maxParticlesAmount;
	unsigned int minParticlesAmount;
	D3DXVECTOR3 position;
	double maxLifeTime;
	double minLifeTime;
	D3DXVECTOR3 ellipsoid;

	HWND handleWindow;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	double maxLife;
	double updateTimer;
	ResourceManager* resources;
	char* texturePath;
	D3DMATERIAL9 mat;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<Particle2D*> particles;
	LPDIRECT3DDEVICE9 p_Device;
	D3DXMATRIXA16* cameraView;

	LPDIRECT3DVERTEXBUFFER9 FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos);
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	void RenderParticle();
	void CreateParticle();
	void DestroyParticle(int index);
	void MoveParticle(D3DXVECTOR3 min, D3DXVECTOR3 max,double deltaTime);
	void CheckLifeTime();
	void AddLifeTime(double deltaTime);

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