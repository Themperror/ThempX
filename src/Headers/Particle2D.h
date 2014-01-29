#ifndef _PARTICLE2D_H_
#define _PARTICLE2D_H_

#include "Object2D.h"


struct PosNorTex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};

class Particle2D
{
	
public:
	//get/set voor alles dat nodig is maken
	Particle2D(LPDIRECT3DDEVICE9 device, D3DXMATRIXA16* camView, LPDIRECT3DVERTEXBUFFER9 vertexBuffer,LPDIRECT3DINDEXBUFFER9 indexBuffer);
	void Draw(D3DXMATRIX m_ViewScale,D3DXMATRIX m_ViewWorld, D3DXMATRIX worldMatrix,D3DXMATRIX m_Scale,D3DXMATRIX m_Translation);
	inline D3DXVECTOR3 GetPosition()
	{
		return position;
	}
	inline D3DXVECTOR3 GetScale()
	{
		return scaling;
	}
	inline void AddPosition(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	}
	inline void SetPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}
	inline void SetPosition(D3DXVECTOR3* pos)
	{
		position.x = pos->x;
		position.y = pos->y;
		position.z = pos->z;
	}
	inline void SetScale(float xyz)
	{
		scaling.x = xyz;
		scaling.y = xyz;
		scaling.z = xyz;
	}
	inline void SetScale(float x, float y, float z)
	{
		scaling.x = x;
		scaling.y = y;
		scaling.z = z;
	}
	inline void SetViewMatrix(D3DXMATRIX* matrix)
	{
		viewMatrix = matrix;
	}
	inline D3DXMATRIX* GetViewMatrix()
	{
		return viewMatrix;
	}
	inline void SetLife(double x)
	{
		currentLife = x;
	}
	inline void SetMaxLife(double x)
	{
		maxLife = x;
	}
	inline void AddLife(double x)
	{
		currentLife += x;
	}
	inline double GetLife()
	{
		return currentLife;
	}
	inline double GetMaxLife()
	{
		return maxLife;
	}
	inline void SetIndexBuffer(LPDIRECT3DINDEXBUFFER9 buffer)
	{
		iBuffer = buffer;
	}
	inline void SetVertexBuffer(LPDIRECT3DVERTEXBUFFER9 buffer)
	{
		vBuffer = buffer;
	}
	inline void SetTexture(LPDIRECT3DTEXTURE9 text)
	{
		texture  = text;
	}
	inline void SetMaterial(D3DMATERIAL9* material)
	{
		mat = material;
	}
private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;
	D3DXMATRIX* viewMatrix;
	double currentLife;
	double maxLife;
	LPDIRECT3DTEXTURE9 texture;
	D3DMATERIAL9* mat;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	LPDIRECT3DDEVICE9 p_Device;
	D3DXMATRIX* cameraView;

};
#endif