#ifndef _GUI_H_
#define _GUI_H_

#include "ResourceManager.h"

class GUI
{
public:
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
		float xRows, yRows, currentXAnimValue,currentYAnimValue,sizeX,sizeY,endXAnimValue,endYAnimValue, animationSpeed;
		std::string currentlyPlayingAnimation;
		Animation* currentAnim;
		
		void Nullify()
		{
			rect.Nullify();
			texture = NULL;
			textureName = "";
			hasAnimation = 0;
			vBuffer = NULL;
			iBuffer = NULL;
			timeSinceChange = 0;
			xRows=0;yRows=0;currentXAnimValue=0;currentYAnimValue=0;sizeX=0;sizeY=0;endXAnimValue=0;endYAnimValue=0;animationSpeed=0;
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
	struct Vertex2D
	{
		float x;
		float y;
		float z;
		float rhw;
		D3DXVECTOR2 texC;
	};
	void LoadGUI();
	void Animate(GUITexture* obj , float dTime,float animSpeed);
	D3DXMATRIX matProj;
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	LPDIRECT3DVERTEXBUFFER9 CreateQuadVBuffer(GUITexture* gui);
	LPDIRECT3DINDEXBUFFER9 CreateQuadIndices();
};

#endif