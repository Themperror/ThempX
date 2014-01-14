#ifndef _DEBUGCUBE_H_
#define _DEBUGCUBE_H_

#include "ResourceManager.h"

struct CUSTOMVERTEX {FLOAT X, Y, Z; DWORD COLOR;};
struct VertexPNT{FLOAT X, Y, Z; D3DVECTOR NORMAL;};

class DebugCube
{
public:
	DebugCube(LPDIRECT3DDEVICE9 d3d_Device,D3DXVECTOR3 cubePosition,	D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LLFPosition, D3DXVECTOR3 URBPosition, ResourceManager* resources);
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 LLFPos;
	D3DXVECTOR3 URBPos;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	D3DXMATERIAL d3dxMaterial;
	D3DMATERIAL9 meshMaterial;
	LPD3DXBUFFER materialBuffer;
	LPDIRECT3DTEXTURE9 texture;


	LPDIRECT3DDEVICE9 p_Device;
	void Release();
	void Draw();
	LPDIRECT3DINDEXBUFFER9 FillIndices();
	LPDIRECT3DVERTEXBUFFER9 FillVertices();
	HWND handleWindow;
};



#endif