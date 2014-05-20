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

	struct Animation
	{
		D3DXVECTOR2 StartPosition;
		D3DXVECTOR2 EndPosition;
		D3DXVECTOR2 ActionFrame;
		bool doAction;
		std::string AnimationName;
		float AnimationSpeed;
		bool isFinished;
		bool loop;
		void Nullify()
		{
			StartPosition = D3DXVECTOR2(0,0);
			EndPosition = D3DXVECTOR2(0,0);
			ActionFrame = D3DXVECTOR2(0,0);
			AnimationName = "";
			AnimationSpeed = 0;
			isFinished = false;
			loop = false;
			doAction = false;
		}
	};

	struct VertexPosNorTex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texC;
	};
  
	Object2D(ResourceManager* res, char* texturePath, D3DXMATRIX* camView);
	Object2D(ResourceManager* res, char* texturePath, D3DXMATRIX* camView, D3DXVECTOR2 LLPos, D3DXVECTOR2 URPos);
	Object2D(ResourceManager* res, char* texturePath, D3DXMATRIX* camViewMatrix,int xRowsAnim,int yRowsAnim);
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;	 
	D3DXMATRIX* cameraView;

	void Update(float deltaTime);
	void SetUVValues();
	bool PlayAnimation(std::string aName);
	bool IsAnimFinished(std::string anim);
	void CheckPlayingAnimation(std::string playAnimFinished);
	Animation GetAnimation(std::string aName);
	void Draw(); 
	
	bool hasAnimation;
	void ReleaseResources();
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
	inline Animation* GetCurrentAnim()
	{
		if(hasAnimation)
			return &animations.at(currentAnim);
		else
			return NULL;
	}
	Quad quad;
	D3DXMATRIX worldMatrix;
	std::string objName;
	std::string tag;
	std::string currentlyPlayingAnimation;
	
	inline int GetXRows()
	{
		return xRows;
	}
	inline int GetYRows()
	{
		return yRows;
	}
	inline std::string LowCaseString(std::string string)
	{
		std::string newString = string;
		for(DWORD i = 0; i < string.size(); i++)
		{
			newString[i] = (char)tolower(string[i]);
		}
		return newString;
	}
private:
	void InitVars(); 
	std::vector<Animation> animations; 
	int xRows, yRows, currentXAnimValue,currentYAnimValue,sizeX,sizeY,endXAnimValue,endYAnimValue,currentAnim;
	float timeSinceChange,animationSpeed;
	LPDIRECT3DVERTEXBUFFER9 FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos);
	LPDIRECT3DVERTEXBUFFER9 FillVertices();
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	
	void Animate();
	void LoadAnimation();

	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;

	HRESULT result;
};
#endif