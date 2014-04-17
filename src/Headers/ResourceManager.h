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
		void DrawFont()
		{
			if(gameFont != NULL)
			{
				gameFont->DrawTextA(NULL,text,-1,&textRect,DT_LEFT | DT_TOP,color);
			}
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
	/*
	inline D3DXMATRIX* GetCameraView()
	{
		return &cam->GetView();
	}
	inline void SetCamera(Camera* camPtr)
	{
		cam = camPtr;
	}
	*/
	inline void GetDesktopResolution(int& x, int& y)
	{
		RECT desktop;
	   // Get a handle to the desktop window
	   const HWND hDesktop = GetDesktopWindow();
	   // Get the size of screen to the variable desktop
	   GetWindowRect(hDesktop, &desktop);
	   // The top left corner will have coordinates (0,0)
	   // and the bottom right corner will have coordinates
	   // (horizontal, vertical)
	   x = desktop.right;
	   y = desktop.bottom;
	}
	inline void SetScreenResolution(int x, int y)
	{
		screenWidth = x;
		screenHeight = y;
	}
	inline int GetScreenHeight()
	{
		return screenHeight;
	}
	inline int GetScreenWidth()
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
		return texts.at(i);
	}
	inline void ReleaseAllText()
	{
		for(unsigned int i = 0; i < fonts.size(); i++)
		{
			if(fonts.at(i) != NULL)
			{
				fonts.at(i)->Release();
				fonts.at(i) = NULL;
			}
		}
	}
	inline void SetTextRenderingFalse()
	{
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i)->render = false;
		}
	}
	inline void LostDeviceAllText()
	{
		std::cout << "Text device lost" << std::endl;
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i)->LostDevice();
		}
	}
	inline void ResetDeviceAllText()
	{
		std::cout << "Text device reset" << std::endl;
		for(unsigned int i =0;i<texts.size();i++)
		{
			texts.at(i)->ResetDevice();
		}
	}
	inline void DrawAllText()
	{
		for(unsigned int i =0;i<texts.size();i++)
		{
			if(texts.at(i)->render)
			{
				texts.at(i)->DrawFont();
			}
		}
	}
	inline void PushDevmode(DEVMODE dev)
	{
		devmodes.push_back(dev);
	}
	inline DEVMODE GetDevMode(int i)
	{
		return devmodes.at(i);
	}
	inline DEVMODE GetLastDevMode()
	{
		return devmodes.at(devmodes.size()-1);
	}
	inline int GetDevMode(int width, int height)
	{
		for(unsigned int i =0; i < devmodes.size();i++)
		{
			if(devmodes.at(i).dmPanningWidth == width && devmodes.at(i).dmPelsHeight == height)
			{
				return i;
			}
		}
		return devmodes.size()-1;
	}
	inline void SetCameraCircle(D3DXVECTOR3 circle)
	{
		cameraCircle = circle;
	}
	inline D3DXVECTOR3 GetCameraCircle()
	{
		return cameraCircle;
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
	std::vector<TextData*> texts;
	std::vector<Model> models;
	std::vector<Quad> quads;
	std::vector<DEVMODE> devmodes;
	std::vector<LPD3DXFONT> fonts;
	bool LoadQuadTexture(char* path);
	bool CheckAvailableTexture(char* name);
	bool CheckAvailableModel(char* name);
	int screenWidth,screenHeight;
	D3DXVECTOR3 cameraCircle;
	//easy access classes and variables
	HWND wHandle;
	LPDIRECT3DDEVICE9 p_Device;
	//Camera* cam;
	//Game* game;
	//ThempX* engine;
	//SoundHandler* soundHandler;
	//InputHandler* inputHandler;

	
};
#endif