#ifndef _KEYCODES_H
#define _KEYCODES_H
#include <Windows.h>

#define VK_0 0x0030
#define VK_1 0x0031
#define VK_2 0x0032
#define VK_3 0x0033
#define VK_4 0x0034
#define VK_5 0x0035
#define VK_6 0x0036
#define VK_7 0x0037
#define VK_8 0x0038
#define VK_9 0x0039

#define VK_A 0x0041
#define VK_B 0x0042
#define VK_C 0x0043
#define VK_D 0x0044
#define VK_E 0x0045
#define VK_F 0x0046
#define VK_G 0x0047
#define VK_H 0x0048
#define VK_I 0x0049
#define VK_J 0x004A
#define VK_K 0x004B
#define VK_L 0x004C
#define VK_M 0x004D
#define VK_N 0x004E
#define VK_O 0x004F
#define VK_P 0x0050
#define VK_Q 0x0051
#define VK_R 0x0052
#define VK_S 0x0053
#define VK_T 0x0054
#define VK_U 0x0055
#define VK_V 0x0056
#define VK_W 0x0057
#define VK_X 0x0058
#define VK_Y 0x0059
#define VK_Z 0x005A
//zijn gewoon hex codes van de keys? want dan vind ik ze zelf wel, ja maar nu heb je alles. Lol. Default Key shit is al defined behalve alle cijfers en letters oh handig dan

bool Input[255];

//Hij ziet dat het een typedef is dus ja
void checkInput(WPARAM _keyCode, bool _state){ //hij kan WPARAM nie vinden (Heb je geen colour coding of zo)? kan hem gewoon vinden hoor kijk bij output ik zie het, maar ik denk dat dit ergens anders ligt dan hier
	if(_state){
		Input[_keyCode] = true;
	}
	else{
		Input[_keyCode] = false;
	}
}

#endif