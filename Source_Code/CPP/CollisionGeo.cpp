#include "Source_Code/Headers/CollisionGeo.h"

CollisionGeo::CollisionGeo(D3DXVECTOR3 cubePosition, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos)
{
	currentType = StaticCube;
	position = cubePosition;
	LLFPos = D3DXVECTOR3(cubePosition.x+LowerLeftFrontPos.x,cubePosition.y+LowerLeftFrontPos.y,cubePosition.z+LowerLeftFrontPos.z) ;
	URBPos = D3DXVECTOR3(cubePosition.x+UpperRightBackPos.x,cubePosition.y+UpperRightBackPos.y,cubePosition.z+UpperRightBackPos.z) ;
 //OBB values aangeven
}
CollisionGeo::CollisionGeo(D3DXVECTOR3 cubePosition,D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos)
{
	currentType = OBBCube;	
	LLFPos = D3DXVECTOR3(cubePosition.x+LowerLeftFrontPos.x,cubePosition.y+LowerLeftFrontPos.y,cubePosition.z+LowerLeftFrontPos.z) ;
	URBPos = D3DXVECTOR3(cubePosition.x+UpperRightBackPos.x,cubePosition.y+UpperRightBackPos.y,cubePosition.z+UpperRightBackPos.z) ;
	SetPosition(cubePosition);
	SetRotation(cubeRotation);
 //OBB values aangeven
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

			/*if(position.x+radius > other->GetPosition().x - other->GetRadius()&& position.x-radius < other->GetPosition().x + other->GetRadius()&& position.z+radius > other->GetPosition().z - other->GetRadius() && position.z-radius < other->GetPosition().z + other->GetRadius() && position.y+radius > other->GetPosition().y - other->GetRadius() && position.y-radius < other->GetPosition().y + other->GetRadius())
			{
				return Collision;
			}	 */
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
	switch(DidCubeCollideWith(other))
	{
	case NoCollision: return NoCollision;break;
	case Arg1NotStaticOrOBBCube: return Arg1NotStaticOrOBBCube;break;
	case Arg2NotStaticOrOBBCube: return Arg2NotStaticOrOBBCube;break;
	}

	OBB* a = GetOBB(); 
	OBB* b = other->GetOBB();


	float ra, rb;
	float R[9] , AbsR[9];
	// Compute rotation matrix expressing b in a’s coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i*j] = D3DXVec3Dot(&a->u[i], &b->u[j]);
		}
	}

	// Compute translation vector t
	D3DXVECTOR3 t = b->c - a->c;
	// Bring translation into a’s coordinate frame
	t = D3DXVECTOR3(D3DXVec3Dot(&t, &a->u[0]), D3DXVec3Dot(&t, &a->u[2]), D3DXVec3Dot(&t, &a->u[2]));
	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AbsR[i*j] = std::abs(R[i*j]) + 0.001f; //EPSILON HIER
		}
	}
	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) 
	{
		ra = a->e[i];
		rb = b->e[0] * AbsR[i] + b->e[1] * AbsR[i+3] + b->e[2] * AbsR[i+6];
		if (std::abs(t[i]) > ra + rb) return NoCollision;
	}
	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) 
	{
		ra = a->e[0] * AbsR[3*i] + a->e[1] * AbsR[3*i+1] + a->e[2] * AbsR[3*i+2];
		rb = b->e[i];
		if (std::abs(t[0] * R[3*i] + t[1] * R[3*i+1] + t[2] * R[3*i+2]) > ra + rb) return NoCollision;
	}
	// Test axis L = A0 x B0
	ra = a->e[1] * AbsR[2] + a->e[2] * AbsR[1];
	rb = b->e[1] * AbsR[6] + b->e[2] * AbsR[3];
	if (std::abs(t[2] * R[1] - t[1] * R[2]) > ra + rb) return NoCollision;
	// Test axis L = A0 x B1
	ra = a->e[1] * AbsR[5] + a->e[2] * AbsR[4];
	rb = b->e[0] * AbsR[6] + b->e[2] * AbsR[0];
	if (std::abs(t[2] * R[4] - t[1] * R[5]) > ra + rb) return NoCollision;
	// Test axis L = A0 x B2
	ra = a->e[1] * AbsR[9] + a->e[2] * AbsR[7];
	rb = b->e[0] * AbsR[3] + b->e[1] * AbsR[0];
	if (std::abs(t[2] * R[7] - t[1] * R[9]) > ra + rb) return NoCollision;
	// Test axis L = A1 x B0
	ra = a->e[0] * AbsR[2] + a->e[2] * AbsR[0];
	rb = b->e[1] * AbsR[7] + b->e[2] * AbsR[4];
	if (std::abs(t[0] * R[2] - t[2] * R[0]) > ra + rb) return Collision;
	// Test axis L = A1 x B1
	ra = a->e[0] * AbsR[5] + a->e[2] * AbsR[3];
	rb = b->e[0] * AbsR[7] + b->e[2] * AbsR[1];
	if (std::abs(t[0] * R[5] - t[2] * R[3]) > ra + rb) return NoCollision;
	// Test axis L = A1 x B2
	ra = a->e[0] * AbsR[8] + a->e[2] * AbsR[6];
	rb = b->e[0] * AbsR[4] + b->e[1] * AbsR[1];
	if (std::abs(t[0] * R[9] - t[2] * R[6]) > ra + rb) return NoCollision;
	// Test axis L = A2 x B0
	ra = a->e[0] * AbsR[1] + a->e[1] * AbsR[0];
	rb = b->e[1] * AbsR[8] + b->e[2] * AbsR[5];
	if (std::abs(t[1] * R[0] - t[0] * R[1]) > ra + rb) return NoCollision;
	// Test axis L = A2 x B1
	ra = a->e[0] * AbsR[4] + a->e[1] * AbsR[3];
	rb = b->e[0] * AbsR[8] + b->e[2] * AbsR[2];
	if (std::abs(t[1] * R[3] - t[0] * R[4]) > ra + rb) return NoCollision;
	// Test axis L = A2 x B2
	ra = a->e[0] * AbsR[7] + a->e[1] * AbsR[6];
	rb = b->e[0] * AbsR[5] + b->e[1] * AbsR[2];
	if (std::abs(t[1] * R[6] - t[0] * R[7]) > ra + rb) return NoCollision;
	// Since no separating axis is found, the OBBs must be intersecting
	return Collision;
}

CollisionGeo::CollisionResult CollisionGeo::DidOBBCollideWithOBB2(CollisionGeo* other)
{
	/*switch(DidCubeCollideWith(other))
	{
	case NoCollision: return NoCollision;break;
	case Arg1NotStaticOrOBBCube: return Arg1NotStaticOrOBBCube;break;
	case Arg2NotStaticOrOBBCube: return Arg2NotStaticOrOBBCube;break;
	} */

	for(int i = 0; i < 3; i++)
	{

		float Min0,Max0;
		Project(GetOBB2(),GetOBB2()->normalAxis[i], Min0, Max0);
		float Min1,Max1;
		Project(other->GetOBB2(),other->GetOBB2()->normalAxis[i], Min1, Max1);

		if(Max0<Min1 || Max1<Min0)
		{
			return NoCollision;
		}
	}
  	return Collision;
}

void CollisionGeo::Project(OBB2* obb,const D3DXVECTOR3& dir, float& min, float& max) const
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


