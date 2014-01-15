#include "..\Headers\CollisionGeo.h"

CollisionGeo::CollisionGeo(D3DXVECTOR3 cubePosition, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos)
{
	currentType = StaticCube;
	position = cubePosition;
	LLFPos = D3DXVECTOR3(cubePosition.x+LowerLeftFrontPos.x,cubePosition.y+LowerLeftFrontPos.y,cubePosition.z+LowerLeftFrontPos.z) ;
	URBPos = D3DXVECTOR3(cubePosition.x+UpperRightBackPos.x,cubePosition.y+UpperRightBackPos.y,cubePosition.z+UpperRightBackPos.z) ;
}
CollisionGeo::CollisionGeo(D3DXVECTOR3 cubePosition,D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos)
{
	currentType = OBBCube;
	LLFPos = D3DXVECTOR3(cubePosition.x+LowerLeftFrontPos.x,cubePosition.y+LowerLeftFrontPos.y,cubePosition.z+LowerLeftFrontPos.z) ;
	URBPos = D3DXVECTOR3(cubePosition.x+UpperRightBackPos.x,cubePosition.y+UpperRightBackPos.y,cubePosition.z+UpperRightBackPos.z) ;
	SetPosition(cubePosition);
	SetRotation(cubeRotation);
}
CollisionGeo::CollisionGeo(D3DXVECTOR3 spherePosition, float sphereRadius)
{
	currentType = Sphere;
	position = spherePosition;
	radius = sphereRadius;
}
CollisionGeo::CollisionGeo(D3DXVECTOR3 lowPosition, float capsuleHeight, float sphereRadius, bool isCapsule)
{
	(isCapsule == true ? currentType = Sphere : currentType = Cylinder);
	position = lowPosition;
	radius = sphereRadius;
	height = capsuleHeight;	
} 

CollisionGeo::CollisionResult CollisionGeo::IsInCube(D3DXVECTOR3 pos)
{
	if(currentType == OBBCube || currentType == StaticCube)
	{
		if(pos.x < URBPos.x && pos.y < URBPos.y && pos.z < URBPos.z)
		{
			if(pos.x > LLFPos.x && pos.y > LLFPos.y && pos.z > LLFPos.z)
			{
				return Collision;
			}
			return NoCollision;
		}
		return NoCollision;
	}
	return NotAStaticOrOBBCube;
}

CollisionGeo::CollisionResult CollisionGeo::IsInSphere(D3DXVECTOR3 pos)
{
	if(currentType == Sphere)
	{
		if(Vector3Distance(&pos,&position) <= radius)
		{
			return Collision;
		}
		return NoCollision;
	}			 
	return NotASphere;
}
CollisionGeo::CollisionResult CollisionGeo::IsInCylinder(D3DXVECTOR3 pos)
{
	if(currentType == Cylinder)
	{
		D3DXVECTOR2 v1;
		v1.x = pos.x;
		v1.y = pos.z;
		D3DXVECTOR2 v2;
		v2.x = position.x;
		v2.y = position.z;
		if(Vector2Distance(&v1,&v2) <= radius)
		{
			if(pos.y <= position.y + height/2 && pos.y >= position.y - height/2)
			{
			   return Collision;
			}
			return NoCollision;
		}
		return NoCollision;
	}
	return NotACylinder;
}
CollisionGeo::CollisionResult CollisionGeo::DidSphereCollideWith(CollisionGeo* other)
{
	if(currentType == Sphere)
	{
		if(other->GetType() == Sphere)
		{
			if(Vector3Distance(&position,&other->GetPosition()) <= radius+other->GetRadius())
			{
				return Collision;
			}
			return NoCollision;
		}
		return Arg2NotSphere;
	}
	return Arg1NotSphere;
}
CollisionGeo::CollisionResult CollisionGeo::DidCubeCollideWith(CollisionGeo* other)
{
	if(currentType == OBBCube || currentType == StaticCube)
	{
		if(other->GetType() == OBBCube || other->GetType() == StaticCube)
		{
			D3DXVECTOR3 thisSize = GetCubeSize();
			D3DXVECTOR3 otherSize = other->GetCubeSize();

			if(position.x+thisSize.x >= other->GetPosition().x - otherSize.x&& position.x-thisSize.x <= other->GetPosition().x + otherSize.x&& position.z+thisSize.z >= other->GetPosition().z - otherSize.z && position.z-thisSize.z <= other->GetPosition().z + otherSize.z && position.y+thisSize.y >= other->GetPosition().y - otherSize.y && position.y-thisSize.y <= other->GetPosition().y + otherSize.y)
			{
			   return Collision;
			}
			return NoCollision;
		}
		return Arg2NotStaticOrOBBCube;
	}
	return Arg1NotStaticOrOBBCube;
}

CollisionGeo::CollisionResult CollisionGeo::DidOBBCollideWithOBB(CollisionGeo* other)
{
	//Need to cast a bounding volume over the current OBB before using SAT as performance tweak;

	/*switch(DidCubeCollideWith(other))
	{
	case NoCollision: return NoCollision;break;
	case Arg1NotStaticOrOBBCube: return Arg1NotStaticOrOBBCube;break;
	case Arg2NotStaticOrOBBCube: return Arg2NotStaticOrOBBCube;break;
	} */


	//Start SAT (Separating Axis Theorem)
	for(int i = 0; i < 3; i++)
	{
		float Min0,Max0;
		Project(GetOBB(),GetOBB()->normalAxis[i], Min0, Max0);

		float Min1,Max1;
		Project(other->GetOBB(),other->GetOBB()->normalAxis[i], Min1, Max1);

		if(Max0<Min1 || Max1<Min0)
		{
			return NoCollision;
		}
	}
  	return Collision;
}

void CollisionGeo::Project(OBB* obb,const D3DXVECTOR3& dir, float& min, float& max) const
{
	min = FLT_MAX;
	max = -FLT_MAX;
	for(int i=0;i<8;i++)
	{
		const float dp = D3DXVec3Dot(&D3DXVECTOR3(obb->transformedVertexPoints[i].x,obb->transformedVertexPoints[i].y,obb->transformedVertexPoints[i].z),&dir);
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


