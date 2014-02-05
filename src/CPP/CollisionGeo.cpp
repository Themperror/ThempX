#include "..\Headers\CollisionGeo.h"

CollisionGeo::CollisionGeo(D3DXVECTOR3* cubePosition, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos)
{
	currentType = StaticCube;
	position = cubePosition;
	height = UpperRightBackPos.y - LowerLeftFrontPos.y;
	LLFPos = D3DXVECTOR3(cubePosition->x+LowerLeftFrontPos.x,cubePosition->y+LowerLeftFrontPos.y,cubePosition->z+LowerLeftFrontPos.z) ;
	URBPos = D3DXVECTOR3(cubePosition->x+UpperRightBackPos.x,cubePosition->y+UpperRightBackPos.y,cubePosition->z+UpperRightBackPos.z) ;
}
CollisionGeo::CollisionGeo(D3DXVECTOR3* cubePosition,D3DXVECTOR3* cubeRotation, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos)
{
	currentType = OBBCube;
	height = UpperRightBackPos.y - LowerLeftFrontPos.y;
	LLFPos = D3DXVECTOR3(cubePosition->x+LowerLeftFrontPos.x,cubePosition->y+LowerLeftFrontPos.y,cubePosition->z+LowerLeftFrontPos.z) ;
	URBPos = D3DXVECTOR3(cubePosition->x+UpperRightBackPos.x,cubePosition->y+UpperRightBackPos.y,cubePosition->z+UpperRightBackPos.z) ;
	SetAABB(cubePosition,cubeRotation,scaling);
}
CollisionGeo::CollisionGeo(D3DXVECTOR3* spherePosition, float sphereRadius)
{
	currentType = Sphere;
	position = spherePosition;
	radius = sphereRadius;
}
CollisionGeo::CollisionGeo(D3DXVECTOR3* lowPosition, float capsuleHeight, float sphereRadius, bool isCapsule)
{
	(isCapsule == true ? currentType = Sphere : currentType = Cylinder);
	position = lowPosition;
	radius = sphereRadius;
	height = capsuleHeight;	
} 

CollisionGeo::CollisionInfo CollisionGeo::IsInCube(D3DXVECTOR3* pos)
{
	CollisionInfo info;
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.target = NULL;
	if(currentType == OBBCube || currentType == StaticCube)
	{
		if(pos->x < URBPos.x && pos->y < URBPos.y && pos->z < URBPos.z)
		{
			if(pos->x > LLFPos.x && pos->y > LLFPos.y && pos->z > LLFPos.z)
			{
				info.result = Collision;
				return info;
			}
			info.result = NoCollision;
			return info;
		}
		info.result = NoCollision;
		return info;
	}
	info.result = NotAStaticOrOBBCube;
	return info;
}

