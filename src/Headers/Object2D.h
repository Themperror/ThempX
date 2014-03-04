#ifndef _OBJECT2D_H_
#define _OBJECT2D_H_

#include <d3dx9.h>
#include <d3d9.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <D3DX9Mesh.h>
#include <string>
#include "ResourceManager.h"
#include <fstream>
#include <stdio.h>


struct Animation
{
	D3DXVECTOR2 StartPosition;
	D3DXVECTOR2 EndPosition;
	std::string AnimationName;
};
struct Quad
{
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	D3DXMATERIAL d3dxMaterial;
	D3DMATERIAL9 meshMaterial;
	LPD3DXBUFFER materialBuffer;
	LPDIRECT3DTEXTURE9 texture;
	char* textureName;
};

class Object2D
{
	
public:	 
	struct VertexPosNorTex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texC;
	};
  
	Object2D(ResourceManager* res, char* texturePath, D3DXMATRIX* camView);
	Object2D(ResourceManager* res, char* texturePath, D3DXMATRIX* camView, D3DXVECTOR2 LLPos, D3DXVECTOR2 URPos);
	Object2D(ResourceManager* res, char* texturePath,D3DXMATRIX* camViewMatrix,float tSizeX,float tSizeY,float xRowsAnim,float yRowsAnim);
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;	 
	D3DXMATRIX* cameraView;

	void Draw();
	 
	void Animate(float dTime);
	
	bool hasAnimation;
	void ReleaseResources();
	HWND handleWindow;
	void SetPosition(float x,float y,float z);
	void SetScale(float x,float y,float z);
	Quad quad;
	D3DXMATRIX worldMatrix;
	std::string objName;
	std::string tag;
	//linkedphysX obj
	bool PlayAnimation(std::string name);
	std::string currentlyPlayingAnimation;
	
	inline float GetXRows()
	{
		return xRows;
	}
	inline float GetYRows()
	{
		return yRows;
	}
	inline float GetXSize()
	{
		return sizeX;
	}
	inline float GetYSize()
	{
		return sizeY;
	}
private:
	void InitVars(); 
	void LoadAnimation();
	std::vector<Animation> animations; 
	float xRows, yRows, currentXAnimValue,currentYAnimValue,sizeX,sizeY,endXAnimValue,endYAnimValue;
	float timeSinceChange;
	LPDIRECT3DVERTEXBUFFER9 FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos);
	LPDIRECT3DVERTEXBUFFER9 FillVertices();
	LPDIRECT3DINDEXBUFFER9 FillIndices();

	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;

	HRESULT result;
};
#endif