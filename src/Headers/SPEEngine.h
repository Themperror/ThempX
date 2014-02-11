#ifndef _SPEENGINE_H_
#define _SPEENGINE_H_

#include <SPE.h>
#include "ResourceManager.h"

class SPEEngine
{
public:
	struct RigidData
	{
		SPEVector scaleModel;
		SPEVector velocity;
		SPEVector angularVelocity;
		SPEVector position;
		bool isStatic;
		bool doRender;
		float mass;
		float density;
		void Nullify()
		{
			scaleModel =		SPEVector(1.0f,1.0f,1.0f);
			velocity =			SPEVector(0.0f,0.0f,0.0f);
			angularVelocity =	SPEVector(0.0f,0.0f,0.0f);
			position =			SPEVector(0.0f,0.0f,0.0f);
			isStatic = false;
			doRender = true;
			mass = 1000.0f;
			density = 1000.0f;
		}
	};
	struct RigidModel
	{
		bool doRender;
		ResourceManager::Model* model;
		LPSPERIGIDBODY rigidbody;
	};
	SPEEngine(ResourceManager* res);

	void CreatePhysicsObject(char* pathToMesh, SPEEngine::RigidData* data);

	void Release();

	void InitShape(ISPEShape* pShape, ID3DXMesh* pMesh);

	void InitApp();
	void CALLBACK OnFrameMove(float fElapsedTime);
	void CALLBACK OnFrameRender( IDirect3DDevice9* m_pd3dDevice );
private:
	HRESULT ScaleMesh(ID3DXMesh *pMesh, float scaleX,float scaleY,float scaleZ, D3DXVECTOR3* offset);
	ResourceManager* resources;
	
	LPSPEWORLD         pWorld; // interface of physics world

	//for cleanup
	std::vector<RigidModel> rigidbodies;
	std::vector<RigidModel> staticbodies;
	std::vector<LPSPESHAPE> shapes;
};
#endif