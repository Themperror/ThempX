#ifndef _INPUTHANDLER_H_
#define _INPUTHANDLER_H_
#define DIRECTINPUT_VERSION 0x0800
#define DIMOUSE_LEFTBUTTON   0
#define DIMOUSE_RIGHTBUTTON  1
#define DIMOUSE_MIDDLEBUTTON 2

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputHandler
{
public:
    InputHandler(HWND handleWindow);          //constructor
    ~InputHandler();         //destructor

	bool Update();
    bool KeyPressed(int x);  
	float GetMousePosX();
	float GetMousePosY();
	float GetMouseScroll();
    bool  MouseButtonDown(int button);
private:
	
    LPDIRECTINPUT8 m_pDIObject;
	bool InitDirectInput(); 
	bool InitKeyboard();
	bool InitMouse();
	LPDIRECTINPUTDEVICE8 m_pDIKeyboardDevice; 
	LPDIRECTINPUTDEVICE8 m_pDIMouseDevice;
	HWND handle;
    char keyBuffer[256];
	DIMOUSESTATE2 m_MouseState;
	bool m_bInverted;
    float m_Sensitivity, m_CursorX, m_CursorY;
    DWORD m_dwAxes, m_dwButtons;
};
#endif