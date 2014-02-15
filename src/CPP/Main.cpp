#include "../Headers/ThempX.h"

ThempX* engine;
		//Everything is called and initiated in the constructor of ThempX

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//start of the program here.
	
	//Sleep(1000);
	engine = new ThempX(hInstance,lpCmdLine);

    return 0;
}