#ifndef _GUI_H_
#define _GUI_H_

#include "ResourceManager.h"

class GUI
{
public:
	struct Vertex2D
	{
		float x;
		float y;
		float z;
		float rhw;
		D3DXVECTOR2 texC;
	};
	struct Rectangle
	{
		float x,y,w,h;
		void Nullify()
		{
			x=0;y=0;w=0;h=0;
		}
	};
	struct Animation
	{
		D3DXVECTOR2 StartPosition;
		D3DXVECTOR2 EndPosition;
		std::string AnimationName;
		void Nullify()
		{
			StartPosition = D3DXVECTOR2(0,0);
			EndPosition = D3DXVECTOR2(0,0);
			AnimationName = "";
		}
	};
	struct GUITexture
	{
		Rectangle rect;
		LPDIRECT3DTEXTURE9 texture;
		std::string textureName;
		bool hasAnimation;
		LPDIRECT3DVERTEXBUFFER9 vBuffer;
		LPDIRECT3DINDEXBUFFER9 iBuffer;
		float timeSinceChange;
		std::vector<Animation> animations; 
		float currentXAnimValue,currentYAnimValue,sizeX,sizeY, animationSpeed;
		int xRows, yRows;
		std::string currentlyPlayingAnimation;
		int currentAnim;
		
		void Nullify()
		{
			rect.Nullify();
			texture = NULL;
			textureName = "";
			hasAnimation = 0;
			vBuffer = NULL;
			iBuffer = NULL;
			timeSinceChange = 0;
			currentlyPlayingAnimation = "";
			xRows=0;yRows=0;currentXAnimValue=0;currentYAnimValue=0;sizeX=0;sizeY=0;animationSpeed=0;currentAnim = 0;
		}
	};

	GUI(LPDIRECT3DDEVICE9 d3dDev, ResourceManager* res);
	void Release();
	void Render();
	void Update(float deltaTime);
	bool CreateGUIObject(Rectangle rect,char* textureName);
	bool CreateGUIObject(Rectangle rect,char* textureName, int xRows,int yRows);
	std::vector<GUITexture> guiObjs;
	bool PlayAnimation(GUITexture* obj,std::string name);
	void LoadAnimation(GUITexture* obj);
private:
	
	void LoadGUI();
	void Animate(GUITexture* obj , float dTime,float animSpeed);
	D3DXMATRIX matProj;
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	LPDIRECT3DVERTEXBUFFER9 CreateQuadVBuffer(GUITexture* gui);
	LPDIRECT3DINDEXBUFFER9 CreateQuadIndices();
};

#endif