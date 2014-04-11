#ifndef _TRIGGERBOX_H_
#define _TRIGGERBOX_H_

#include "../Headers/ResourceManager.h"

class TriggerBox
{
public:
	TriggerBox(D3DXVECTOR3 pos,D3DXVECTOR3 halfWidth);
	bool CheckInBox(D3DXVECTOR3* pos);
	inline D3DXVECTOR3 GetPosition()
	{
		return position;
	}
	inline D3DXVECTOR3 GetHalfSize()
	{
		return URBPos;
	}
private:
	D3DXVECTOR3 LLFPos,URBPos,position;
};

#endif