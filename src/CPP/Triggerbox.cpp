#include "Triggerbox.h"

TriggerBox::TriggerBox(D3DXVECTOR3 pos,D3DXVECTOR3 halfWidth)
{
	position = pos;
	LLFPos = -halfWidth;
	URBPos = halfWidth;
}
bool TriggerBox::CheckInBox(D3DXVECTOR3* pos)
{
	if(pos->x > LLFPos.x+position.x && pos->x < URBPos.x+position.x)
	{
		if(pos->y > LLFPos.y+position.y && pos->y < URBPos.y+position.y)
		{
			if(pos->z > LLFPos.z+position.z && pos->z < URBPos.z+position.z)
			{
				return true;
			}
			return false;
		}
		return false;
	}
	return false;
}