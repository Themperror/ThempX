#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "ResourceManager.h"

struct Model
{
    LPD3DXMESH mesh;
	D3DXMATERIAL* d3dxMaterials;
	D3DMATERIAL9* meshMaterials;
	LPD3DXBUFFER materialBuffer;
	LPDIRECT3DTEXTURE9* meshTextures;
	DWORD numMaterials;
	char* meshPath;
};

class Object3D
{
	
public:
	Object3D(ResourceManager* resources, char* path);
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;

	inline D3DXVECTOR3 GetCenterPos()
	{
		return centerPos;
	}
	void DrawModel();
	HWND handleWindow;
	void SetPosition(float x,float y,float z);
	inline void SetPosition(D3DXVECTOR3 pos)
	{
		position = pos;

	}
	inline void SetPosition(D3DXVECTOR3* pos)
	{
		position.x = pos->x;
		position.y = pos->y;
		position.z = pos->z;
	}
	void SetScale(float x,float y,float z);
	inline void SetScale(D3DXVECTOR3 scale)
	{
		scaling = scale;
	}
	void SetRotation(float x,float y,float z);
	D3DXVECTOR3 GetBoundingBox();
	bool LoadModel(LPSTR pathName, ResourceManager* res);
	Model model;
	D3DXMATRIX worldMatrix;
	std::string objName;
	std::string tag;
private:
	void InitVars();
	
	LPDIRECT3DDEVICE9 p_Device;

	D3DXVECTOR3 centerPos;
	HRESULT result;
	
};
#endif