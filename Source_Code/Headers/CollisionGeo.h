#ifndef _COLLISIONGEO_H_
#define _COLLISIONGEO_H_

#include "ResourceManager.h"

struct OBB 
{
	D3DXVECTOR3 c; // OBB center point
	D3DXVECTOR3 u[3]; // Local x-, y-, and z-axes
	D3DXVECTOR3 e; // Positive halfwidth extents of OBB along each axis
};

struct OBB2 
{
	D3DXVECTOR3 centerPoint;
	float min;
	float max;
	D3DXVECTOR3 normalAxis[3]; // Local x-, y-, and z-axes
	D3DXVECTOR4 transformedVertexPoints[8];
};

class CollisionGeo
{
public:
	enum GeoType { OBBCube,StaticCube, Sphere , Capsule, Cylinder };
	enum CollisionResult { NoCollision, Collision, NotASphere, NotAOBBCube,NotAStaticCube,NotAStaticOrOBBCube, NotACapsule, NotACylinder,Arg1NotCylinder,Arg1NotStaticCube,Arg1NotOBBCube,Arg1NotStaticOrOBBCube,Arg1NotSphere,Arg1NotCapsule,Arg2NotCylinder,Arg2NotOBBCube,Arg2NotStaticOrOBBCube,Arg2NotStaticCube,Arg2NotSphere,Arg2NotCapsule};
	
	CollisionGeo(D3DXVECTOR3 cubePosition, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos); // Cube

//		   ________________	  URB
//	      /|			  /|
//		 / |			 / |
//		/__|____________/  |
//		|  |	        |  |
//		|  |	        |  |
//		|  |	        |  |
//		|  |____________|__|
//		|  /  		    |  /
//		| / 		    | /
//	LLF	|/______________|/

	 CollisionGeo(D3DXVECTOR3 cubePosition,D3DXVECTOR3 cubeRotation, D3DXVECTOR3 LowerLeftFrontPos,D3DXVECTOR3 UpperRightBackPos);
	 CollisionGeo(D3DXVECTOR3 spherePosition,	float radius); //Sphere;
	 CollisionGeo(D3DXVECTOR3 lowPosition, float height, float radius, bool isCapsule);	//Cylinder of Capsule
	 //CollisionGeo(D3DXVECTOR3 lowPosition, float height, float radius, float capheight); //capheight moet een halve sphere zijn, dus dit is dezelfde functie als de capsule, moet kijken hoe ik daar onderscheid in maak;

