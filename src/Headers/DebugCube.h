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
	DebugCube(D3DXVECTOR3 cubePosition,	D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LLFPosition, D3DXVECTOR3 URBPosition, ResourceManager* resources);
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
	bool doRender;
	void Release();
	void Draw();
	void ChangeTexture(char* path,boost::mutex* current);
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	LPDIRECT3DVERTEXBUFFER9 FillVertices();
	HWND handleWindow;
	std::string objName;
	std::string tag;
	bool didCollide;

	short cubeIndices[36];

	inline bool SetPosRotScale(boost::mutex* current,D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale) //safe version
	{	
		current->lock();
		position = pos;
		rotation = rot;
		scaling = scale;
		current->unlock();
		return true;
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
		current->unlock();
		return true;
	}
};



#endif