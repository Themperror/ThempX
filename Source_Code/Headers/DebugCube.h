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
	void ChangeTexture(char* path);
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	LPDIRECT3DVERTEXBUFFER9 FillVertices();
	HWND handleWindow;
	CollisionGeo* collision;

	short cubeIndices[36];


	inline void AddPosition(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
		if(collision != NULL)
		{
			collision->SetPosition(position);
		}
	}
	inline void AddRotation(float x, float y, float z)
	{
		rotation.x += x;
		rotation.y += y;
		rotation.z += z;
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
			collision->SetRotation(rotation);
		}
	}
};



#endif