CollisionGeo::CollisionInfo CollisionGeo::IsInSphere(D3DXVECTOR3* pos)
{
	CollisionInfo info;
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.target = NULL;
	if(currentType == Sphere)
	{
		if(Vector3Distance(pos,position) <= radius)
		{
			info.result = Collision;
			return info;
		}
		info.result = NoCollision;
		return info;
	}			 
	info.result = NotASphere;
	return info;
}
CollisionGeo::CollisionInfo CollisionGeo::IsInCylinder(D3DXVECTOR3* pos)
{
	CollisionInfo info;
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.target = NULL;
	if(currentType == Cylinder)
	{
		D3DXVECTOR2 v1;
		v1.x = pos->x;
		v1.y = pos->z;
		D3DXVECTOR2 v2;
		v2.x = position->x;
		v2.y = position->z;
		if(Vector2Distance(&v1,&v2) <= radius)
		{
			if(pos->y <= position->y + height/2 && pos->y >= position->y - height/2)
			{
				info.result = Collision;
				return info;
			}
			info.result = NoCollision;
			return info;
		}
		info.result = NoCollision;
		return info;
	}
	info.result = NotACylinder;
	return info;
}
CollisionGeo::CollisionInfo CollisionGeo::DidSphereCollideWith(CollisionGeo* other)
{
	CollisionInfo info;
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.target = other;
	if(currentType == Sphere)
	{
		if(other->GetType() == Sphere)
		{
			if(Vector3Distance(position,other->GetPosition()) <= radius+other->GetRadius())
			{
				info.result = Collision;
				info.hitPos = position +  (position - other->GetPosition());
				return info;
			}
			info.result = NoCollision;
			return info;
		}
		info.result = Arg2NotSphere;
		return info;
	}
	info.result = Arg1NotSphere;
	return info;
}
CollisionGeo::CollisionInfo CollisionGeo::DidCubeCollideWith(CollisionGeo* other)
{
	CollisionInfo info;
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.target = other;
	if(currentType == OBBCube || currentType == StaticCube)
	{
		if(other->GetType() == OBBCube || other->GetType() == StaticCube)
		{
			D3DXVECTOR3 thisSize = GetCubeSize();
			D3DXVECTOR3 otherSize = other->GetCubeSize();

			if(position->x+thisSize.x >= other->GetPosition()->x - otherSize.x&& position->x-thisSize.x <= other->GetPosition()->x + otherSize.x&& position->z+thisSize.z >= other->GetPosition()->z - otherSize.z && position->z-thisSize.z <= other->GetPosition()->z + otherSize.z && position->y+thisSize.y >= other->GetPosition()->y - otherSize.y && position->y-thisSize.y <= other->GetPosition()->y + otherSize.y)
			{
			   info.result = Collision;
			   info.hitPos = &D3DXVECTOR3((position->x+LLFPos.x + other->GetLowerLeftFrontPos().x+other->GetPosition()->x + position->x+LLFPos.x + other->GetUpperRightBackPos().x+other->GetPosition()->x) /2 ,(position->y+LLFPos.y + other->GetLowerLeftFrontPos().y+other->GetPosition()->y + position->y+LLFPos.y + other->GetUpperRightBackPos().y+other->GetPosition()->y) / 2,( position->z+LLFPos.z + other->GetLowerLeftFrontPos().z+other->GetPosition()->z + position->z+URBPos.z + other->GetUpperRightBackPos().z+other->GetPosition()->z ) / 2);
			}
			info.result = NoCollision;
			return info;
		}
		info.result = Arg2NotStaticOrOBBCube;
		return info;
	}
	info.result = Arg1NotStaticOrOBBCube;
	return info;
}

CollisionGeo::CollisionInfo CollisionGeo::DidAABBCollideWithAABB(CollisionGeo* other)
{
	//Need to cast a bounding volume over the current OBB before using SAT as performance tweak;

	/*switch(DidCubeCollideWith(other))
	{
	case NoCollision: return NoCollision;break;
	case Arg1NotStaticOrOBBCube: return Arg1NotStaticOrOBBCube;break;
	case Arg2NotStaticOrOBBCube: return Arg2NotStaticOrOBBCube;break;
	} */

	CollisionInfo info;
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.target = other;

	//AABB
	for(int i = 0; i < 3; i++)
	{
		float Min0,Max0;
		Project(GetAABB(),GetAABB()->normalAxis[i], Min0, Max0);

		float Min1,Max1;
		Project(other->GetAABB(),other->GetAABB()->normalAxis[i], Min1, Max1);

		if(i == 0)
		{
			info.hitPos->x = Min0;
		}
		else if(i == 1)
		{
			info.hitPos->y = Min0;
		}
		else if(i == 2)
		{
			info.hitPos->z = Min0;
		}

		if(Max0<Min1 || Max1<Min0)
		{
			info.result = Collision;
			return info;
		}
	}
	info.hitPos = &D3DXVECTOR3(0,0,0);
	info.result = NoCollision;
	return info;
}

void CollisionGeo::Project(AABB* aabb,const D3DXVECTOR3& dir, float& min, float& max) const
{
	min = FLT_MAX;
	max = -FLT_MAX;
	for(int i=0;i<8;i++)
	{
		const float dp = D3DXVec3Dot(&D3DXVECTOR3(aabb->transformedVertexPoints[i].x,aabb->transformedVertexPoints[i].y,aabb->transformedVertexPoints[i].z),&dir);
		if(dp < min)	min = dp;
		if(dp > max)	max = dp;
	}
	if(min>max)
	{
		float tmp = min;
		min = max;
		max = tmp;
	}
}


