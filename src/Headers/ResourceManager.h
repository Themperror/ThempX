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
//#include "GUI.h"
//#include "ThempX.h"
//#include "SoundHandler.h"
//#include "Game.h"

#include <boost/thread.hpp>

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
	struct TextData
	{
		bool render;
		char* font;
		int fontsize;
		char* text;
		RECT textRect;
		D3DXCOLOR color;
		LPD3DXFONT gameFont; 
		bool hasLostDone;
		void LostDevice()
		{
			//std::cout << "LostDevice called" << std::endl;
			gameFont->OnLostDevice();
			hasLostDone = true;
		}
		void ResetDevice()
		{
			//std::cout << "ResetDevice called" << std::endl;
			gameFont->OnResetDevice();
			hasLostDone = false;
		}
		void ReleaseFont()
		{
			gameFont->Release();
			gameFont = NULL;
		}
		void DrawFont()
		{
			gameFont->DrawTextA(NULL,text,-1,&textRect,DT_LEFT | DT_TOP,color);
		}
	};

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
	inline int GetWindowHeight()
	{
		WINDOWINFO info;
		GetWindowInfo(wHandle,&info);
		return (int)(info.rcWindow.bottom - info.rcWindow.top);
	}
	inline int GetWindowWidth()
	{
		WINDOWINFO info;
		GetWindowInfo(wHandle,&info);
		return (int)(info.rcWindow.right - info.rcWindow.left);
	}
	inline LPD3DXMESH GetMeshFromVector(int i)
	{
		return models.at(i).mesh;
	}
	inline ResourceManager::Model* GetModelStructFromVector(int i)
	{
		return &models.at(i);
	}
	
	int CreateTextObject(char* font,char* text,int fontsize, int posX, int posY, int width, int height, D3DXCOLOR color);
	inline TextData* GetText(int i)
	{
		return &texts.at(i);
	}
	inline void ReleaseAllText()
	{
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i).ReleaseFont();
		}
	}
	inline void LostDeviceAllText()
	{
		std::cout << "Text device lost" << std::endl;
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i).LostDevice();
		}
	}
	inline void ResetDeviceAllText()
	{
		std::cout << "Text device reset" << std::endl;
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i).ResetDevice();
		}
	}
	inline void DrawAllText()
	{
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i).DrawFont();
		}
	}
	//inline void SetGUI(GUI* g)
	//{
	//	gui = g;
	//}
	//inline GUI* getGUI()
	//{
	//	return gui;
	//}

private:
	
	
	struct Quad
	{
		LPDIRECT3DTEXTURE9 texture;
		char* textureName;
	};
	std::vector<TextData> texts;
	std::vector<Model> models;
	std::vector<Quad> quads;
	bool LoadQuadTexture(char* path);
	bool CheckAvailableTexture(char* name);
	bool CheckAvailableModel(char* name);
	float screenWidth,screenHeight;

	//easy access classes and variables
	HWND wHandle;
	LPDIRECT3DDEVICE9 p_Device;
	//Game* game;
	//ThempX* engine;
	//SoundHandler* soundHandler;
	//InputHandler* inputHandler;

	
};
#endif