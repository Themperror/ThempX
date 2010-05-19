#ifndef _PARTICLE2D_H_
#define _PARTICLE2D_H_

class Particle2D
{
public:
	//get/set voor alles dat nodig is maken
	Particle2D(LPDIRECT3DDEVICE9 device, D3DXMATRIXA16* camView, LPDIRECT3DVERTEXBUFFER9 vertexBuffer,LPDIRECT3DINDEXBUFFER9 indexBuffer);
	void Draw();
	inline D3DXVECTOR3 GetPosition()
	{
		return position;
	}
	inline D3DXVECTOR3 GetScale()
	{
		return scaling;
	}
	inline void SetPosition(float x,float y,float z)
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
	inline void SetViewMatrix(D3DXMATRIXA16* viewmatrix)
	{
		viewMatrix = viewmatrix;
	}
	inline D3DXMATRIXA16* GetViewMatrix()
	{
		return viewMatrix;
	}
	inline void SetLife(float x)
	{
		currentLife = x;
	}
	inline void AddLife(float x)
	{
		currentLife += x;
	}
	inline float GetLife()
	{
		return currentLife;
	}
	inline void SetIndexBuffer(LPDIRECT3DINDEXBUFFER9 buffer)
	{
		iBuffer = buffer;
	}
	inline void SetVertexBuffer(LPDIRECT3DVERTEXBUFFER9 buffer)
	{
		vBuffer = buffer;
	}
private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 scaling;
	D3DXMATRIX* viewMatrix;
	float currentLife;
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	LPDIRECT3DDEVICE9 p_Device;
	D3DXMATRIXA16* cameraView;

};
#endif