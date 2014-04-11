#ifndef _CAMERA_H_
#define _CAMERA_H_

#define THEMPX_CAM_PERSPECTIVE 0x000000
#define THEMPX_CAM_ORTOGRAPHIC 0x000001

#include "ResourceManager.h"

class Camera
{
public:
	Camera(ResourceManager* res,DWORD cameraModeFlag);

	float angleX;
	float angleY;
	float angleZ; //unused/unimplemented , value is mostly used in shooters for leaning sideways
	float sensitivity;
	
	float FoV;
	float AspectRatio;
	void Update(float deltaTime, float mouseInputX,float mouseInputY);
	
	//inline 
	inline D3DXVECTOR3 ReturnDirection(float anglesX,float anglesY)
	{    
		return D3DXVECTOR3((position.x + 1 * std::cos(anglesX * 3.141592f / 180)) - position.x,(position.y+1 * std::tan(anglesY * 3.141592f / 200))-position.y,(position.z+1 * std::sin(anglesX * 3.141592f / 180))-position.z);
	}
	inline D3DXVECTOR3 ReturnCameraCircle()
	{
		float x = 1 * std::cos(angleX * 3.141592f / 180);
		float z = 1 * std::sin(angleX * 3.141592f / 180);
		float y = 1 * std::tan(angleY * 3.141592f / 200);
		return D3DXVECTOR3(x,y,z);
	}
	inline D3DXVECTOR3 GetPosition()
	{
		return position;
	}
	inline D3DXMATRIX GetView()
	{
		return m_View;
	}
	inline D3DXMATRIX GetProjection()
	{
		return m_Projection;
	}
	inline D3DXVECTOR3 GetLookDir()
	{
		return lookDir;
	}
	inline void SetLookDir(D3DXVECTOR3 value)
	{
		lookDir = value;
	}
	inline D3DXVECTOR3 GetLookAt()
	{
		return lookAt;
	}
	inline void SetLookAt(D3DXVECTOR3 value)
	{
		lookAt = value;
	}
	inline void SetPosition(D3DXVECTOR3 pos)
	{
		position = pos;
	}
	inline void SetPosition(float x, float y, float z)
	{
		position = D3DXVECTOR3(x,y,z);
	}
	inline void AddPosition(float x,float y, float z)
	{
		position += D3DXVECTOR3(x,y,z);
	}
	inline void AddPosition(D3DXVECTOR3 value)
	{
		position += value;
	}
	inline void RenderCamera(DWORD mode)
	{
		SetUpCamera(mode);
	}
private:
	D3DXVECTOR3 lookAt;
	D3DXMATRIX m_View;
	D3DXMATRIX m_Projection;
	D3DXVECTOR3 lookDir;	
	D3DXVECTOR3 position;
	ResourceManager* resources;
	DWORD camMode;
	void CalculateCameraBounds(float deltaTime, float mouseInputX,float mouseInputY);
	void SetCameraLook(float anglesX,float anglesY);
	void SetUpCamera(DWORD mode);
};
#endif