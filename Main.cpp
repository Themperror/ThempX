#include "ThempX.h"

ThempX* engine;
HWND NewWindow(LPCTSTR windowName,int posX,int posY, int sizeX,int sizeY);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			//alles wordt in de constructor van ThempX uitgevoerd
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
HWND NewWindow(LPCTSTR windowName,int posX,int posY, int sizeX,int sizeY)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	wc.lpszClassName = "ThempX";
	wc.lpszMenuName = NULL;
	wc.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	RegisterClassEx(&wc);
 
	return CreateWindowEx(WS_EX_CONTROLPARENT, "ThempX", windowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, posX, posY, sizeX, sizeY, NULL, NULL, GetModuleHandle(NULL), NULL);
}
void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Begin of the function //init

	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	HWND windowHandle = NewWindow("ThempX Engine",horizontal/2-400,vertical/2-300,800,600);

	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	engine = new ThempX(windowHandle,hInstance);


	DestroyWindow(windowHandle);
    return 0;
}