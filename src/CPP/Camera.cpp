#include "../Headers/Camera.h"

Camera::Camera(ResourceManager* res, DWORD cameraModeFlag)
{
	resources = res;
	camMode = cameraModeFlag;
	angleX = 0;
	angleY = 0;
	angleZ = 0;
	sensitivity = 0.12f;
	SetUpCamera(cameraModeFlag);
	FoV = 1;
	AspectRatio = 800/600;
}

void Camera::Update(float deltaTime, float mouseInputX,float mouseInputY)
{
	CalculateCameraBounds(deltaTime,mouseInputX,mouseInputY);
}
void Camera::CalculateCameraBounds(float deltaTime, float mouseInputX,float mouseInputY)
{
	if(angleX < -360 || angleX > 720)
	{
		angleX = 0;
	}
	float xMod = mouseInputX*sensitivity;
	if(angleX-xMod < 0) 
	{
		float rest = xMod - angleX;
		angleX = 360-rest;
	}
	else if(angleX-xMod > 360)
	{
		float rest = abs(360 - angleX+xMod);
		angleX = rest;
	}
	else
	{
		angleX -= xMod;
	}
	angleY-= mouseInputY*sensitivity;
	(angleY < -75 ? angleY = -75 : (angleY> 75 ? angleY = 75 : 0));

	SetCameraLook(angleX,angleY);
}

void Camera::SetUpCamera(DWORD mode)
{
	if(mode == 0x000000)
	{
		D3DXVECTOR3 m_UpVector = D3DXVECTOR3(0, 1, 0);
		D3DXMatrixLookAtLH(&m_View, &position, &lookAt, &m_UpVector);
		resources->GetDevice()->SetTransform(D3DTS_VIEW, &m_View);
		D3DXMatrixPerspectiveFovLH(&m_Projection, FoV, AspectRatio, 1, 2000);
		resources->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_Projection);
	}
	else if(mode == 0x000001)
	{
		D3DXVECTOR3 m_UpVector = D3DXVECTOR3(0, 1, 0);
		D3DXMatrixLookAtLH(&m_View, &position, &lookAt, &m_UpVector);
		resources->GetDevice()->SetTransform(D3DTS_VIEW, &m_View);
		D3DXMatrixOrthoLH(&m_Projection,resources->GetScreenWidth(),resources->GetScreenHeight(),0,1);
		resources->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_Projection);
	}
	else
	{
		std::cout << "Incorrect camera mode flag, No camera will be viewed" << std::endl;
	}
}

void Camera::SetCameraLook(float anglesX,float anglesY)
{
	float x = 1 * std::cos(anglesX * 3.141592f / 180);
	float z = 1 * std::sin(anglesX * 3.141592f / 180);
	float y = 1 * std::tan(anglesY * 3.141592f / 200);

	lookAt.x = position.x+x*1;
	lookAt.y = position.y+y*1;
	lookAt.z = position.z+z*1;

	lookDir.x = position.x+x - position.x;
	lookDir.y = position.y+y - position.y;
	lookDir.z = position.z+z - position.z;
	D3DXVec3Normalize(&lookDir,&lookDir);
}