#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include <d3dx9.h>
#include <d3d9.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <D3DX9Mesh.h>
#include <string>


class ResourceManager
{
	
public:
	struct Model
	{
		LPD3DXMESH mesh;
		D3DXMATERIAL* d3dxMaterials;
		D3DMATERIAL9* meshMaterials;
		LPD3DXBUFFER materialBuffer;
		LPDIRECT3DTEXTURE9* meshTextures;
		DWORD numMaterials;
		char* meshName;
	};
	struct Quad
	{
		LPDIRECT3DTEXTURE9 texture;
		char* textureName;
	};
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager(LPDIRECT3DDEVICE9 d3d_Device, HWND handle);
	void ReleaseResources();
	bool LoadQuadTexture(char* path);
	bool CheckAvailableTexture(char* name);
	bool CheckAvailableModel(char* name);
	int GetMeshData(char* name);
	LPDIRECT3DTEXTURE9 GetTexture(char* name);
	std::vector<Model> models;
	std::vector<Quad> quads;
	HWND wHandle;
};
#endif