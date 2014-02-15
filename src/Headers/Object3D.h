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
	void SetScale(float x,float y,float z);
	void SetRotation(float x,float y,float z);

	bool LoadModel(LPSTR pathName, ResourceManager* res);
	Model model;
	D3DXMATRIX worldMatrix;
	std::string objName;
	std::string tag;
private:
	void InitVars();
	
	HRESULT CalcBounds(ID3DXMesh* mesh, D3DXVECTOR3* vCenter, float* radius);
	HRESULT Object3D::CalcBoundingBox(ID3DXMesh *pMesh,D3DXVECTOR3* inner,D3DXVECTOR3 *outer);
	HRESULT ScaleMesh(ID3DXMesh *pMesh, float scaleX,float scaleY,float scaleZ, D3DXVECTOR3* offset);
	HRESULT NormalizeMesh(float scaleToX, float scaleToY, float scaleToZ, bool bCenter);
	
	LPDIRECT3DDEVICE9 p_Device;

	D3DXVECTOR3 centerPos;
	HRESULT result;
	
};
#endif