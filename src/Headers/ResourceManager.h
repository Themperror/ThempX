#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#define ToRadian 0.0174532925f

#include <d3dx9.h>
#include <d3d9.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <D3DX9Mesh.h>
#include <string>
#include "CollisionGeo.h"
#include <boost/thread.hpp>

class ResourceManager
{
	
public:
	
	ResourceManager(LPDIRECT3DDEVICE9 d3d_Device, HWND handle);
	void ReleaseResources();
	int GetMeshData(char* name);
	LPDIRECT3DTEXTURE9 GetTexture(char* name);
	inline LPDIRECT3DDEVICE9 GetDevice()
	{
		return p_Device;
	}
	inline HWND GetWindowHandle()
	{
		return wHandle;
	}

	inline void SetScreenResolution(float x, float y)
	{
		screenWidth = x;
		screenHeight = y;
	}
	inline float GetScreenHeight()
	{
		return screenHeight;
	}
	inline float GetScreenWidth()
	{
		return screenWidth;
	}

private:

	
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
	std::vector<Model> models;
	std::vector<Quad> quads;
	bool LoadQuadTexture(char* path);
	bool CheckAvailableTexture(char* name);
	bool CheckAvailableModel(char* name);
	float screenWidth,screenHeight;
	HWND wHandle;
	LPDIRECT3DDEVICE9 p_Device;
};
#endif