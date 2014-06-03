#ifndef _GUI_H_
#define _GUI_H_

#include "ResourceManager.h"
#include <sstream>
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
		float AnimationSpeed;
		bool isFinished;
		bool loop;
		bool showEnd;
		void Nullify()
		{
			StartPosition = D3DXVECTOR2(0,0);
			EndPosition = D3DXVECTOR2(0,0);
			AnimationName = "";
			AnimationSpeed = 0;
			isFinished = false;
			showEnd = false;
			loop = false;
		}
	};
	struct GUITexture
	{
		GUITexture()
		{
			Nullify();
		}
		Rectangle rect;
		LPDIRECT3DTEXTURE9 texture;
		std::string textureName;
		bool hasAnimation, mirrored,render;
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
			mirrored = 0;
			render = true;
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
	void DrawAllText();
	void Update(float deltaTime);
	
	bool CreateGUIObject(Rectangle rect,char* textureName);
	bool CreateGUIObject(Rectangle rect,char* textureName, int xRows,int yRows);
	std::vector<GUITexture> guiObjs;
	bool PlayAnimation(GUITexture* obj,std::string name);
	void LoadAnimation(GUITexture* obj);
	void ReloadGUI();
	inline void SetTextRenderingTrue()
	{
		healthText->render = true;
		armourText->render = true;
	}
	//player variables
	int health;
	int armour;
	GUITexture* attackGUI;
	GUITexture* levelCompleteGUI;
	ResourceManager::TextData* healthText;
	ResourceManager::TextData* armourText;
	ResourceManager::TextData* gameOverText;
	inline GUITexture* GetGUIObj(std::string sname)
	{
		std::string name = LowCaseString(sname);
		for(unsigned int i =0; i < guiObjs.size(); i++)
		{
			if(strcmp(guiObjs.at(i).textureName.c_str(),name.c_str()) == 0)
			{
				return &guiObjs.at(i);
			}
		}
		return NULL;
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
	
	void LoadGUI();
	void SetUVValues(GUITexture* obj);
	void Animate(GUITexture* obj , float dTime,float animSpeed);
	D3DXMATRIX matProj;
	LPDIRECT3DDEVICE9 p_Device;
	ResourceManager* resources;
	LPDIRECT3DVERTEXBUFFER9 CreateQuadVBuffer(GUITexture* gui);
	LPDIRECT3DINDEXBUFFER9 CreateQuadIndices();
};

#endif