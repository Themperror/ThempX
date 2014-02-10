#include "../Headers/InputHandler.h"

InputHandler::InputHandler(HWND handleWindow)
{
	handle = handleWindow;
	m_pDIObject = NULL;
	m_pDIKeyboardDevice = NULL;       

	InitDirectInput();
	InitKeyboard();
	InitMouse();

} 

InputHandler::~InputHandler()
{
	if(m_pDIKeyboardDevice != NULL)
	{
		m_pDIKeyboardDevice->Unacquire(); //unacquire device
		m_pDIKeyboardDevice->Release();   //release keyboard stuff
		m_pDIKeyboardDevice = NULL;       //pointer to NULL
	}
	if(m_pDIMouseDevice != NULL)
	{
		m_pDIMouseDevice->Unacquire();
		m_pDIMouseDevice->Release();
		m_pDIMouseDevice = NULL;
	}
	if(m_pDIObject != NULL)
	{
		m_pDIObject->Release();
		m_pDIObject = NULL;
	}
}
bool InputHandler::InitDirectInput(void)
{
	if(FAILED(DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&m_pDIObject,NULL)))
	{
		MessageBox(handle,"DirectInput8Create() failed!","InitDirectInput()",MB_OK);
		return false;
	}
	return true;
}
bool InputHandler::InitKeyboard(void)
{
	if(FAILED(m_pDIObject->CreateDevice(GUID_SysKeyboard,&m_pDIKeyboardDevice,NULL)))
	{
		MessageBox(handle,"CreateDevice() failed!","InitKeyboard()",MB_OK);
		return false;
	}
	if(FAILED(m_pDIKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		MessageBox(handle,"SetDataFormat() failed!","InitKeyboard()",MB_OK);
		return false;
	}
	if(FAILED(m_pDIKeyboardDevice->SetCooperativeLevel(handle,DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox(handle,"SetCooperativeLevel() failed!","InitKeyboard()",	MB_OK);
		return false;
	}
	if(FAILED(m_pDIKeyboardDevice->Acquire()))
	{
		MessageBox(handle,"Acquire() failed!","InitKeyboard()",MB_OK);
		return false;
	}
	return true;
}
bool InputHandler::InitMouse(void)
{
	DIDEVCAPS MouseCapabilities;
	if(FAILED(m_pDIObject->CreateDevice(GUID_SysMouse,&m_pDIMouseDevice,NULL)))
	{
		MessageBox(handle,"CreateDevice() failed!","InitMouse()",MB_OK);
		return false;
	}
	if(FAILED(m_pDIMouseDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		MessageBox(handle,"SetDataFormat() failed!","InitMouse()",MB_OK);
		return false;
	}
	if(FAILED(m_pDIMouseDevice->SetCooperativeLevel(handle,DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox(handle,"SetCooperativeLevel() failed!","InitMouse()",MB_OK);
		return false;
	}
	if(FAILED(m_pDIMouseDevice->Acquire()))
	{
		MessageBox(handle,"Acquire() failed!","InitMouse()",MB_OK);
		return false;
	}
	MouseCapabilities.dwSize = sizeof(MouseCapabilities);
	m_pDIMouseDevice->GetCapabilities(&MouseCapabilities);

	if(!(MouseCapabilities.dwFlags & DIDC_ATTACHED))
	{
		MessageBox(handle,"Mouse is currently not attached!","InitMouse()",MB_OK);
		return false;
	}

	m_dwAxes = MouseCapabilities.dwAxes;
	m_dwButtons = MouseCapabilities.dwButtons;
	return true;
}
bool InputHandler::Update(void)
{
	if(FAILED(m_pDIKeyboardDevice->GetDeviceState(sizeof(keyBuffer),(LPVOID)&keyBuffer)))
	{
		MessageBox(handle,"GetDeviceState() failed!","Update()",MB_OK);
		return false;
	}
	if(DIERR_INPUTLOST == m_pDIMouseDevice->GetDeviceState(sizeof(m_MouseState), (LPVOID)&m_MouseState))
	{
		m_pDIMouseDevice->Acquire();
	}
	return true;
}
float InputHandler::GetMousePosX()
{
	return (float)m_MouseState.lX;
}
float InputHandler::GetMousePosY()
{
	return (float)m_MouseState.lY;
}
float InputHandler::GetMouseScroll()
{
	return (float)m_MouseState.lZ;
}
bool InputHandler::MouseButtonDown(int button)
{
	if(m_MouseState.rgbButtons[button] & 0x80)
	{
		return true;
	}
	return false;
}
bool InputHandler::KeyPressed(int key)
{
	if(keyBuffer[key] & 0x80)
	{
		return true;
	}
	return false;
}