	inline float GetHeight() //Capsule / Cylinder
	{
		return height;
	}
	inline float GetRadius() // Capsule / Cylinder / Sphere
	{
		return radius;
	}
	inline D3DXVECTOR3 GetLowerLeftFrontPos()	//Cube
	{
		return LLFPos;
	}
	inline D3DXVECTOR3 GetUpperRightBackPos()  //Cube
	{
		return URBPos;
	}
	inline D3DXVECTOR3 GetPosition()
	{
		return position;
	}
	inline D3DXVECTOR3 GetCubeSize()
	{
		return D3DXVECTOR3(std::abs(LLFPos.x - URBPos.x),std::abs(LLFPos.y - URBPos.y),std::abs(LLFPos.z - URBPos.z));
	}
	inline OBB* GetOBB()
	{
		return &thisOBB;
	}
	inline OBB2* GetOBB2()
	{
		return &thisOBB2;
	}
	inline void SetPosition(D3DXVECTOR3 newValue)
	{
		position = newValue;
		thisOBB2.centerPoint= newValue;
	}
	inline void SetOBB(D3DXVECTOR3 newValue)
	{
		thisOBB.c = position;
		D3DXVec3Normalize(&thisOBB.u[0],&D3DXVECTOR3(rotation.x,0,0));
		D3DXVec3Normalize(&thisOBB.u[1],&D3DXVECTOR3(0,rotation.y,0));
		D3DXVec3Normalize(&thisOBB.u[2],&D3DXVECTOR3(0,0,rotation.z));
		thisOBB.e = newValue;
	}
	inline void SetRotation(D3DXVECTOR3 newValue)
	{
		rotation = newValue;
		thisOBB2.normalAxis[0] = D3DXVECTOR3(rotation.x+90,rotation.y,rotation.z);
		thisOBB2.normalAxis[1] = D3DXVECTOR3(rotation.x,rotation.y+90,rotation.z);
		thisOBB2.normalAxis[2] = D3DXVECTOR3(rotation.x,rotation.y,rotation.z+90);

		D3DXMATRIX worldMatrix;
		D3DXMATRIX m_Translation;
		D3DXMATRIX m_Rotation;
		D3DXMatrixRotationYawPitchRoll(&m_Rotation,rotation.x,rotation.y,rotation.z);
		D3DXMatrixTranslation(&m_Translation,position.x,position.y,position.z);
		D3DXMatrixMultiply(&worldMatrix,&m_Rotation,&m_Translation);

		/*thisOBB2.transformedVertexPoints[0] = D3DXVec3Transform(NULL,&D3DXVECTOR3(LLFPos.x, URBPos.y, LLFPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[1] = D3DXVec3Transform(NULL,&D3DXVECTOR3(URBPos.x, URBPos.y, LLFPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[2] = D3DXVec3Transform(NULL,&D3DXVECTOR3(LLFPos.x, LLFPos.y, LLFPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[3] = D3DXVec3Transform(NULL,&D3DXVECTOR3(URBPos.x, LLFPos.y, LLFPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[4] = D3DXVec3Transform(NULL,&D3DXVECTOR3(LLFPos.x, URBPos.y, URBPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[5] = D3DXVec3Transform(NULL,&D3DXVECTOR3(URBPos.x ,URBPos.y, URBPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[6] = D3DXVec3Transform(NULL,&D3DXVECTOR3(LLFPos.x, LLFPos.y, URBPos.z),&rotationMatrix);
		thisOBB2.transformedVertexPoints[7] = D3DXVec3Transform(NULL,&D3DXVECTOR3(URBPos.x, LLFPos.y, URBPos.z),&rotationMatrix);*/  //rotatie op nul basis


		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[0],&D3DXVECTOR3(LLFPos.x, URBPos.y, LLFPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[1],&D3DXVECTOR3(URBPos.x, URBPos.y, LLFPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[2],&D3DXVECTOR3(LLFPos.x, LLFPos.y, LLFPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[3],&D3DXVECTOR3(URBPos.x, LLFPos.y, LLFPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[4],&D3DXVECTOR3(LLFPos.x, URBPos.y, URBPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[5],&D3DXVECTOR3(URBPos.x ,URBPos.y, URBPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[6],&D3DXVECTOR3(LLFPos.x, LLFPos.y, URBPos.z),&worldMatrix);
		D3DXVec3Transform(&thisOBB2.transformedVertexPoints[7],&D3DXVECTOR3(URBPos.x, LLFPos.y, URBPos.z),&worldMatrix);  //rotatie op positie basis
			  

	}
	inline void ChangeCubeMetrics(D3DXVECTOR3 cubePosition,D3DXVECTOR3 LowerLeftFrontPos, D3DXVECTOR3 UpperRightBackPos)
	{
		position = cubePosition;
		LLFPos = LowerLeftFrontPos;
		URBPos = UpperRightBackPos;
	}
	inline void ChangeSphereMetrics(D3DXVECTOR3 spherePosition, float rad)
	{
		position = spherePosition;
		radius = rad;
	}
	inline void ChangeCapsuleMetrics(D3DXVECTOR3 capsulePosition, float capsuleHeight, float capsuleRadius)
	{
		position = capsulePosition;
		height = capsuleHeight;
		radius = capsuleRadius;
	}
	inline GeoType GetType()
	{
		return currentType;
	}
	CollisionResult IsInCube(D3DXVECTOR3 pos);
	CollisionResult IsInSphere(D3DXVECTOR3 pos);
	CollisionResult IsInCylinder(D3DXVECTOR3 pos);
	CollisionResult DidCubeCollideWith(CollisionGeo* other);
	CollisionResult DidSphereCollideWith(CollisionGeo* other);
	CollisionResult DidOBBCollideWithOBB(CollisionGeo* other);
	CollisionResult DidOBBCollideWithOBB2(CollisionGeo* other);

private:
	GeoType currentType;

	//general data
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	//cube data
	D3DXVECTOR3 LLFPos;
	D3DXVECTOR3 URBPos;

	//Sphere and capsule data
	float radius;
	float height;

	OBB thisOBB;
	OBB2 thisOBB2;

	inline float Vector3Distance(D3DXVECTOR3* a, D3DXVECTOR3* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2))+(pow(a->z,2) + pow(b->z,2)));
	}
	inline float Vector2Distance(D3DXVECTOR2* a, D3DXVECTOR2* b)
	{
		return sqrt((pow(a->x,2) + pow(b->x,2))+(pow(a->y,2) + pow(b->y,2)));
	}
	void CollisionGeo::Project(OBB2* obb,const D3DXVECTOR3& dir, float& min, float& max) const;
};


#endif