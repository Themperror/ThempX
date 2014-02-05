#ifndef _DEBUGCUBE_H_
#define _DEBUGCUBE_H_

#include "ResourceManager.h"

struct VertexPNT
{
	FLOAT X; 
	FLOAT Y; 
	FLOAT Z;
	FLOAT nX; 
	FLOAT nY; 
	FLOAT nZ;
	FLOAT U;
	FLOAT V;
};

class DebugCube
{
public:
	DebugCube(LPDIRECT3DDEVICE9 d3d_Device,D3DXVECTOR3 cubePosition,	D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LLFPosition, D3DXVECTOR3 URBPosition, ResourceManager* resources);
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 scaling;
	D3DXVECTOR3 LLFPos;
	D3DXVECTOR3 URBPos;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	D3DXMATERIAL d3dxMaterial;
	D3DMATERIAL9 meshMaterial;
	LPD3DXBUFFER materialBuffer;
	LPDIRECT3DTEXTURE9 texture;
	ResourceManager* resource;
	LPDIRECT3DDEVICE9 p_Device;
	void Release();
	void Draw();
	void ChangeTexture(char* path,boost::mutex* current);
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	LPDIRECT3DVERTEXBUFFER9 FillVertices();
	HWND handleWindow;
	CollisionGeo* collision;
	std::string objName;
	std::string tag;
	bool didCollide;

	short cubeIndices[36];

	inline bool HasCollision(boost::mutex* current)
	{
		current->lock();
		bool hasCol = (collision != NULL);
		current->unlock();
		return hasCol;
	}
	inline void SetCollide(bool val, boost::mutex* current) // only used for collision debugging
	{
		current->lock();
		didCollide = val;
		current->unlock();
	}
	inline bool DidCollide(boost::mutex* current)  // only used for collision debugging
	{
		current->lock();
		bool didCol = didCollide;
		current->unlock();
		return didCollide;
	}
	inline CollisionGeo* GetCollision()
	{
		return collision;
	}
	inline void SetCollision(void* col)
	{
		collision = collision;
	}

	//this function expects you to manually have set the variables before execution, failing in doing so will give in-accurate collisions
	inline bool HardUpdateCollisionGeo(boost::mutex* current) //safe version
	{
		current->lock();
		if(collision != NULL)
		{
			collision->SetAABB(&position,&rotation,&scaling);
			current->unlock();
			return true;
		}
		current->unlock();
		return false;
	}
	inline bool HardUpdateCollisionGeo() //unsafe version
	{
		if(collision != NULL)
		{
			collision->SetAABB(&position,&rotation,&scaling);
			return true;
		}
		return false;
	}
	inline bool SetPosRotScale(boost::mutex* current,D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale) //safe version
	{
		position = pos;
		rotation = rot;
		scaling = scale;
		if(collision != NULL)
		{
			collision->SetAABB(&pos,&rot,&scale);
			return true;
		}
		return false;
	}
	inline bool AddPositionAndRotation(boost::mutex* current,float x, float y, float z,float rx, float ry, float rz)
	{
		current->lock();
		position.x += x;
		position.y += y;
		position.z += z;
		rotation.x += rx;
		rotation.y += ry;
		rotation.z += rz;
		if(rotation.x > 360)
		{
			rotation.x = 0;
		}
		if(rotation.x < 0)
		{
			rotation.x = 360;
		}
		if(rotation.y > 360)
		{
			rotation.y = 0;
		}
		if(rotation.y < 0)
		{
			rotation.y =360;
		}
		if(rotation.z > 360)
		{
			rotation.z = 0;
		}
		if(rotation.z < 0)
		{
			rotation.z =360;
		}
		if(collision != NULL)
		{
			collision->SetAABB(&position,&rotation,&scaling);
			current->unlock();
			return true;
		}
		current->unlock();
		return false;
	}
};



#endif